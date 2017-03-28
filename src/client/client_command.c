#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "client.h"
#include "status.h"
#include "audio_input.h"

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

bool login(char* client_id, char* password, char* server_ip, int server_port) {
    PRINT("Login: %s %s at %s %d\n", client_id, password, server_ip, server_port);

    if (!(MIN_PORTNUM <= server_port && server_port <= MAX_PORTNUM)) {
        fprintf(stderr, "port = %d should be within range [%d:%d]\n", server_port, MIN_PORTNUM, MAX_PORTNUM);
        return false;
    }

    int sockfd;
    struct addrinfo hints, *p, *servinfo;
    int rv;

    char server_port_s[PORT_BUF_SIZE];
    snprintf(server_port_s, PORT_BUF_SIZE, "%d", server_port);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(server_ip, server_port_s, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return false;
    }
    // loop through all the results and make a socket
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to create socket\n");
        return false;
    }

    char s[INET6_ADDRSTRLEN];
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr), s, sizeof s);
    freeaddrinfo(servinfo);

    // Send a message and get an reply
    PRINT("Connecting to %s\n", s);
    Message m;
    m.type = LOGIN;
    bzero(m.source, MAX_NAME);
    bzero(m.data, MAX_DATA);
    strncpy(m.source, client_id, MAX_NAME);
    strncpy(m.data, password, MAX_DATA);

    PRINT("Logging In\n");
    deliver_message(&m, sockfd);

    Message* r;
    r = receive_message(sockfd);

    // Update Status with the client and server socket and port
    status.sockfd = sockfd;
    status.p = p;
    strncpy(status.connected_server_ip, server_ip, MAXBUFSIZE);
    status.connected_server_port = server_port;

    if (r->type == LO_NAK) {
        PRINT("Login Failed: %s\n", r->data);
        status.sockfd = -1;
    }

    if (r->type == LO_ACK) {
        PRINT("Login Succeeded\n");
        strncpy(status.client_id, client_id, MAXBUFSIZE);
        strncpy(status.password, password, MAXBUFSIZE);
        free(r);
        return true;
    }
    free(r);
    return false;
}

bool logout() {
    PRINT("Logging out\n");

    Message m;
    m.type = EXIT;
    strncpy(m.source, status.client_id, MAX_NAME);
    deliver_message(&m, status.sockfd);
    close(status.sockfd);
    return true;
}

bool join_session(char *session_name) {
    PRINT("Joining session %s\n", session_name);

    Message m;
    m.type = JOIN;
    snprintf(m.source, MAX_NAME, "%s", status.client_id);
    snprintf(m.data, MAX_DATA, "%s", session_name);
    deliver_message(&m, status.sockfd);

    Message* r;
    r = receive_message(status.sockfd);
    if (r->type == JN_NAK) {
        PRINT(r->data);
        free(r);
        return false;
    } else if (r->type == JN_ACK) {
        PRINT("Joined Session %s\n", r->data);
        free(r);
        return true;
    }

    PRINT("Received wrong packet from server\n");
    free(r);
    return false;
}

bool leave_session() {
    PRINT("Leaving session\n");

    Message m;
    m.type = LEAVE_SESS;
    snprintf(m.source, MAX_NAME, "%s", status.client_id);
    deliver_message(&m, status.sockfd);

    return true;
}

bool create_session(char *session_id) {
    PRINT("Creating session %s\n", session_id);

    Message m;
    m.type = NEW_SESS;
    snprintf(m.source, MAX_NAME, "%s", status.client_id);
    snprintf(m.data, MAX_DATA, "%s", session_id);
    deliver_message(&m, status.sockfd);

    Message* r;
    r = receive_message(status.sockfd);
    if (r->type == NS_ACK) {
        PRINT("Session Created\n");
        join_session(session_id);
        free(r);
        return true;
    }
    PRINT(r->data);
    free(r);
    return false;
}

bool list() {
    PRINT("Listing Sessions\n");

    Message m;
    m.type = QUERY;
    snprintf(m.source, MAX_NAME, "%s", status.client_id);
    deliver_message(&m, status.sockfd);

    Message* r;
    r = receive_message(status.sockfd);

    if (r->type == QU_ACK)
        PRINT(r->data);
    else
        PRINT("Invalid packet %d\n", (int) r->type);

    free(r);
    return false;
}

bool quit() {
    PRINT("Quitting\n");

    close(status.sockfd);
    exit(1);
}

bool send_message(char* message) {
    PRINT("Sending Message\n");

    Message m;
    m.type = MESSAGE;
    snprintf(m.source, MAX_NAME, "%s", status.client_id);
    strncpy(m.data, message, MAX_DATA);
    deliver_message(&m, status.sockfd);

    return true;
}

bool start_call() {
    PRINT("Starting Call\n");

    Message m;
    m.type = ST_CONF;
    snprintf(m.source, MAX_NAME, "%s", status.client_id);
    deliver_message(&m, status.sockfd);

    Message* r;
    r = receive_message(status.sockfd);

    if (r->type == ST_CONF_NCK) {
        PRINT(r->data);
        free(r);
        return false;
    }

    PRINT("Call Started, Joining call\n");
    free(r);

    return true;
}

bool join_call() {
    PRINT("Joining call\n");
    setup_capture();
    setup_playback();

    // Open the UDP socket to the server
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    char* address = status.connected_server_ip;
    char port[MAXDATASIZE];
    PRINT("Open UDP Port to %d\n", status.connected_server_port + 1);
    sprintf(port, "%d", status.connected_server_port + 1);

    if ((rv = getaddrinfo(address, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to create socket\n");
        return 2;
    }

    // Add this information to status
    status.udp = p;
    status.voicefd = sockfd;

    // Send a reply to the server
    Message m;
    m.type = ST_CONF_INIT_ACK;
    snprintf(m.source, MAX_NAME, "%s", status.client_id);
    deliver_message(&m, status.sockfd);

    Message* r;
    r = receive_message(status.sockfd);
    
    // Start the capture and send thread
    open_capture();
    
    // Start receiving everything
    FD_SET(status.voicefd, &master);
    fdmax = fdmax > status.voicefd ? fdmax : status.voicefd;
    
    free(r);
    return true;
}