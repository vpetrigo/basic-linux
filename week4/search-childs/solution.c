#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#define CHUNK_SIZE  256
// Convert pid number to string
// in: @pid - int number represeing process ID
// out: @str_repr - where to store PID representation
void pid_to_str(int pid, char *str_repr) {
    assert(str_repr);
    sprintf(str_repr, "%d", pid);
}

// Concatenate strings with a dir name in the right format
// example: concat_path(path, 5, "usr", "local", "etc", "nginx", "conf.d") ->
// -> /usr/local/etc/nginx/conf.d
// in: @q_path - number of variable list size
//     @... - arguments to concatenate
// out: @path - pointer for result string storage
void concat_path(char *path, int q_path, ...) {
    assert(path);
    va_list args;
    va_start(args, q_path);
    int counter = 0;
    int bytes_written = 0;

    while (counter < q_path) {
        const char *dir = va_arg(args, const char *);
        sprintf(path + bytes_written, "/%s", dir);
        bytes_written += strlen(dir) + 1;
        ++counter;
    }
}

// read the content of the stat file
// in: @stat_file_fd - descriptor of a stat file. Must be opened!
// return: @data - pointer to the content of stat file. Must be freed after use!
char *get_stat_content(int stat_file_fd) {
    char *data = NULL;
    unsigned cur_data_size = 0;
    char data_chunk_buf[CHUNK_SIZE];
    ssize_t bytes_read = 0;
    ssize_t total_n_bytes = 0;
    
    // read the whole stat file content
    while ((bytes_read = read(stat_file_fd, data_chunk_buf, CHUNK_SIZE)) > 0) {
        if (cur_data_size == 0) {
            data = calloc(CHUNK_SIZE, sizeof(char));
            assert(data);
            cur_data_size = CHUNK_SIZE;
        }
        else if (cur_data_size < total_n_bytes + bytes_read) {
            char *new_data = calloc(cur_data_size * 2, sizeof(char));
            assert(new_data);
            memcpy(new_data, data, total_n_bytes);
            free(data);
            data = new_data;
            cur_data_size *= 2;
        }
        
        memcpy(data + total_n_bytes, data_chunk_buf, bytes_read);
        total_n_bytes += bytes_read;
    }
    // in case of error just return
    if (bytes_read < 0) {
        return NULL;
    }
    
    return data;
}

// get PPID from the stat file
// in: @stat_file_fd - file descriptor of a stat file to be read
// return: @PPID
int get_ppid(int stat_file_fd) {
    char *stat_content = get_stat_content(stat_file_fd);
    assert(stat_content);
    // as only process name has parentheses around use that fact to 
    // find the end of it and read status and PPID
    const char close_br = ')';
    char *end_proc_name = strrchr(stat_content, close_br);
    assert(end_proc_name);
    ptrdiff_t end_proc_name_pos = end_proc_name - stat_content + 1;
    int ppid;
    
    sscanf(stat_content + end_proc_name_pos, " %*c %d", &ppid);
    free(stat_content);

    return ppid;
}

int find_children(int ppid) {
    // count process themselve
    int child_counter = 1;

    DIR *dir = opendir("/proc");

    if (!dir) {
        perror("Cannot open /proc folder");
        _Exit(EXIT_FAILURE);
    }

    struct dirent *dent = NULL;
    char str_ppid[7] = {0};
    pid_to_str(ppid, str_ppid);

    while ((dent = readdir(dir)) != NULL) {
        int dir_pid = atoi(dent->d_name);
        // here we want to check only dirs that may contain stat file
        // and we do not want to check dirs with PID less than current ppid
        if (dent->d_type == DT_DIR && dir_pid != 0 &&
                dir_pid > ppid) {
            // build path to open
            char stat_path[18] = {0};
            concat_path(stat_path, 3, "proc", dent->d_name, "stat");
            int stat_file = open(stat_path, O_RDONLY);
            
            if (stat_file == -1) {
                fprintf(stderr, "Cannot open file %s\n", stat_path);
                perror("Bad file");
                _Exit(EXIT_FAILURE);
            }
            
            int process_ppid = get_ppid(stat_file);
            
            // if the current process has the PPID == PID
            if (ppid == process_ppid) {
                // find his children and count them
                child_counter += find_children(dir_pid);
            }

            close(stat_file);
        } 
    }
    
    return child_counter;
}

int main(int argc, char *argv[]) {
    if (argc == 1 || argc > 2) {
        fprintf(stderr, "Wrong number of arguments\n");
        fprintf(stderr, "Usage: solution <pid>\n");
        _Exit(EXIT_FAILURE);
    }
    
    int pid = atoi(argv[1]);
    printf("%d\n", find_children(pid));

    return 0;
}
