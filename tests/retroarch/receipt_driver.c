/* Test driver for verifying that the bounded host launcher retains its receipt. */
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc != 13 || strcmp(argv[1], "--host") != 0)
        return 2;
    const char *shot = argv[11];
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
    puts("states host: MOCK_SUCCESS explicit_advances=41/64 presented_budget=109/120");
    return 0;
}
