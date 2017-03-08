#ifndef SERVER_H
#define	SERVER_H

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

// The list of registered users
extern User registered_users[];

// The list of online ussers
extern User_List* online_users;

// The session list
extern Session_List* session_list;

void handle_client_message(Message* msg, int fd);

#endif	/* SERVER_H */