#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "session_list.h"

Session_List* session_list;

Session* open_session(int id) {
    Session_List* next = session_list;
    if(session_list == NULL) {
        session_list = (Session_List*) malloc(sizeof(Session_List));
        session_list->session.id = id;
        return &session_list->session;
    }
    
    while(next->next != NULL)
        next = next->next;
    
    next->next = (Session_List*) malloc(sizeof(Session_List));
    next->next->session.id = id;
    FD_ZERO(&next->next->session.ports);
    return &next->next->session;
}

Session* find_session(int id) {
    Session_List* next = session_list;
    while(next != NULL) {
        if(next->session.id == id) return &next->session;
        next = next->next;
    }
    return NULL;
}

void close_session(int id) {
    Session_List* next = session_list;
    Session_List* previous = session_list;
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
            return;
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
    Session_List* s = session_list;
    
    PRINT("Active Sessions:\n");
    while(s != NULL) {
        PRINT("Session %d\n", s->session.id);
        for(int i = 0; i < MAX_USERS_PER_SESSION; i++) {
            if(s->session.users[i] == NULL) break;
            PRINT("  User %d\n", s->session.users[i]->id);
        }
        s = s->next;
    }
}

char* get_session_string() {
    Session_List* s = session_list;
    char* sess_str = "Active Sessions:\n";
    
    while(s != NULL) {
        char* id_str;
        sprintf(id_str, "Session %d - ", s->session.id);
        strcat(sess_str, id_str);
        for(int i = 0; i < MAX_USERS_PER_SESSION; i++) {
            if(s->session.users[i] == NULL) continue;
            
            char* user_id_str;
            sprintf(user_id_str, "%d, ", s->session.users[i]->id);
            strcat(sess_str, user_id_str);
        }
        s = s->next;
    }
    return sess_str;
}