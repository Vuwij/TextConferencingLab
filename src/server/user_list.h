#ifndef USER_LIST
#define	USER_LIST

#include <stdbool.h>

#include "user.h"

typedef struct user_list {
    User user;
    int session_id;
    int fd;
    struct user_list *next;
} User_List;

bool authenticate_existing_user(int id, char* password);

User_List* find_active_user(int id);

User_List* add_user(User* user, int fd);

bool delete_user(int id);

void print_active_users();

#endif	/* ACTIVE_CLIENTS_H */
