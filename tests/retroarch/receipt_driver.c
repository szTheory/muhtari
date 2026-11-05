/* Test driver for verifying that the bounded host launcher retains its receipt. */
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc != 13 || strcmp(argv[1], "--host") != 0)
        return 2;
    const char *shot = argv[11];
    static const struct {
        const char *suffix;
        const char *diagnostic;
    } failures[] = {
        {"sandbox-exec-failure.bmp", "states prelaunch stage=sandbox-exec failed\n"},
        {"sandbox-helper-failure.bmp", "states prelaunch stage=sandbox-helper failed\n"},
        {"work-deadline-failure.bmp", "states prelaunch stage=work-deadline failed\n"},
        {"config-port-failure.bmp", "states prelaunch stage=config-port failed\n"},
        {"runahead-config-failure.bmp", "states prelaunch stage=runahead-config failed\n"},
        {"endpoint-ownership-failure.bmp",
         "states: could not prove the unique owned RetroArch endpoint\n"},
    };
    for (size_t i = 0u; i < sizeof failures / sizeof failures[0]; i++) {
        if (strstr(shot, failures[i].suffix) != NULL) {
            fputs(failures[i].diagnostic, stderr);
            return 7;
        }
    }
    if (strstr(shot, "failure.bmp") != NULL) {
        puts("states host: MOCK_FAILURE output retained");
        fputs("receipt mock intentional host failure\n", stderr);
        return 7;
    }
    FILE *image = fopen(shot, "wb");
    if (image == NULL)
        return 3;
    static const char pixels[] = "mock picture bytes\n";
    int failed = fwrite(pixels, 1u, sizeof pixels - 1u, image) != sizeof pixels - 1u;
    if (fclose(image) != 0)
        failed = 1;
    if (failed)
        return 4;
    char save_path[4096];
    int save_length = snprintf(save_path, sizeof save_path, "%s/frontend-states.srm", argv[8]);
    if (save_length < 0 || (size_t)save_length >= sizeof save_path)
        return 5;
    FILE *save = fopen(save_path, "wb");
    if (save == NULL)
        return 6;
    unsigned char media[2048];
    memset(media, 0x5a, sizeof media);
    failed = fwrite(media, 1u, sizeof media, save) != sizeof media;
    if (fclose(save) != 0)
        failed = 1;
    if (failed)
        return 7;
    puts("states host: MOCK_SUCCESS explicit_advances=41/64 presented_budget=109/120");
    return 0;
}
