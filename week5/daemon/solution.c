#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

volatile sig_atomic_t done = 0;

void sigurg_handler(int signum) {
    done = 1; 
}

void daemon_init(void) {
    int retval = setsid();
    
    if (retval == -1) {
        perror("Cannot create a new SID");
        exit(EXIT_FAILURE);
    }
    
    umask(0);
    retval = chdir("/");
    if (retval == -1) {
        perror("Cannot change directory to the '/'");
        exit(EXIT_FAILURE);
    }
    
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void init_signal(void) {
    struct sigaction act = {
        .sa_handler = sigurg_handler
    };

    int retval = sigaction(SIGURG, &act, NULL);
    if (retval == -1) {
        exit(EXIT_FAILURE);
    }
}

int main() {
    pid_t ch_pid;

    if ((ch_pid = fork()) > 0) {
        printf("%d\n", ch_pid);
        exit(EXIT_SUCCESS);
    }
    else {
        // child branch
        sigset_t mask;

        sigemptyset(&mask);
        daemon_init();
        init_signal();

        while (!done) {
            sigsuspend(&mask);
        }
    }

    return 0;
}
