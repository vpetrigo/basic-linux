#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int master_socket(void) {
    int ms = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if (ms == -1) {
        perror("Cannot get TCP socket");
        exit(EXIT_FAILURE);
    }

    return ms;
}

void bind_to_port(int master_fd, int port) {
    const struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = INADDR_ANY
    };

    int status = bind(master_fd, (const struct sockaddr *) &addr, sizeof(addr));

    if (status == -1) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }
}

void shutdown_conn(int sock_fd) {
    int status = shutdown(sock_fd, SHUT_RDWR);

    if (status == -1) {
        perror("shutdown() error");
    }
}

int handle_incoming_conn(int master_fd) {
    int client_fd = accept(master_fd, NULL, NULL);

    if (client_fd == -1) {
        perror("Accept() failed");
        exit(EXIT_FAILURE);
    }

    return client_fd;
}

// simple char compare function for qsort
int char_comp_gr(const void *a, const void *b) {
    const char *ca = a;
    const char *cb = b;

    if (*ca > *cb) {
        return -1;
    }
    else if (*ca < *cb) {
        return 1;
    }
    else {
        return 0;
    }
}

void read_input(int master_fd) {
    fd_set readfd;
    int max_fd = master_fd;
    char read_buf[BUFSIZ];
    // init all necessary structures  
    bzero(read_buf, BUFSIZ);
    FD_ZERO(&readfd);
    FD_SET(master_fd, &readfd);
    listen(master_fd, SOMAXCONN);

    while (true) {
        fd_set temp_read = readfd;
        int status = select(max_fd + 1, &temp_read, NULL, NULL, NULL);

        if (status == -1) {
            perror("Select error");
        }
        
        int old_max_fd = max_fd;
        for (size_t i = 0; i < old_max_fd + 1; ++i) {
            if (!FD_ISSET(i, &temp_read)) {
                continue;
            }

            if (i == master_fd) {
                int client_fd = handle_incoming_conn(master_fd);                

                max_fd = (max_fd < client_fd) ? client_fd : max_fd;
                FD_SET(client_fd, &readfd);
            }
            else {
                int bytes_read = recv(i, read_buf, BUFSIZ, MSG_DONTWAIT);

                if (bytes_read <= 0) {
                    int err_code = errno;

                    if (err_code != EAGAIN) {
                        shutdown_conn(i); 
                        close(i);
                        FD_CLR(i, &readfd);
                        max_fd = (max_fd == i) ? i - 1 : max_fd;
                    }
                }
                else {
                    read_buf[bytes_read] = '\0';
                    qsort(read_buf, strlen(read_buf), sizeof(char), char_comp_gr);
                    send(i, read_buf, strlen(read_buf), MSG_NOSIGNAL);
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    assert(argc == 2);
    int master_fd = master_socket();
    
    bind_to_port(master_fd, strtol(argv[1], NULL, 10));
    read_input(master_fd);

    return 0;
}
