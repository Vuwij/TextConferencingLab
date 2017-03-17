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
#include <assert.h>
#include <sys/select.h>

#include "session_list.h"
#include "user_list.h"
#include "../message.h"

Session_List* session_list;

Session* open_session(char *id) {
    Session_List* curr = session_list;
    Session_List* new_session = (Session_List*) malloc(sizeof (Session_List));
    new_session->next = NULL;
    strcpy(new_session->session.id, id);
    new_session->session.fd_max = -1;
    new_session->session.num_user = 0;
    FD_ZERO(&new_session->session.client_fds);
    int i;
    for (i = 0; i < MAX_USERS_PER_SESSION; i++)
        new_session->session.users[i] = NULL;

    if (session_list == NULL) {
        session_list = new_session;
        return &(session_list->session);
    }

    while (curr->next != NULL)
        curr = curr->next;

    curr->next = new_session;
    return &(new_session->session);
}

Session* find_session(char *id) {
    Session_List* curr = session_list;
    while (curr != NULL) {
        if (strcmp(curr->session.id, id) == 0)
            return &(curr->session);
        curr = curr->next;
    }
    return NULL;
}

void close_session(char *session_id) {
    Session_List* curr = session_list;
    Session_List* prev = NULL;

    while (curr != NULL) {
        if (strcmp(curr->session.id, session_id) == 0)
            break;
        prev = curr;
        curr = curr->next;
    }


    if (curr == NULL) {
        fprintf(stderr, "Attempted to close session %s, but it doesn't exists\n", session_id);
        return;
    }

    assert(curr->session.num_user == 0);

    //Found it deleting it
    if (prev == NULL)
        session_list = NULL;
    else
        prev->next = curr->next;
    curr->next = NULL;
    free(curr);

}

void add_user_to_session(Session* session, User* user) {
    int avail_slot = NOT_AVAIL;
    assert(session != NULL);
    // Check for existing user
    for (int i = 0; i < MAX_USERS_PER_SESSION; i++) {
        if (session->users[i] == NULL) {
            if (avail_slot == NOT_AVAIL)
                avail_slot = i;
            continue;
        }
        if (session->users[i]->id == user->id)
            fprintf(stderr, "ERROR: User %d already exists in session %d", user->id, session->id);
    }

    if (avail_slot == NOT_AVAIL) {
        fprintf(stderr, "ERROR: Session is Full\n");
    } else {
        session->users[avail_slot] = user;
        session->num_user++;
    }
}

/* Remove the user through iterating*/
void remove_user_from_session(Session *session, User_List* user) {
    for (int i = 0; i < MAX_USERS_PER_SESSION; i++) {
        if (session->users[i] == NULL)
            continue;

        if (session->users[i]->id == user->user->id) {
            session->users[i] = NULL;

            FD_CLR(user->fd, &session->client_fds);
            session->num_user--;
            return;
        }
    }
    fprintf(stderr, "ERROR: Cannot delete User %d in session %d. Does not exist", user->user->id, session->id);
}

/* Get a list of users in a session*/
User** get_users_in_session(Session * session) {

    int index = 0;

    User** users;
    users = (User**) malloc(sizeof (User*) * session->num_user);

    for (int i = 0; i < MAX_USERS_PER_SESSION; i++) {
        if (session->users[i] == NULL)
            continue;
        else {
            index++;
            users[index] = users[i];
        }
    }
    assert(index == session->num_user - 1);
    return users;
}

bool is_session_empty(Session * session) {
    //testing
    if (session->num_user == 0)
        return true;
    else
        return false;
}

void print_active_sessions() {
    PRINT("Active Sessions:\n");

    Session_List* s = session_list;

    while (s != NULL) {
        PRINT("Session %s\n", s->session.id);

        for (int i = 0; i < MAX_USERS_PER_SESSION; i++) {
            if (s->session.users[i] == NULL)
                continue;

            PRINT("  User %s\n", s->session.users[i]->id);
        }
        s = s->next;
    }
}

void get_session_string(char *sess_str) {
    Session_List* s = session_list;

    char id_str[MAXDATASIZE];
    char user_id_str[MAXDATASIZE];
    int i = 0;

    bzero(sess_str, MAXDATASIZE);
    strncat(sess_str, "Active: \n", MAXDATASIZE);
    while (s != NULL) {
        snprintf(id_str, MAXDATASIZE, "Session %s - ", s->session.id);
        strncat(sess_str, id_str, MAXDATASIZE);
        int count = 0;
        for (int i = 0; i < MAX_USERS_PER_SESSION; i++) {
            if (s->session.users[i] == NULL)
                continue;
            snprintf(user_id_str, MAXDATASIZE, "%s ", s->session.users[i]->id);
            strncat(sess_str, user_id_str, MAXDATASIZE);
            count++;
        }
        if (count == 0)
            snprintf(user_id_str, MAXDATASIZE, "None");
        strncat(sess_str, user_id_str, MAXDATASIZE);
        strncat(sess_str, "\n", MAXDATASIZE);
        s = s->next;
    }
}

void start_call(Session* session) {
    // Send clients TCP message telling them to open UDP ports to the data port
    Message m, *r;
    m.type = ST_CONF_INIT;
    
    for(int i = 0; i < session->fd_max; i++) {
        if(FD_ISSET(i, &session->client_fds)) {
            deliver_message(&m, i);
            r = receive_message(i);
            if(r->type == ST_CONF_INIT_ACK) {
                // Recieved reply from the user
            }
        }
    }
}

void end_call(Session* session) {
    // Send clients TCP message telling them to close UDP ports
    
    // Hear client TCP reply, then close the session
}