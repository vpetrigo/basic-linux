#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

// as a directory name in Linux cannot be greater than 256
// characters
#define MAX_INPUT_SIZE 256

// concatenate two directory names like '@base_dir/@add_dir'
char *concat_dir(const char * restrict base_dir, const char * restrict add_dir) {
    assert(base_dir != NULL);
    assert(add_dir != NULL);

    size_t bd_size = strlen(base_dir);
    size_t ad_size = strlen(add_dir);
    char *full_dir = calloc(bd_size + ad_size + 2, sizeof(char));
    
    strncat(full_dir, base_dir, bd_size);
    full_dir[bd_size] = '/';
    strncat(full_dir, add_dir, ad_size);

    return full_dir;
}

// freed memory allocated for process's path string
void free_process_path(char *proc_path) {
    free(proc_path);
}

// Open directory named by @process_path
FILE *find_stat_file(const char *process_path) {
    assert(process_path != NULL);
    DIR *dir = NULL;
    
    if ((dir = opendir(process_path)) == NULL) {
        perror("Cannot open dir");
        _Exit(EXIT_FAILURE);
    }

    struct dirent *dinfo = NULL;
    const char *stat_name = "stat";
    FILE *stat_file = NULL;

    while ((dinfo = readdir(dir)) != NULL &&
                strcmp(dinfo->d_name, stat_name)) {
    }

    if (dinfo != NULL) {
        char *stat_file_dir = concat_dir(process_path, "stat");

        stat_file = fopen(stat_file_dir, "r");

        if (!stat_file) {
            perror("Cannot open stat file");
            _Exit(EXIT_FAILURE);
        }

        free_process_path(stat_file_dir);
    }

    closedir(dir);

    return stat_file;
}

void get_ppid(const char *process_path) {
    assert(process_path != NULL);

    FILE *stat_file = find_stat_file(process_path);
    if (!stat_file) {
        perror("No stat_file");
        _Exit(EXIT_FAILURE);
    }

    char ppid[16];

    fscanf(stat_file, "%*s %*s %*s %s", ppid);
    puts(ppid);
}

void delete_newline(char *arr, size_t len) {
    if (isspace(arr[len - 1])) {
        arr[len - 1] = '\0';
    }
}

int main() {
    const char *procfs_dir = "/proc";
    char proc_id[MAX_INPUT_SIZE];

    if (fgets(proc_id, MAX_INPUT_SIZE, stdin) == NULL) {
        perror("Cannot get string");
        _Exit(EXIT_FAILURE);
    }
    
    delete_newline(proc_id, strlen(proc_id));

    char *process_path = concat_dir(procfs_dir, proc_id);
    
    get_ppid(process_path);
    free_process_path(process_path);

    return 0;
}
