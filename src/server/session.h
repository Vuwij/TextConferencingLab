#ifndef SESSION_H
#define SESSION_H

#include <sys/time.h>

#include "user.h"
#include "../constants.h"

#define NOT_AVAIL -1

typedef struct session {
    char id[MAXDATASIZE];
    fd_set client_ports;
    int fd_max;
    User* users[MAX_USERS_PER_SESSION];
    int num_user;
} Session;

#endif /* SESSION_H */