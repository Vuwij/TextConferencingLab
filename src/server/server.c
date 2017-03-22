#include <sys/select.h>

#include "server.h"
#include "audio_port.h"
#include "../audio_packet.h"

int port_c;
int sockfd_c;
fd_set master;
bool control_fd[MAX_FD_NUM] = {0}; // Identifier for control fd

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

void usage(char *program) {
    fprintf(stderr, "Usage: %s <TCP port number to listen on>\n", program);
    exit(1);
}

void sigchld_handler(int s) {
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

void open_server_socket(int port) {
    char server_port_s[PORT_BUF_SIZE];
    snprintf(server_port_s, PORT_BUF_SIZE, "%d", port);
    int new_fd; // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, server_port_s, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
    }

    for (audio_port = servinfo; audio_port != NULL; audio_port = audio_port->ai_next) {
        if ((sockfd_c = socket(audio_port->ai_family, audio_port->ai_socktype,
                audio_port->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd_c, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof (int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd_c, audio_port->ai_addr, audio_port->ai_addrlen) == -1) {
            close(sockfd_c);
            perror("server: bind");
            continue;
        }
        break;
    }

    freeaddrinfo(servinfo);

    if (audio_port == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
    if (listen(sockfd_c, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    // Clear all dead processes
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

}

struct timeval tv = {
    .tv_sec = 1,
    .tv_usec = SERVER_NACK_TIMEOUT,
};

void listen_for_messages() {
    PRINT("Waiting for Clients\n");

    //Initialize File Descriptor
    fd_set read_fds;

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    FD_SET(sockfd_c, &master); // add the Control listener to the master set

    int fdmax = sockfd_c > sockfd_d ? sockfd_c : sockfd_d;

    struct sockaddr_storage remoteaddr; // client address
    char remoteIP[INET6_ADDRSTRLEN];
    int nbytes;

    while (1) {
        // Copy the master to the temporary FD
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, &tv) == -1) {
            perror("select - server");
            exit(4);
        }

        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {

                if (i == sockfd_c) { // Received a data from the control port
                    socklen_t addrlen = sizeof remoteaddr;
                    int newfd = accept(sockfd_c,
                            (struct sockaddr *) &remoteaddr, &addrlen);
                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        control_fd[newfd] = true;
                        if (newfd > fdmax) fdmax = newfd;

                        PRINT("New Con. %s on socket %d\n",
                                inet_ntop(remoteaddr.ss_family,
                                get_in_addr((struct sockaddr*) &remoteaddr),
                                remoteIP, INET6_ADDRSTRLEN
                                ),
                                newfd
                                );
                    }
                }
                else if (i == sockfd_d) { // Received a data from the data port
                    // Add UDP socket into the fd list
                    AudioPacket* audiopacket = (AudioPacket*) malloc(sizeof(AudioPacket));
                    socklen_t addrlen = sizeof remoteaddr;
                    
                    if ((nbytes = recvfrom(sockfd_d, audiopacket, sizeof (AudioPacket), 0,
                            (struct sockaddr *) &remoteaddr, &addrlen)) == -1) {
                        perror("recvfrom udp");
                        exit(1);
                    }
                    
                    process_audio_packets(audiopacket, remoteaddr);
                }
                else { // Received control port info
                    Message* msg = (Message*) malloc(sizeof (Message));
                    if ((nbytes = recv(i, msg, sizeof (Message), 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) PRINT("selectserver: socket %d hung up\n", i);
                        else perror("recv");

                        exitserver(msg, i);
                    } else {
                        if (DEBUG_MSG) print_message(msg);
                        handle_client_message(msg, i);
                    }
                    free(msg);
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) usage(argv[0]);

    // Obtain the Port Number
    port_c = atoi(argv[1]);
    if (!(MIN_PORTNUM <= port_c && port_c <= MAX_PORTNUM)) {
        fprintf(stderr, "port = %d should be within range [%d:%d]\n", port_c, MIN_PORTNUM, MAX_PORTNUM);
        usage(argv[0]);
    }

    // Open a server side socket
    PRINT("Opened Ctl Port: %d\n", port_c);
    open_server_socket(port_c);

    // Open the audio listener socket
    PRINT("Opened Data Port: %d\n", port_c + 1);
    open_audio_socket(port_c + 1);

    // The main iteration loop
    listen_for_messages();

    close(sockfd_c);
    return 0;
}
