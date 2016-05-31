#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

void get_proc_name(const char * restrict stat_data, char *proc_name) {
    const char open_par = '(';
    const char close_par = ')';
    char *start_proc_name = strchr(stat_data, open_par);
    char *end_proc_name = strrchr(stat_data, close_par);

    if (!start_proc_name || !end_proc_name) {
        return;
    }
    // skip to the first char after the '('
    ptrdiff_t start_pos = start_proc_name - stat_data + 1;
    ptrdiff_t end_pos = end_proc_name - stat_data;

    memcpy(proc_name, stat_data + start_pos, end_pos - start_pos);
}

bool look_for_proc_name(int stat_file_fd, const char * restrict proc_name) {
    char *data = NULL;
    unsigned cur_data_size = 0;
    const unsigned chunk_size = 256;
    char data_chunk_buf[chunk_size];
    ssize_t bytes_read = 0;
    ssize_t total_n_bytes = 0;
    
    // read the whole stat file content
    while ((bytes_read = read(stat_file_fd, data_chunk_buf, chunk_size)) > 0) {
        if (cur_data_size == 0) {
            data = calloc(chunk_size, sizeof(char));
            cur_data_size = chunk_size;
        }
        else if (cur_data_size < total_n_bytes + bytes_read) {
            char *new_data = calloc(cur_data_size * 2, sizeof(char));
            // TODO: check pointer allocations
            memcpy(new_data, data, total_n_bytes);
            free(data);
            data = new_data;
            cur_data_size *= 2;
        }
        
        memcpy(data + total_n_bytes, data_chunk_buf, bytes_read);
        total_n_bytes += bytes_read;
    }
    
    // in case of error just return
    if (bytes_read != 0) {
        return false;
    }
    
    // assume that file name cannot be greater than 256 bytes
    char readed_proc[256] = {0};
    
    get_proc_name(data, readed_proc);
    
    bool is_equal = strcmp(proc_name, readed_proc) == 0;
    free(data);

    return is_equal;
}

// build path to the stat file in the format
// /proc/@d_name/stat and write it to @path
void build_stat_path(const char *d_name, char *path) {
    const char *proc = "/proc/";
    const char *stat = "/stat";

    strncat(path, proc, strlen(proc));
    strncat(path, d_name, strlen(d_name));
    strncat(path, stat, strlen(stat));
}

// function for searching process with name @proc_name 
// in the system procfs structure
int count_n_proc(const char * restrict proc_name) {
    const char *proc_folder = "/proc";

    DIR *proc_dir = opendir(proc_folder);
    
    if (!proc_dir) {
        perror("Cannot open /proc");
        _Exit(EXIT_FAILURE);
    }

    struct dirent *dent = NULL;
    unsigned long counter = 0;

    while ((dent = readdir(proc_dir)) != NULL) {
        // if this is not a directory do nothing
        if (dent->d_type != DT_DIR) {
            continue;
        }
        // here we might use smaller buf as
        // /proc/ - 6 bytes
        // <max pid> for x64 ~4200000 - 7 bytes
        // /stat - 5 bytes
        char stat_file_path[24] = {0};

        build_stat_path(dent->d_name, stat_file_path);

        int stat_fd = open(stat_file_path, O_RDONLY);
        
        if (stat_fd > 0 && look_for_proc_name(stat_fd, proc_name)) {
            ++counter;
        }
    }

    return counter;
}

int main() {
    // process name to search in procfs
    const char *proc_name = "genenv";
    int work_processes = count_n_proc(proc_name);

    printf("%d\n", work_processes);

    return 0;
}
