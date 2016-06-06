#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SEG_SIZE 1000
#define NUM_Q 100

key_t sum_to_regions(const int *seg1, const int *seg2) {
    // TODO: do all necessary sanity checks during work flow
    // create new file for shared memory storage
    const char *filename = "/tmp/shared_mame_seg";
    int fd = creat(filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    assert(fd != -1);
    // obtain file token
    key_t new_mem = ftok(filename, 'm');
    // get new shared memory segment
    int new_seg = shmget(new_mem, SEG_SIZE, IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    assert(new_seg != -1);
    int *num_in_new_seg = shmat(new_seg, 0, 0);
    assert(num_in_new_seg != (int *)(-1));
    int *traverse_it = num_in_new_seg;

    // read numbers from seg1 and seg2 and store the sum result into
    // the new shared memory segment
    for (size_t i = 0; i < NUM_Q; ++i) {
        *traverse_it++ = *seg1++ + *seg2++;
    }

    shmdt(num_in_new_seg);

    return new_mem;
}

int main(int argc, char *argv[]) {
    assert(argc == 3);
    // read args that specify shared memory keys 
    key_t first_seg = atoi(argv[1]);
    key_t second_seg = atoi(argv[2]);
    // access to the shared segments
    int seg1 = shmget(first_seg, SEG_SIZE, 0);
    int seg2 = shmget(second_seg, SEG_SIZE, 0);

    assert(seg1 != -1 && seg2 != -1);

    // get pointers to the start of segments
    int *num_in_seg1 = shmat(seg1, 0, 0);
    int *num_in_seg2 = shmat(seg2, 0, 0);

    assert(num_in_seg1 != (int *)(-1) && num_in_seg2 != (int *)(-1));
    key_t new_mem = sum_to_regions(num_in_seg1, num_in_seg2);
    
    printf("%ld\n", new_mem);

    return 0;
}
