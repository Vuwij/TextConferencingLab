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
#include <openssl/ssl.h>

#include "../ssl_common.h"
#include "server.h"
#include "audio_port.h"
#include "../audio_packet.h"

SSL_CTX *ctx;
int port;
fd_set master; // master file descriptor list
int sockfd;
AddrInfo *p;

struct timeval tv = {
    .tv_sec = 1,
    .tv_usec = SERVER_NACK_TIMEOUT,
};

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

/* Check that the common name and email matches the host name and email */
void verify_client_cert(ssl, host, email)
SSL *ssl;
char *host;
char *email;
{
    X509 *peer;
    char peer_CN[256];
    char peer_EM[256];

    if (SSL_get_verify_result(ssl) != X509_V_OK) {
        berr_exit(FMT_NO_VERIFY);
    }

    /* Check the common name */
    peer = SSL_get_peer_certificate(ssl);
    X509_NAME_get_text_by_NID(X509_get_subject_name(peer),
            NID_commonName, peer_CN, 256);

    if (strcasecmp(peer_CN, host))
        err_exit(FMT_CN_MISMATCH);

    X509_NAME_get_text_by_NID(X509_get_subject_name(peer),
            NID_pkcs9_emailAddress, peer_EM, 256);

    if (strcasecmp(peer_EM, email))
        err_exit(FMT_EMAIL_MISMATCH);

}

void open_server_socket(int port) {
    char server_port_s[PORT_BUF_SIZE];
    snprintf(server_port_s, PORT_BUF_SIZE, "%d", port);
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

void listen_for_messages() {
    PRINT("Waiting for Clients\n");

    //Initialize File Descriptor
    fd_set read_fds;

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    FD_SET(sockfd_c, &master); // add the Control listener to the master set
    FD_SET(sockfd_d, &master); // add the Data listener to the master set
    
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
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                SSL *ssl;
                // New Connections
                if (i == sockfd) {
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

                        BIO *sbio = BIO_new_socket(newfd, BIO_NOCLOSE);
                        ssl = SSL_new(ctx);
                        SSL_set_bio(ssl, sbio, sbio);
                        if ((SSL_accept(ssl) <= 0))
                            berr_exit("SSL accept error");
                        verify_client_cert(ssl, EXPECTED_HOST_NAME, EXPECTED_CLIENT_EMAIL);

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
                    if ((nbytes = SSL_read(ssl, msg, sizeof (Message))) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) PRINT("selectserver: socket %d hung up\n", i);
                        else perror("recv");

                        exitserver(msg, i);
                    } else {
                        if (DEBUG_MSG) print_message(msg);
                        handle_client_message(msg, i, ssl);
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
    port = atoi(argv[1]);
    if (!(MIN_PORTNUM <= port && port <= MAX_PORTNUM)) {
        fprintf(stderr, "port = %d should be within range [%d:%d]\n",
                port, MIN_PORTNUM, MAX_PORTNUM);
        usage(argv[0]);
    }
    
    // Open the audio listener socket
    open_audio_socket(port_c + 1);
    PRINT("Opened Data Port: %d\n", port_c + 1);
    
    // Open a server side socket
    open_server_socket(port);
    ctx = initialize_ctx(KEY_FILE_PATH, PASSWORD);

    // Configure SSL server to ask for client certificate
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE
            | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);


    // The main iteration loop
    listen_for_messages();

    close(sockfd_c);
    return 0;
}

static void clean_up(int s, int sock, SSL * ssl) {
    int r = SSL_shutdown(ssl);
    if (!r) {
        shutdown(s, SHUT_WR);
        r = SSL_shutdown(ssl);
    }

    if (r != 1) {
        err_exit(FMT_INCOMPLETE_CLOSE);
    }

    SSL_free(ssl);
    close(sock);
    close(s);
}
