#ifndef USER_LIST
#define USER_LIST

#include <stdbool.h>

#include "user.h"

#define ERR_NO 0
#define ERR_LOGGED_IN 1
#define ERR_ID_NO_MATCH 2
#define ERR_PASS_NO_MATCH 3

typedef struct user_list {
    User user;
    int session_id;
    int fd;
    struct user_list *next;
} User_List;

int authenticate_existing_user(int id, char* password);

User_List* find_active_user(int id);

void add_user(User* user, int fd);

bool delete_user(int id);

void print_active_users();

#endif /* ACTIVE_CLIENTS_H */

