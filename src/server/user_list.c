#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "user_list.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

// Global list of online users
User_List online_users;

// Global list of registered users
User registered_users[] = { 
    {1, "User 1"}, 
    {2, "User 2"},
    {3, "User 3"},
    {4, "User 4"}
};

bool authenticate_existing_user(int id, char* password) {
    for(int i = 0; i < NELEMS(registered_users); i++) {
        if(registered_users[i].id == id) {
            if(registered_users[i].password == password) {
                // Authentication Succeeded, returned L_ACK
                return true;
            }
            else {
                // Password failed, return N_ACK
                return false;
            }
        }
    }
    return false;
}

User_List* find_active_user(int id) {
    User_List* next = &online_users;
    while(next != NULL) {
        if(next->user.id == id) return next;
        next = next->next;
    }
    return NULL;
}

User_List* add_user(User* user) {
    User_List* next = &online_users;
    while(next->next != NULL)
        next = next->next;
    
    next->next->user = *user;
    return next->next;
}

bool delete_user(int id) {
    User_List* previous = &online_users;
    User_List* next = &online_users;
    while(next != NULL) {
        if(next->user.id == id) break;
        next = next->next;
        previous = next;
    }
    
    if(next == NULL) return false;
    
    previous->next = next->next;
    next->next = NULL;
    free(next);
    
    return true;
}

void print_active_users() {
    printf("Active Users:\n");
    printf("User\tIP Address\tPort\tSession");
    
    User_List* next = &online_users;
    printf("%04d\t%s\t%4d\t%4d", next->user.id, next->ip_address, next->port_address, next->session_id);
    while(next != NULL) {
        next = next->next;
        printf("%04d\t%s\t%4d\t%4d", next->user.id, next->ip_address, next->port_address, next->session_id);
    }
}