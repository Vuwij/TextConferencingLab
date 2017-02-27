#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../message.h"
#include "../packet_type.h"
#include "user_list.h"
#include "session_list.h"

void login(Message* msg) {
    char* s;
    sprintf(s, "%s", msg->source);
    unsigned id = atoi(s);
    
    char* pass;
    sprintf(pass, "%s", msg->data);
    
    bool valid = authenticate_existing_user(id, pass);
    
    if(valid) {
        // Add the user to the database
        User user; 
        user.id = id;
        user.password = pass;
        
        add_user(&user);
    }
}

void exitserver(Message* msg) {
    char* s;
    sprintf(s, "%s", msg->source);
    unsigned id = atoi(s);
    
    // Find the user associated
    User_List* user = find_active_user(id);
    
    // Close the session
    if(user->session_id >= 0) {
        Session* userSession = find_session(user->session_id);
        close_session(userSession->id);
    }
    
    // Delete the user
    bool deleteSuccess = delete_user(id);
    if(!deleteSuccess)
        fprintf(stderr, "ERROR: unable to delete user with id %d", id);
}

void join(Message* msg) {
    char* s;
    sprintf(s, "%s", msg->source);
    unsigned id = atoi(s);
    
    sprintf(s, "%s", msg->data);
    unsigned session_id = atoi(s);
    
    // Find the user associated
    User_List* user = find_active_user(id);
    
    if(user == NULL) {
        // Send NACK, user doesn't exist
        return;
    }
    
    // Check that the session exists
    Session* session = find_session(session_id);
    
    if(session == NULL) {
        // Send NACK, session doesn't exist
        return;
    }
    
    // Add the session to the user and the user to the session
    user->session_id = session_id;
    add_user_to_session(session, &user->user);
}

void leave_sess(Message* msg) {
    char* s;
    sprintf(s, "%s", msg->source);
    unsigned id = atoi(s);
    
    sprintf(s, "%s", msg->data);
    unsigned session_id = atoi(s);
    
    // Find the user associated
    User_List* user = find_active_user(id);
    
    if(user == NULL) {
        // Send NACK, user doesn't exist
        return;
    }
    
    // Check that the session exists
    Session* session = find_session(session_id);
    
    if(session == NULL) {
        // Send NACK, session doesn't exist
        return;
    }
    
    // Add the session to the user and the user to the session
    user->session_id = session_id;
    remove_user_from_session(session, &user->user);
    
    // If it is the last user then close the session
    if(session_is_empty(session)) close_session(session->id);
}

void new_sess(Message* msg) {
    char* s;
    sprintf(s, "%s", msg->source);
    unsigned id = atoi(s);
    
    sprintf(s, "%s", msg->data);
    unsigned session_id = atoi(s);
    
    // Find the user associated
    User_List* user = find_active_user(id);
    
    if(user == NULL) {
        // Send NACK, user doesn't exist
        return;
    }
    
    // Check that the session exists
    Session* session = find_session(session_id);
    
    if(session != NULL) {
        // Send NACK, session already exists
        return;
    }
    
    // Opens the session
    open_session(session_id);
}

void query(Message* msg) {
    //print_active_users();
    //print_active_sessions();
    
    // Need to pack the information about users and sessions back to the client
}

void handle_client_message(char* buf, int fd) {
    Message msg;
    msg = deserialize_message(buf, sizeof(buf));
    
    // Handles the message
    if(msg.type == LOGIN)
        login(&msg);
    else if (msg.type == EXIT)
        exitserver(&msg);
    else if (msg.type == JOIN)
        join(&msg);
    else if (msg.type == LEAVE_SESS)
        leave_sess(&msg);
    else if (msg.type == NEW_SESS)
        new_sess(&msg);
    else if (msg.type == QUERY)
        query(&msg);
    else
        fprintf(stderr, "Incorrect packet type sent by client. Recieved %d", (int) msg.type);
}
