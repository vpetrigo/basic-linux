#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <netdb.h>

#define DGRAM_SIZE 5120 

int set_non_blocking(int fd) {
    int flags;
    /* If they have O_NONBLOCK, use the Posix way to do it */
#if defined(O_NONBLOCK)
    /* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
        flags = 0;
    }

    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    /* Otherwise, use the old way of doing it */
    flags = 1;
    return ioctl(fd, FIONBIO, &flags);
#endif
}

int get_socket(void) {
    // create non-blocking UDP socket
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (server_fd == -1) {
        perror("Cannot open UDP socket");
        exit(EXIT_FAILURE);       
    }
    
    set_non_blocking(server_fd);

    return server_fd;
}

void bind_to_port(int server_fd, const char *port) {
    struct addrinfo *res = NULL;
    // get sockaddr structure for INADDR_ANY and a specific port
    int status = getaddrinfo("0.0.0.0", port, NULL, &res);

    if (status == -1) {
        perror("Cannot get address info");
        exit(EXIT_FAILURE);
    }
    
    // binding
    status = bind(server_fd, res->ai_addr, res->ai_addrlen);

    if (status == -1) {
        perror("Cannot bind");
        exit(EXIT_FAILURE);
    }
}

void wait_for_data(int server_fd) {
    fd_set readfd;
    char readbuf[DGRAM_SIZE];
    const char *TRANSF_END = "OFF\n";
    int max_fd = server_fd;
    
    bzero(readbuf, DGRAM_SIZE);
    FD_ZERO(&readfd);
    FD_SET(server_fd, &readfd);
    listen(server_fd, SOMAXCONN);

    while (true) {
        fd_set temp_read = readfd;
        int status = select(max_fd + 1, &temp_read, NULL, NULL, NULL);

        if (status == -1) {
            perror("select error");
            exit(EXIT_FAILURE);
        }
        
        for (size_t i = 0; i < max_fd + 1; ++i) {
            if (i == server_fd) {
                int bytes_read = recvfrom(server_fd, readbuf, DGRAM_SIZE, MSG_DONTWAIT, NULL, NULL);

                if (bytes_read == -1) {
                    perror("Read from socket error");
                }

                readbuf[bytes_read] = '\0'; 
                if (strcmp(readbuf, TRANSF_END) == 0) {
                    return;
                }
                printf("%s\n", readbuf);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int server_fd = get_socket();
    
    bind_to_port(server_fd, argv[1]);
    wait_for_data(server_fd);

    return 0;
}
