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

#include "audio_port.h"
#include "../constants.h"
#include "server.h"

struct addrinfo *audio_port;
int port_d;
int sockfd_d;

void open_audio_socket(int port) {
    struct addrinfo hints, *servinfo;
    
    int rv;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET for IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // Using my IP
    
    char port_s[MAXDATASIZE];
    sprintf(port_s, "%d", port);
    
    if ((rv = getaddrinfo(NULL, port_s, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
    }

    // looping through all results and bind the first one available
    for (audio_port = servinfo; audio_port != NULL; audio_port = audio_port->ai_next) {
        if ((sockfd_d = socket(audio_port->ai_family, audio_port->ai_socktype,
                audio_port->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd_d, audio_port->ai_addr, audio_port->ai_addrlen) == -1) {
            close(sockfd_d);
            perror("listener: bind");
            continue;
        }

        break;
    }
    
    if (audio_port == NULL) {
        fprintf(stderr, "server: failed to bind socket\n");
        return;
    }
    
    port_d = port;
    freeaddrinfo(servinfo);
    return;
}