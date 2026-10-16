/* The runner's stop on SIGINT or SIGTERM. Runner-only: the core never sees
   a signal. */
#ifndef MUHTARI_RUNNER_STOP_H
#define MUHTARI_RUNNER_STOP_H

/* Installs the handlers. Call once, after the command line is valid. */
void muhtari__stop_install(void);

/* 1 once SIGINT or SIGTERM has arrived, else 0. */
int muhtari__stop_requested(void);

#endif /* MUHTARI_RUNNER_STOP_H */
