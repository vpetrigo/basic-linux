#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>

volatile sig_atomic_t counter_usr1 = 0;
volatile sig_atomic_t counter_usr2 = 0;
volatile sig_atomic_t done = 0;

void usr1_handler(int signum) {
    ++counter_usr1;
}

void usr2_handler(int signum) {
    ++counter_usr2;
}

void term_handler(int signum) {
    done = 1;
}

int main() {
    sigset_t mask;

    sigemptyset(&mask);

    struct sigaction act = {
        .sa_handler = usr1_handler,
        .sa_mask = 0,
        .sa_flags = 0
    };

    sigaction(SIGUSR1, &act, NULL);
    act.sa_handler = usr2_handler;
    sigaction(SIGUSR2, &act, NULL);
    act.sa_handler = term_handler;
    sigaction(SIGTERM, &act, NULL);

    while (!done) {
        sigsuspend(&mask);
    }

    printf("%d %d\n", counter_usr1, counter_usr2);

    return 0;
}
