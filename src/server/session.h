#ifndef SESSION_H
#define	SESSION_H

#include <sys/time.h>

#include "user.h"
#include "../constants.h"

typedef struct session {
    int id;
    fd_set ports;
    int fd_max;
    User* users[MAX_USERS_PER_SESSION];
}Session;

#endif	/* SESSION_H */