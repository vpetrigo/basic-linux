#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define TASK_ARGS_Q 3
#define CMD_SIZE    512

int main(int argc, char *argv[]) {
    assert(argc == TASK_ARGS_Q);
    char cmd[CMD_SIZE] = {0};

    for (size_t i = 1; i < argc; ++i) {
        strncat(cmd, argv[i], strlen(argv[i]));
        cmd[strlen(cmd)] = ' ';
        cmd[strlen(cmd) + 1] = '\0';
    }
    
    FILE *proc_pipe = popen(cmd, "r");
    
    if (!proc_pipe) {
        perror("Cannot call the process");
        exit(EXIT_FAILURE);
    }
    
    char ch = '\0';
    size_t counter = 0;
    
    while ((ch = getc(proc_pipe)) != EOF) {
       if (ch == '0') {
            ++counter;
       }
    }

    printf("%zu\n", counter);

    if (pclose(proc_pipe) == -1) {
        perror("Cannot close called process pipe");
        exit(EXIT_FAILURE);
    }

    return 0;
}
