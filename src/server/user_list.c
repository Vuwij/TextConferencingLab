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
    {"User1", "User1"},
    {"User2", "User2"},
    {"User3", "User3"},
    {"User4", "User4"}
};

Login_Error authenticate_existing_user(const char *id, const char* password, User** user_ret) {
    for (int i = 0; i < NELEMS(registered_users); i++) {
        if (strcmp(registered_users[i].id, id) == 0) {
            if (strncmp(registered_users[i].password, password, sizeof (password)) == 0) {
                *user_ret = &registered_users[i];
                // Check if there is anyone logged in with the same ID
                if (find_active_user(id) == NULL)
                    return ERR_NO;
                else
                    return ERR_LOGGED_IN;
            } else {
                // Password failed
                return ERR_PASS_NO_MATCH;
            }
        }
    }
    return ERR_ID_NO_MATCH;
}

User_List* find_active_user(const char *id) {
    User_List* next = online_users;
    while (next != NULL) {
        if (strcmp(next->user->id, id) == 0)
            return next;
        next = next->next;
    }
    return NULL;
}

User_List* find_active_user_fd(int sock_fd) {
    User_List* next = online_users;
    while (next != NULL) {
        if (next->fd == sock_fd)
            return next;
        next = next->next;
    }
    return NULL;
}

void add_user(User* user, int fd, SSL *ssl) {

    User_List* curr = online_users;

    User_List* new_user = (User_List*) malloc(sizeof (User_List));
    new_user->user = user;
    new_user->next = NULL;
    strcpy(new_user->session_id, "");
    new_user->fd = fd;
    new_user->ssl = ssl;

    if (online_users == NULL) {
        online_users = new_user;
        return;
    }

    while (curr->next != NULL)
        curr = curr->next;
    curr->next = new_user;
}

bool delete_user(char *id) {
    User_List* prev = NULL;
    User_List* curr = online_users;
    while (curr != NULL) {
        if (strcmp(curr->user->id, id) == 0)
            break;
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) {
        PRINT("Deleting: couldn't find User\n");
        return false;
    }

    if (prev == NULL)
        online_users = NULL;
    else
        prev->next = curr->next;
    curr->next = NULL;
    free(curr);

    return true;
}

void print_active_users() {
    PRINT("Active Users:\n");
    PRINT("User Session\n");

    User_List* curr = online_users;
    while (curr != NULL) {
        PRINT("%05s %s\n", curr->user->id, curr->session_id);
        curr = curr->next;
    }
}