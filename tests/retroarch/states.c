/* Portable owned-child state controls; production host calls are added below. */
#include <stdio.h>
#include <string.h>

typedef struct mock_session {
    unsigned progress;
    unsigned picture_generation;
    int child_owned;
} mock_session;

static int run_save_load(mock_session *session)
{
    (void)session;
    return -1;
}

static int run_controls(void)
{
    mock_session session = {.progress = 12u, .picture_generation = 4u, .child_owned = 1};
    int result = run_save_load(&session);
    int passed = result == 0 && session.progress == 12u && session.picture_generation >= 6u &&
                 session.child_owned == 0;

    puts("TAP version 13");
    puts("1..1");
    if (passed) {
        puts("ok 1 - owned save load restores progress and judges fresh replay");
        return 0;
    }
    printf("not ok 1 - owned save load restores progress and judges fresh replay\n"
           "  ---\n  error: valid mock sequence did not finish (result=%d progress=%u picture=%u child=%d)\n"
           "  ...\n",
           result, session.progress, session.picture_generation, session.child_owned);
    return 1;
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--controls") == 0)
        return run_controls();
    fprintf(stderr, "usage: states --controls\n");
    return 2;
}
