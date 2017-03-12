#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../message.h"
#include "../packet_type.h"
#include "user_list.h"
#include "session_list.h"

void login(Message* msg, int fd) {
    PRINT("Login Request: %s, %s\n", msg->source, msg->data);
    char* s = msg->source;
    unsigned id = atoi(s);

    char* pass = msg->data;

    bool valid = authenticate_existing_user(id, pass);

    if (valid) {
        // Add the user to the database
        User* user = (User*) malloc(sizeof (User));
        user->id = id;
        user->password = pass;

        add_user(user, fd);
        PRINT("User Valid\n");

        // Sending LO_ACK
        Message m;
        m.type = LO_ACK;
        deliver_message(&m, fd);
    } else {
        PRINT("User Invalid\n");
        // Sending LO_NACK
        Message m;
        m.type = LO_NAK;
        strcpy(m.data, "Invalid User or Pass\n");
        deliver_message(&m, fd);
    }
}

void exitserver(Message* msg, int fd) {
    char* s;
    sprintf(s, "%s", msg->source);
    unsigned id = atoi(s);

    // Find the user associated
    User_List* user = find_active_user(id);

    // Close the session
    if (user->session_id >= 0) {
        Session* userSession = find_session(user->session_id);
        close_session(userSession->id);
    }

    // Delete the user
    bool deleteSuccess = delete_user(id);
    if (!deleteSuccess)
        fprintf(stderr, "ERROR: unable to delete user with id %d", id);
}

void join(Message* msg, int fd) {
    unsigned id = atoi(msg->source);
    unsigned session_id = atoi(msg->data);

    // Find the user associated
    User_List* user = find_active_user(id);

    if (user == NULL) {
        // Send NACK, user doesn't exist
        Message r;
        r.type = JN_NAK;
        strcpy(r.data, "User Invalid\n");
        deliver_message(&r, fd);
        return;
    }

    // Check that the session exists
    Session* session = find_session(session_id);

    if (session == NULL) {
        // Send NACK, session already exists
        Message r;
        r.type = JN_NAK;
        strcpy(r.data, "Session Doesn't Exist\n");
        deliver_message(&r, fd);
        return;
    }

    // Add the session to the user and the user to the session
    user->session_id = session_id;
    add_user_to_session(session, &user->user);
    FD_SET(fd, &session->ports);
    if(fd > session->fd_max) session->fd_max = fd;
    PRINT("Added User %d to Session %d\n", id, session_id);

    // ACK the joining
    Message r;
    r.type = JN_ACK;
    strcpy(r.data, msg->data);
    deliver_message(&r, fd);
}

void leave_sess(Message* msg, int fd) {
    unsigned id = atoi(msg->source);


    // Find the user associated
    User_List* user = find_active_user(id);

    if (user == NULL) {
        // Send NACK, user doesn't exist
        Message r;
        r.type = JN_NAK;
        strcpy(r.data, "User Invalid\n");
        deliver_message(&r, fd);
        return;
    }

    unsigned session_id = user->session_id;
    if (session_id == -1) return;

    // Check that the session exists
    Session* session = find_session(session_id);

    // Add the session to the user and the user to the session
    user->session_id = -1;
    remove_user_from_session(session, &user->user);

    // If it is the last user then close the session
    if (session_is_empty(session)) close_session(session->id);
}

void new_sess(Message* msg, int fd) {
    unsigned id = atoi(msg->source);
    unsigned session_id = atoi(msg->data);

    // Find the user associated
    User_List* user = find_active_user(id);

    if (user == NULL) {
        // Send NACK, user doesn't exist
        Message r;
        r.type = NS_NACK;
        strcpy(r.data, "User Invalid\n");
        deliver_message(&r, fd);
        return;
    }

    // Check that the session exists
    Session* session = find_session(session_id);

    if (session != NULL) {
        // Send NACK, session already exists
        Message r;
        r.type = NS_NACK;
        strcpy(r.data, "Session Already Exists\n");
        deliver_message(&r, fd);
        return;
    }

    // Opens the session
    open_session(session_id);
    PRINT("Session %d added\n", session_id);

    // Acknowledging the new session
    Message r;
    r.type = NS_ACK;
    deliver_message(&r, fd);
}

void query(Message* msg, int fd) {
    print_active_users();
    print_active_sessions();
    char* sess_str = get_session_string();

    // Need to pack the information about users and sessions back to the client
    Message m;
    m.type = QU_ACK;
    strcpy(m.data, sess_str);

    deliver_message(&m, fd);
}

void message(Message* msg, int fd) {
    unsigned id = atoi(msg->source);

    // Find the user associated
    User_List* user = find_active_user(id);

    if (user == NULL) return;
    unsigned session_id = user->session_id;
    if (session_id == -1) return;

    Session* session = find_session(session_id);
    
    PRINT("BCAST %s: \n", msg->data);
    
    for (int i = 0; i <= session->fd_max; i++) {
        
        if (FD_ISSET(i, &session->ports)) {
            if (i != fd) {
                deliver_message(msg, i);
            }
        }
    }
}

void handle_client_message(Message* msg, int fd) {
    // Handles the message
    if (msg->type == CONNECT)
        return;
    if (msg->type == LOGIN)
        login(msg, fd);
    else if (msg->type == EXIT)
        exitserver(msg, fd);
    else if (msg->type == JOIN)
        join(msg, fd);
    else if (msg->type == LEAVE_SESS)
        leave_sess(msg, fd);
    else if (msg->type == NEW_SESS)
        new_sess(msg, fd);
    else if (msg->type == QUERY)
        query(msg, fd);
    else if (msg->type == MESSAGE)
        message(msg, fd);
    else
        fprintf(stderr, "Incorrect packet type sent by client. Recieved %d", (int) msg->type);
    return;
}
