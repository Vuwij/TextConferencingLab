#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "user_list.h"
#include "../constants.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

// Global list of online users
User_List* online_users;

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
    User_List* next = online_users;
    while(next != NULL) {
        if(next->user.id == id) return next;
        next = next->next;
    }
    return NULL;
}

User_List* add_user(User* user, int fd) {
    User_List* next = online_users;
    
    if(online_users == NULL) {
        online_users = (User_List*) malloc(sizeof(User_List));
        online_users->user = *user;
        online_users->fd = fd;
        return online_users;
    }
    
    while(next->next != NULL)
        next = next->next;
    
    // Add information
    next->next = (User_List*) malloc(sizeof(User_List));
    next->next->user = *user;
    next->next->fd = fd;

    return next->next;
}

bool delete_user(int id) {
    User_List* previous = online_users;
    User_List* next = online_users;
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
    PRINT("Active Users:\n");
    PRINT("User FD Session\n");
    
    User_List* next = online_users;
    while(next != NULL) {
        PRINT("%d\t%d\t%d\n", next->user.id, next->fd, next->session_id);
        next = next->next;
    }
}