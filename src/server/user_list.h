#ifndef USER_LIST
#define	USER_LIST

#include "user.h"

typedef struct user_list {
    User user;
    int session_id;
    char* ip_address;
    int port_address;
    struct user_list *next;
} User_List;

extern User_List online_users;
extern User registered_users[];

bool authenticate_existing_user(int id, char* password);

User_List* find_active_user(int id);

User_List* add_user(User* user);

bool delete_user(int id);

void print_active_users();

#endif	/* ACTIVE_CLIENTS_H */

