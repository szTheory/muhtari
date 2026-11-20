/* Fault-injection driver for the installed quickstart; never built for users. */
#include <muhtari.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { QUICKSTART_FAULT_MAX_ALLOCS = 64 };

typedef struct quickstart_fault_allocation {
    void *pointer;
    const char *name;
    int released;
} quickstart_fault_allocation;

static quickstart_fault_allocation quickstart_fault_allocations[QUICKSTART_FAULT_MAX_ALLOCS];
static size_t quickstart_fault_allocation_count;
static size_t quickstart_fault_nonnull_free_count;
static size_t quickstart_fault_buffer_attempts;
static size_t quickstart_fault_buffer_successes;
static size_t quickstart_fault_create_allocator_attempts;
static int quickstart_fault_harness_error;
static int quickstart_fault_in_create;
static int quickstart_fault_in_frame;
static int quickstart_fault_created;
static int quickstart_fault_injected;
static int quickstart_fault_frame_rejected;
static int quickstart_fault_frame_succeeded;
static int quickstart_fault_sample_count_changed;
static int quickstart_fault_frame_call_count;
static const char *quickstart_fault_mode;

static void *quickstart_fault_malloc(size_t size);
static void quickstart_fault_free(void *pointer);
static muhtari_status quickstart_fault_create(const muhtari_config *config, muhtari **out);
static muhtari_status quickstart_fault_run_frame(muhtari *instance, muhtari_frame *frame);

#define main quickstart_subject
#define malloc(size) quickstart_fault_malloc(size)
#define free(pointer) quickstart_fault_free(pointer)
#define muhtari_create quickstart_fault_create
#define muhtari_run_frame quickstart_fault_run_frame
#include "../../examples/embedding/quickstart.c"
#undef muhtari_run_frame
#undef muhtari_create
#undef free
#undef malloc
#undef main

static int quickstart_fault_is_mode(const char *mode)
{
    return strcmp(quickstart_fault_mode, mode) == 0;
}

static void *quickstart_fault_malloc(size_t size)
{
    void *pointer;
    const char *name = "core";

    if (quickstart_fault_in_create) {
        name = "instance";
        quickstart_fault_create_allocator_attempts++;
        if (quickstart_fault_is_mode("create") &&
            quickstart_fault_create_allocator_attempts == 1u) {
            quickstart_fault_injected = 1;
            return NULL;
        }
    } else if (quickstart_fault_created && !quickstart_fault_in_frame) {
        quickstart_fault_buffer_attempts++;
        name = quickstart_fault_buffer_successes == 0u ? "video" : "audio";
        if (quickstart_fault_is_mode("audio_alloc") &&
            quickstart_fault_buffer_attempts == 2u) {
            quickstart_fault_injected = 1;
            return NULL;
        }
    }

    if (quickstart_fault_allocation_count >= QUICKSTART_FAULT_MAX_ALLOCS) {
        quickstart_fault_harness_error = 1;
        return NULL;
    }

    pointer = malloc(size);
    if (pointer != NULL) {
        quickstart_fault_allocations[quickstart_fault_allocation_count].pointer = pointer;
        quickstart_fault_allocations[quickstart_fault_allocation_count].name = name;
        quickstart_fault_allocations[quickstart_fault_allocation_count].released = 0;
        quickstart_fault_allocation_count++;
        if (quickstart_fault_created && !quickstart_fault_in_frame)
            quickstart_fault_buffer_successes++;
    }
    return pointer;
}

static void quickstart_fault_free(void *pointer)
{
    size_t i;

    if (pointer == NULL)
        return;
    quickstart_fault_nonnull_free_count++;
    for (i = 0u; i < quickstart_fault_allocation_count; i++) {
        if (quickstart_fault_allocations[i].pointer == pointer) {
            if (quickstart_fault_allocations[i].released != 0) {
                quickstart_fault_harness_error = 1;
                return;
            }
            quickstart_fault_allocations[i].released = 1;
            return;
        }
    }
    quickstart_fault_harness_error = 1;
}

static muhtari_status quickstart_fault_create(const muhtari_config *config, muhtari **out)
{
    muhtari_status status;

    quickstart_fault_in_create = 1;
    status = muhtari_create(config, out);
    quickstart_fault_in_create = 0;
    quickstart_fault_created = status == MUHTARI_STATUS_OK;
    return status;
}

