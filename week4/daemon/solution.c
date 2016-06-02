#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

void daemon_creation(void) {
    pid_t sid = setsid();

    if (sid == -1) {
        perror("Cannot assign session ID");
        _Exit(EXIT_FAILURE);
    }

    if (chdir("/") == -1) {
        perror("Cannot change directory to the root '/'");
        _Exit(EXIT_FAILURE);
    }

    umask(0);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main() {
    pid_t child_pid;

    if ((child_pid = fork()) > 0) {
        printf("%d\n", child_pid);
        _Exit(EXIT_SUCCESS);
    }
    else {
        daemon_creation();
        pause();
    }
}
