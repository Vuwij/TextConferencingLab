#ifndef SERVER_H
#define SERVER_H

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

#define MAX_FD_NUM 1024

#include "user_list.h"
#include "session_list.h"
#include "user.h"
#include "../constants.h"
#include "../message.h"
#include "../packet_type.h"

// Master file descriptor list
extern fd_set master;
extern bool control_fd[MAX_FD_NUM];

// Control Port information
extern int port_c;  // TCP Control Port
extern int sockfd_c;

extern int port_d;  // UDP Data Port
extern int sockfd_d;
extern AddrInfo *audio_port;

// Registered User List
extern User registered_users[];

// Online User List
extern User_List* online_users;

// Session List
extern Session_List* session_list;

void handle_client_message(Message* msg, int fd);

void exitserver(Message* msg, int fd);

#endif /* SERVER_H */