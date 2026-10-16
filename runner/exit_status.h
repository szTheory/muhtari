/* The exit statuses of muhtari-run, defined once. --help prints this table
   and docs/runner.md repeats it; runner.exit.table keeps the two equal. A
   status enters the table when the runner first returns it. */
#ifndef MUHTARI_RUNNER_EXIT_STATUS_H
#define MUHTARI_RUNNER_EXIT_STATUS_H

enum { MUHTARI__EXIT_DONE = 0, MUHTARI__EXIT_FAILURE = 1, MUHTARI__EXIT_USAGE = 2 };

typedef struct muhtari__exit_status {
    int code;
    const char *name;
    const char *meaning;
} muhtari__exit_status;

static const muhtari__exit_status muhtari__exit_statuses[] = {
    {MUHTARI__EXIT_DONE, "done",
     "every frame ran, or SIGINT or SIGTERM stopped the run after a whole frame"},
    {MUHTARI__EXIT_FAILURE, "failure",
     "a file could not be written, memory ran out or the core failed"},
    {MUHTARI__EXIT_USAGE, "usage", "the command line is invalid, so nothing ran"},
};

#endif /* MUHTARI_RUNNER_EXIT_STATUS_H */
