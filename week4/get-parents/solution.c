#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#define PRINT_INT(x) ({ __typeof__(x) _x = (x); printf("%d\n", _x); })

void get_pid_path(int pid, char *pid_path) {
    const char *proc_path = "/proc/";
    const char *stat = "/stat";
    char str_pid[6] = {0};
    
    sprintf(str_pid, "%d", pid);
    strncat(pid_path, proc_path, strlen(proc_path));
    strncat(pid_path, str_pid, strlen(str_pid));
    strncat(pid_path, stat, strlen(stat));
}

char *read_stat_content(int stat_fd) {
    char *data = NULL;
    const unsigned chunk_size = 256;
    char chunk_buf[chunk_size];
    unsigned data_size = 0;
    int readed_bytes = 0;
    unsigned total_readed = 0;

    while ((readed_bytes = read(stat_fd, chunk_buf, chunk_size)) > 0) {
        if (!data) {
            data = calloc(chunk_size, sizeof(char));
            data_size = chunk_size;
        }
        else if (data_size < total_readed + readed_bytes) {
            char *new_data = calloc(data_size * 2, sizeof(char));

            memcpy(new_data, data, data_size);
            free(data);
            data = new_data;
            data_size *= 2;
        }

        memcpy(data + total_readed, chunk_buf, readed_bytes);
        total_readed += readed_bytes;
    }

    if (readed_bytes < 0) {
        perror("Error during reading occured");
        _Exit(EXIT_FAILURE);
    }

    return data;
}

void free_stat_content(char *data) {
    if (data) {
        free(data);
    }
}

int get_ppid(const char * restrict stat_content) {
    const char close_pr = ')';
    int ppid;
    char *end_proc_name = strrchr(stat_content, close_pr);
    if (!end_proc_name) {
        _Exit(EXIT_FAILURE);
    }

    ptrdiff_t end_of_proc_name = end_proc_name - stat_content;
    sscanf(stat_content + end_of_proc_name + 1, " %*c %d", &ppid);
    
    return ppid;
}

int get_parent_pid(int pid) {
    char pid_path[18] = {0};

    get_pid_path(pid, pid_path);
    
    int stat_file = open(pid_path, O_RDONLY);

    if (!stat_file) {
        fprintf(stderr, "Cannot open /proc/%d/stat", pid);
        perror("Bad directory");
        _Exit(EXIT_FAILURE);
    }

    char *stat_content = read_stat_content(stat_file);
    int ppid = get_ppid(stat_content);
    free_stat_content(stat_content);
    close(stat_file);

    return ppid;
}

void print_proc_parents(int pid) {
    PRINT_INT(pid);
    if (pid == 1) {
        return;
    }
    
    int ppid = get_parent_pid(pid);

    print_proc_parents(ppid);
}

int main(int argc, char *argv[]) {
    if (argc == 1 || argc > 2) {
        fprintf(stderr, "Bad arguments. Only one pid acceptable");
        fprintf(stderr, "Usage: solution <pid>");
        _Exit(EXIT_FAILURE);
    }

    int proc_pid = atoi(argv[1]);

    print_proc_parents(proc_pid);

    return 0;
}
