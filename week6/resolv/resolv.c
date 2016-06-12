#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    assert(argc == 2);

    const struct hostent *hent = gethostbyname(argv[1]);

    if (!hent) {
        perror("gethostbyname() error");
        exit(EXIT_FAILURE);
    }
    
    for (const char **it = hent->h_addr_list; *it != NULL; ++it) {
        printf("%s\n", inet_ntoa(*((struct in_addr *) *it)));
    }

    return 0;
}
