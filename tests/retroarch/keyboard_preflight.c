/* Nonprompting access observation only; never request access or post events. */
#include <ApplicationServices/ApplicationServices.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc != 2 || strcmp(argv[1], "--preflight") != 0)
        return 2;
    int permitted = CGPreflightPostEventAccess();
    printf("post-event-access=%s\n", permitted ? "permitted" : "denied");
    return permitted ? 0 : 77;
}
