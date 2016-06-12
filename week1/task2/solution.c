#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <dlfcn.h>

// typedef for a pointer to a function from an external library
typedef int (*ext_fun)(int);

enum program_argv_t {
    LIB_NAME = 1,
    FUN_NAME,
    VALUE
};

bool load_ext_fun(const char *lib_name, const char *fun_name, ext_fun *fptr) {
    void *lib_ptr = dlopen(lib_name, RTLD_LAZY);

    if (!lib_ptr) {
        perror("Cannot find the library");
        return false;
    }
    
    *fptr = dlsym(lib_ptr, fun_name);

    if (!fptr) {
        perror("Cannot load the function");
        return false;
    }

    return true;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        _Exit(EXIT_FAILURE);
    }

    ext_fun function = NULL;

    if (load_ext_fun(argv[LIB_NAME], argv[FUN_NAME], &function)) {
        printf("%d\n", function(atoi(argv[VALUE])));
    }

    return EXIT_SUCCESS;
}
