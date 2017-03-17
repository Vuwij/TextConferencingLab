#ifndef SESSION_H
#define SESSION_H

#include <sys/time.h>

#include "user.h"
#include "../constants.h"

#define NOT_AVAIL -1

typedef struct session {
    char id[MAXDATASIZE];
    fd_set client_fds;
    int fd_max;
    User* users[MAX_USERS_PER_SESSION];
    int num_user;
    
    bool call_started;
    fd_set audio_fds;
    int audio_fd_max;
} Session;

#endif /* SESSION_H */