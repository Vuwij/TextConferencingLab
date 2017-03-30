#ifndef SERVER_H
#define SERVER_H

#include <netdb.h>

#include "../constants.h"
#include "../message.h"
#include "../packet_type.h"
#include "user_list.h"
#include "session_list.h"

#include "../ssl_common.h"
#include "user.h"

/* use these strings to tell the marker what is happening */
#define FMT_ACCEPT_ERR "SERVER: SSL accept error\n"
#define FMT_CLIENT_INFO "SERVER: %s %s\n"
#define FMT_OUTPUT "SERVER: %s %s\n"
#define FMT_INCOMPLETE_CLOSE "SERVER: Incomplete shutdown\n"

#define PASSWORD "password"
#define KEY_FILE_PATH "bob.pem"
#define EXPECTED_HOST_NAME "Alice's client"
#define EXPECTED_CLIENT_EMAIL "ece568alice@ecf.utoronto.ca"

typedef struct addrinfo AddrInfo;

extern int sockfd;
extern int PORT;
extern AddrInfo *p;

// Registered User List
extern User registered_users[];

// Online User List
extern User_List* online_users;

// Session List
extern Session_List* session_list;
extern fd_set master; // master file descriptor list



void handle_client_message(Message* msg, int fd, SSL *ssl);

void exitserver(Message* msg, int fd);

#endif /* SERVER_H */