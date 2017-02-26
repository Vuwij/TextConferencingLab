#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>

#include "server.h"

int port;

void usage(char *program) {
    fprintf(stderr, "Usage: %s <TCP port number to listen on>\n", program);
    exit(1);
}

int open_server_socket(struct addrinfo **p, char* argv[]) {
    struct addrinfo hints, *servinfo;
    int sockfd;
    int rv;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET for IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // Using my IP

    if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // looping through all results and bind the first one available
    for ((*p) = servinfo; (*p) != NULL; (*p) = (*p)->ai_next) {
        if ((sockfd = socket((*p)->ai_family, (*p)->ai_socktype,
                (*p)->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, (*p)->ai_addr, (*p)->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "server: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);
    
    return sockfd;
}

int main(int argc, char *argv[]) {
    if (argc != 2) usage(argv[0]);

    // Obtain the Port Number
    port = atoi(argv[1]);
    if (!(0 <= port && port <= 65535)) {
        fprintf(stderr, "port = %d should be within 0-65535\n", port);
        usage(argv[0]);
    }
    
    // Open a server side socket
    int sockfd;
    struct addrinfo *p;
    sockfd = open_server_socket(&p, argv);
    
    close(sockfd);
    return 0;
}
