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

int authenticate_existing_user(int id, char* password) {
    for (int i = 0; i < NELEMS(registered_users); i++) {
        if (registered_users[i].id == id) {
            if (strncmp(registered_users[i].password, password, sizeof (password)) == 0) {
                // Check if there is anyone logged in with the same ID
                if (find_active_user(id) == NULL)
                    // Not found duplicate user No ERROR
                    return ERR_NO;
                else
                    // Found duplicate user
                    return ERR_LOGGED_IN;
            } else {
                // Password failed
                return ERR_PASS_NO_MATCH;
            }
        }
    }
    return ERR_ID_NO_MATCH;
}

User_List* find_active_user(int id) {
    User_List* next = online_users;
    while (next != NULL) {
        if (next->user.id == id)
            return next;
        next = next->next;
    }
    return NULL;
}

void add_user(User* user, int fd) {
    User_List* curr = online_users;

    User_List* new_user = (User_List*) malloc(sizeof (User_List));
    new_user->user = *user;
    new_user->next = NULL;
    new_user->session_id = -1;
    new_user->fd = fd;

    if (online_users == NULL) {
        online_users = new_user;
        return;
    }

    while (curr->next != NULL)
        curr = curr->next;
    curr->next = new_user;
}

bool delete_user(int id) {
    User_List* prev = NULL;
    User_List* curr = online_users;
    while (curr != NULL) {
        if (curr->user.id == id)
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
    PRINT("User\tFD\tSession\n");

    User_List* curr = online_users;
    while (curr != NULL) {
        PRINT("%d\t%d\t%d\n", curr->user.id, curr->fd, curr->session_id);
        curr = curr->next;
    }
}