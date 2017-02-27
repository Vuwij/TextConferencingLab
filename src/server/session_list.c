#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include "session_list.h"

Session_List session_list;

Session* open_session(int id) {
    Session_List* next = &session_list;
    while(next != NULL) {
        if(next->session.id == id) {
            fprintf(stderr, "Session %d already exists\n", id);
            return NULL;
        }
        next = next->next;
    }
    next->next = (Session_List*) malloc(sizeof(Session_List));
    next->next->session = *((Session*) malloc(sizeof(Session)));
    next->next->session.id = id;
    FD_ZERO(&next->next->session.ports);
    return &next->next->session;
}

Session* find_session(int id) {
    Session_List* next = &session_list;
    while(next != NULL) {
        if(next->session.id == id) return &next->session;
        next = next->next;
    }
    return NULL;
}

void close_session(int id) {
    Session_List* next = &session_list;
    Session_List* previous = &session_list;
    while(next != NULL) {
        if(next->session.id == id) {
            previous->next = next->next;
            next->next = NULL;
            free(next);
            printf("Session %d closed", id);
            return;
        }
        next = next->next;
        previous = next;
    }
    fprintf(stderr, "Attempted to close session %d, but it doesn't exists\n", id);
}

void add_user_to_session(Session* session, User* user) {
    // Check for existing user
    for(int i = 0; i < MAX_USERS_PER_SESSION; i++) {
        if(session->users[i] == NULL) continue;
        if(session->users[i]->id == user->id) {
            fprintf(stderr, "ERROR: User %d already exists in session %d", user->id, session->id);
        }
    }
    
    // Add the user to the first slot
    for(int i = 0; i < MAX_USERS_PER_SESSION; i++) {
        if(session->users[i] == NULL) {
            session->users[i] = user;
        }
    }
}

void remove_user_from_session(Session* session, User* user) {
    // Remove the user through iterating
    for(int i = 0; i < MAX_USERS_PER_SESSION; i++) {
        if(session->users[i] == NULL) continue;
        
        if(session->users[i]->id == user->id) {
            session->users[i] = NULL;
            return;
        }
    }
    fprintf(stderr, "ERROR: Cannot delete User %d in session %d. Does not exist", user->id, session->id);
}

User** get_users_in_session(Session* session) {
    // Get the total number of users in the session
    int count = 0;
    for(int i = 0; i < MAX_USERS_PER_SESSION; i++) {
        if(session->users[i] == NULL) continue;
        else count++;
    }
    
    User** users;
    users = (User**) malloc(sizeof(User*) * count);
    
    for(int i = 0; i < MAX_USERS_PER_SESSION; i++) {
        if(session->users[i] == NULL) continue;
        else {
            count--;
            users[count] = users[i];
        }
    }
    
    return users;
}

bool session_is_empty(Session* session) {
    for(int i = 0; i < MAX_USERS_PER_SESSION; i++) {
        if(session->users[i] != NULL) return false;
    }
    return true;
}

void print_active_sessions() {
    
}