#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>

#include "server.h"

int port;

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void usage(char *program) {
    fprintf(stderr, "Usage: %s <TCP port number to listen on>\n", program);
    exit(1);
}

int open_server_socket(AddrInfo **p, char* argv[]) {
    AddrInfo hints, *servinfo;
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

struct timeval tv = {
    .tv_sec = 1,
    .tv_usec = SERVER_NACK_TIMEOUT,
};

void listen_for_messages(int listener, AddrInfo *p) {
    fd_set master;                      // master file descriptor list
    fd_set read_fds;                    // temp file descriptor list for select()
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    FD_SET(listener, &master);          // add the listener to the master set
    
    int fdmax = listener;               // keep track of the biggest file descriptor
    
    struct sockaddr_storage remoteaddr; // client address
    char remoteIP[INET6_ADDRSTRLEN];
    
    char buf[MAXBUFSIZE];                      // buffer for client data
    int nbytes;
    
    // main loop
    PRINT("Waiting for Clients\n");
    for(;;) {
        // Copy the master to the temporary FD
        read_fds = master;
        if (select(fdmax+1, &read_fds, NULL, NULL, &tv) == -1) {
            perror("select");
            exit(4);
        }
        
        // run through the existing connections looking for data to read
        for(int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                // New Connections
                if (i == listener) { 
                    socklen_t addrlen = sizeof remoteaddr;
                    int newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);
                    
                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) fdmax = newfd;
                        
                        printf("selectserver: new connection from %s on socket %d\n",
                            inet_ntop(remoteaddr.ss_family,
                                get_in_addr((struct sockaddr*)&remoteaddr),
                                remoteIP, INET6_ADDRSTRLEN
                                ),
                            newfd
                        );
                    }
                } else {
                    // TODO: move to a new function
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) printf("selectserver: socket %d hung up\n", i);
                        else perror("recv");
                        
                        close(i); // bye!
                        FD_CLR(i, &master);// remove from master set
                    } 
                    // Actually get the message
                    else {
                        handle_client_message(buf, i);
                        // we got some data from a client
                        
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)   
}

int main(int argc, char *argv[]) {
    if (argc != 2) usage(argv[0]);
    PRINT("Started\n");
    
    // Obtain the Port Number
    port = atoi(argv[1]);
    if (!(0 <= port && port <= 65535)) {
        fprintf(stderr, "port = %d should be within 0-65535\n", port);
        usage(argv[0]);
    }
    
    // Open a server side socket
    int sockfd;
    AddrInfo *p;
    sockfd = open_server_socket(&p, argv);
    
    // The main iteration loop
    listen_for_messages(sockfd, p);
    
    close(sockfd);
    return 0;
}
