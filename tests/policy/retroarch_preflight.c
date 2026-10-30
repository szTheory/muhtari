/* Exercise the real helper with a deterministic permission API, without posting. */
int preflight_main(int argc, char **argv);
#define main preflight_main
#include "../retroarch/keyboard_preflight.c"
#undef main

static int permitted, calls;

int CGPreflightPostEventAccess(void)
{
    calls++;
    return permitted;
}

int main(void)
{
    char *valid[] = {"keyboard-preflight", "--preflight", NULL};
    char *invalid[] = {"keyboard-preflight", "--request", NULL};
    if (preflight_main(0, NULL) != 2 || preflight_main(1, valid) != 2 ||
        preflight_main(3, valid) != 2 || preflight_main(2, invalid) != 2 || calls != 0)
        return 1;
    if (preflight_main(2, valid) != 77 || calls != 1)
        return 1;
    permitted = 1;
    return preflight_main(2, valid) != 0 || calls != 2;
}
