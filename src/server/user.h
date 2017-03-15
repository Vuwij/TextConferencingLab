#ifndef USER_H
#define	USER_H

#include "../constants.h"

typedef struct user {
    char id[MAXDATASIZE];
    char password[MAXDATASIZE];
} User;

#endif