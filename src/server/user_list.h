#ifndef USER_LIST
#define USER_LIST

#include <stdbool.h>
#include <sys/socket.h>

#include "user.h"
#include "../constants.h"
#include "../ssl_common.h"

typedef enum login_error {
    ERR_NO = 0,
    ERR_LOGGED_IN = 1,
    ERR_ID_NO_MATCH = 2,
    ERR_PASS_NO_MATCH = 3
} Login_Error;

typedef struct user_list {
    User* user;
    char session_id[MAXDATASIZE];
    int fd;
    SSL *ssl;
    struct sockaddr_storage udp_addr;

    struct user_list *next;
} User_List;

Login_Error authenticate_existing_user(const char *id, const char* password, User** user_ret);

User_List* find_active_user(const char *id);

User_List* find_active_user_fd(int sock_fd);

void add_user(User* user, int fd, SSL *ssl);

bool delete_user(char *id);

void print_active_users();

#endif /* ACTIVE_CLIENTS_H */

