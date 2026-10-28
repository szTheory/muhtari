/* The exit statuses of muhtari-run, defined once. --help prints this table
   and docs/runner.md repeats it; runner.exit.table keeps the two equal. A
   status enters the table when the runner first returns it. */
#ifndef MUHTARI_RUNNER_EXIT_STATUS_H
#define MUHTARI_RUNNER_EXIT_STATUS_H

enum {
    MUHTARI__EXIT_DONE = 0,
    MUHTARI__EXIT_FAILURE = 1,
    MUHTARI__EXIT_USAGE = 2,
    MUHTARI__EXIT_SAVE_SIZE = 4,
    MUHTARI__EXIT_REFUSED = 77
};

typedef struct muhtari__exit_status {
    int code;
    const char *name;
    const char *meaning;
} muhtari__exit_status;

static const muhtari__exit_status muhtari__exit_statuses[] = {
    {MUHTARI__EXIT_DONE, "done",
     "every frame ran, or SIGINT or SIGTERM stopped the run after a whole frame"},
    {MUHTARI__EXIT_FAILURE, "failure",
     "a file could not be read or written, an image or a movie is malformed, truncated or too "
     "large, a BIOS is invalid, a movie names another cartridge or BIOS, memory ran out or the "
     "core failed"},
    {MUHTARI__EXIT_USAGE, "usage",
     "the command line is invalid or conflicts with the movie's header, so nothing ran"},
    {MUHTARI__EXIT_SAVE_SIZE, "save-size", "the save file is larger than the attached battery RAM"},
    {MUHTARI__EXIT_REFUSED, "refused",
     "the image is a 2600 cartridge, or uses a board, device or region this version does not "
     "emulate, "
     "or the BIOS rejected it"},
};

#endif /* MUHTARI_RUNNER_EXIT_STATUS_H */
