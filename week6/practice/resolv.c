#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

int main() {
    char hostname[256];

    fgets(hostname, 256, stdin);
    hostname[strlen(hostname) - 1] = '\0';
    struct hostent *hn = gethostbyname(hostname);

    if (!hn) {
        perror("Cannot resolve the host");
        exit(EXIT_FAILURE);
    }

    printf("host name: %s\n", hn->h_name);
    for (size_t i = 0; hn->h_aliases[i] != NULL; ++i) {
        printf("alias: %s\n", hn->h_aliases[i]);
    }
    printf("Type: ");
    if (hn->h_addrtype == AF_INET) {
        printf("IPv4\n");
    }
    else {
        printf("IPv6\n");
    }

    printf("%d\n", hn->h_length);
    for (size_t i = 0; hn->h_addr_list[i] != NULL; ++i) {
        char ipv4_addr[INET_ADDRSTRLEN];

        inet_ntop(hn->h_addrtype, hn->h_addr_list[i], ipv4_addr, INET_ADDRSTRLEN);
        printf("ip addr: %s\n", ipv4_addr);
    }

    return 0;
}