static muhtari_status quickstart_fault_run_frame(muhtari *instance, muhtari_frame *frame)
{
    muhtari_frame injected = *frame;
    muhtari_status status;

    quickstart_fault_frame_call_count++;
    if (quickstart_fault_created == 0 || frame->video == NULL || frame->audio == NULL ||
        quickstart_fault_buffer_successes != 2u) {
        quickstart_fault_harness_error = 1;
    }

    quickstart_fault_in_frame = 1;
    if (quickstart_fault_is_mode("frame"))
        injected.audio_capacity = 0u;
    status = muhtari_run_frame(instance, &injected);
    quickstart_fault_in_frame = 0;

    if (quickstart_fault_is_mode("frame") &&
        status == MUHTARI_STATUS_ERR_BUFFER_TOO_SMALL) {
        quickstart_fault_injected = 1;
        quickstart_fault_frame_rejected = 1;
    }
    if (quickstart_fault_is_mode("sample_count") && status == MUHTARI_STATUS_OK) {
        injected.audio_count = 799u;
        quickstart_fault_injected = 1;
        quickstart_fault_sample_count_changed = 1;
    }
    if (status == MUHTARI_STATUS_OK)
        quickstart_fault_frame_succeeded = 1;
    frame->audio_count = injected.audio_count;
    return status;
}

static int quickstart_fault_require(int condition, const char *message)
{
    if (condition)
        return 1;
    fprintf(stderr, "quickstart fault harness: %s\n", message);
    return 0;
}

int main(int argc, char **argv)
{
    size_t i;
    int subject_result;
    int checks_ok = 1;
    int mode_known;

    if (argc != 2) {
        fputs("quickstart fault harness: expected one mode\n", stderr);
        return 2;
    }
    quickstart_fault_mode = argv[1];
    mode_known = quickstart_fault_is_mode("create") || quickstart_fault_is_mode("audio_alloc") ||
                 quickstart_fault_is_mode("frame") || quickstart_fault_is_mode("sample_count");
    if (!mode_known) {
        fputs("quickstart fault harness: unknown mode\n", stderr);
        return 2;
    }

    subject_result = quickstart_subject();
    checks_ok &= quickstart_fault_require(subject_result == EXIT_FAILURE,
                                          "example did not return EXIT_FAILURE");
    checks_ok &= quickstart_fault_require(quickstart_fault_injected != 0,
                                          "requested fault was not injected");

    if (quickstart_fault_is_mode("create")) {
        checks_ok &= quickstart_fault_require(quickstart_fault_created == 0 &&
                                              quickstart_fault_create_allocator_attempts == 1u,
                                              "create fault did not reject the first allocation");
    } else {
        checks_ok &= quickstart_fault_require(quickstart_fault_created != 0,
                                              "instance creation did not complete");
    }
    if (quickstart_fault_is_mode("audio_alloc")) {
        checks_ok &= quickstart_fault_require(quickstart_fault_buffer_attempts == 2u &&
                                              quickstart_fault_buffer_successes == 1u,
                                              "audio allocation fault did not follow video allocation");
    }
    if (quickstart_fault_is_mode("frame")) {
        checks_ok &= quickstart_fault_require(quickstart_fault_frame_rejected != 0 &&
                                              quickstart_fault_frame_call_count == 1,
                                              "real frame call did not reject zero audio capacity");
    }
    if (quickstart_fault_is_mode("sample_count")) {
        checks_ok &= quickstart_fault_require(quickstart_fault_frame_succeeded != 0 &&
                                              quickstart_fault_sample_count_changed != 0 &&
                                              quickstart_fault_frame_call_count == 1,
                                              "sample count fault did not follow a successful frame");
    }
    checks_ok &= quickstart_fault_require(quickstart_fault_harness_error == 0,
                                          "allocator tracking reported an invalid operation");
    checks_ok &= quickstart_fault_require(quickstart_fault_nonnull_free_count ==
                                          quickstart_fault_allocation_count,
                                          "not every acquired allocation was released exactly once");
    for (i = 0u; i < quickstart_fault_allocation_count; i++) {
        if (quickstart_fault_allocations[i].released == 0) {
            fprintf(stderr, "quickstart fault harness: %s allocation was not released\n",
                    quickstart_fault_allocations[i].name);
            checks_ok = 0;
        }
        free(quickstart_fault_allocations[i].pointer);
        quickstart_fault_allocations[i].pointer = NULL;
    }

    if (!checks_ok)
        return 2;
    puts("quickstart fault: cleanup verified");
    return EXIT_FAILURE;
}
