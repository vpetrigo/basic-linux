#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

int main(int argc, char *argv[]) {
    struct option opts[] = {
        {"query", required_argument, 0, 'q'},
        {"longinformationrequest", no_argument, 0, 0},
        {"version", no_argument, 0, 0},
        {0, 0, 0, 0}
    };

    int c = 0;
    int opt_index = 0;

    while ((c = getopt_long(argc, argv, "iq:v", opts, &opt_index)) >= 0) {
        switch (c) {
            case 'q':
            case 0:    
            case 'i':
            case 'v':
                break;
            default:
                printf("-");
                return EXIT_FAILURE;
        }
    }
    
    printf("opt_index: %d optind: %d, argc: %d\n", opt_index, optind, argc);
    if (optind < argc) {
        printf("-");
    }
    else {
        printf("+");
    }

    return EXIT_SUCCESS;
}
