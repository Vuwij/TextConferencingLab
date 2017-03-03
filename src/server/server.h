#ifndef SERVER_H
#define	SERVER_H

#include <netdb.h>

#include "../constants.h"
#include "../message.h"
#include "../packet_type.h"
#include "user_list.h"

#include "user.h"

typedef struct addrinfo AddrInfo;

extern int sockfd;
extern int PORT;
extern AddrInfo *p;

// The list of online ussers
extern User_List online_users;

// The lust of registered users
extern User registered_users[];

void handle_client_message(char* message, int fd);

#endif	/* SERVER_H */