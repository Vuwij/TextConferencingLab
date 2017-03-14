#ifndef SERVER_H
#define SERVER_H

#include <netdb.h>

#include "../constants.h"
#include "../message.h"
#include "../packet_type.h"
#include "user_list.h"
#include "session_list.h"

#include "user.h"

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


void handle_client_message(Message* msg, int fd);

#endif /* SERVER_H */