#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
    assert(argc == 2);
    
    struct addrinfo *res = NULL;
    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM
    };
    int status = getaddrinfo(argv[1], NULL,
                             &hints, &res);

    if (status == -1) {
        perror("getaddrinfo() error");
        exit(EXIT_FAILURE);
    }
    
    for (struct addrinfo *it = res; it != NULL; it = it->ai_next) {
        switch (it->ai_family) {
            case AF_INET: {
                // IPv4 address
                char ipv4_addr[INET_ADDRSTRLEN];
                struct sockaddr_in *pipv4 = (struct sockaddr_in *) it->ai_addr;

                inet_ntop(AF_INET, &pipv4->sin_addr, ipv4_addr, sizeof(ipv4_addr));
                printf("%s\n", ipv4_addr);
                break;
            }
            case AF_INET6: {
                // IPv6 address
                char ipv6_addr[INET6_ADDRSTRLEN];
                struct sockaddr_in6 *pipv6 = (struct sockaddr_in6 *) it->ai_addr;

                inet_ntop(AF_INET6, &pipv6->sin6_addr, ipv6_addr, sizeof(ipv6_addr));
                printf("%s\n", ipv6_addr);

                break;
            }
            default:
                break;
        }
    }

    return 0;
}
