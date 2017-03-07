#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "user_list.h"
#include "../constants.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

// Global list of online users
User_List online_users;

// Global list of registered users
User registered_users[] = { 
    {1, "User1"}, 
    {2, "User2"},
    {3, "User3"},
    {4, "User4"}
};

bool authenticate_existing_user(int id, char* password) {
    for(int i = 0; i < NELEMS(registered_users); i++) {
        if(registered_users[i].id == id) {
            if(strcmp(registered_users[i].password, password) == 0) {
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
    
    next->user = *user;
    next->next = (User_List*) malloc(sizeof(User_List));
    return next;
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