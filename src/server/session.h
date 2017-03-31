#ifndef SESSION_H
#define SESSION_H

#include <sys/time.h>

#include "user.h"
#include "../constants.h"
#include "voice_queue.h"

#define NOT_AVAIL -1

typedef struct session {
    char id[MAXDATASIZE];
    fd_set client_fds;
    int fd_max;
    User* users[MAX_USERS_PER_SESSION];
    int num_user;

    // Audio section
    bool call_started;
    VoicePacketQueue* vpq [MAX_USERS_PER_SESSION];
} Session;

#endif /* SESSION_H */