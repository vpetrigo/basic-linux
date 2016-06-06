#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>

#define max(a,b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a > _b ? _a : _b; })

#define IS_BOTH_CLOSED(x)   (((x) & 0x3) == 0x3)
#define MAX_NUM_SIZE    32

int main() {
    int in1 = open("in1", O_RDONLY | O_NONBLOCK);
    int in2 = open("in2", O_RDONLY | O_NONBLOCK);
    assert((in1 != -1) && (in2 != -1));
    int max_fd = max(in1, in2);

    fd_set read_fd;
    FD_ZERO(&read_fd);
    // set file descriptors in the reader set
    FD_SET(in1, &read_fd);
    FD_SET(in2, &read_fd);
    // flag for controlling file close
    short both_closed = 0;
    char buf[MAX_NUM_SIZE];
    bzero(buf, MAX_NUM_SIZE);
    int sum = 0;

    while (!IS_BOTH_CLOSED(both_closed)) {
        fd_set temp_set = read_fd;
        int retval = select(max_fd + 1, &temp_set, NULL, NULL, NULL);

        if (retval == -1) {
            perror("select error");
            exit(EXIT_FAILURE);
        }

        for (size_t i = 0; i < max_fd + 1; ++i) {
            if (FD_ISSET(i, &temp_set)) {
                retval = read(i, buf, MAX_NUM_SIZE);

                if (retval == -1) {
                    perror("File read error");
                    exit(EXIT_FAILURE);
                }
                else if (retval == 0) {
                    // file closed
                    assert(close(i) != -1);
                    FD_CLR(i, &read_fd);
                    both_closed += (!both_closed) ? 1 : 2;
                    if (i == max_fd) {
                        --max_fd;
                    }
                }

                sum += atoi(buf);
                bzero(buf, MAX_NUM_SIZE); 
            }
        }
    }

    printf("%d\n", sum);

    return 0;
}
