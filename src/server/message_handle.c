#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "../message.h"
#include "../packet_type.h"
#include "user_list.h"
#include "session_list.h"
#include "server.h"

void login(Message* msg, int fd) {
    PRINT("Login Request: %s, %s\n", msg->source, msg->data);

    // Check User ID and Password
    char* s = msg->source;
    unsigned id = atoi(s);
    char* pass = msg->data;

    Message m;
    int authen_status = authenticate_existing_user(id, pass);
    switch (authen_status) {
        case ERR_NO:
            PRINT("User Valid\n");
            // Add the user to the database
            User* user = (User*) malloc(sizeof (User));
            user->id = id;
            user->password = pass;
            add_user(user, fd);
            m.type = LO_ACK;
            break;
        case ERR_LOGGED_IN:
            PRINT("User Already Logged In\n");
            m.type = LO_NAK;
            strncpy(m.data, "User Already Logged In\n", strlen("User Already Logged In\n"));
            break;
        case ERR_ID_NO_MATCH:
            PRINT("User ID not found\n");
            m.type = LO_NAK;
            strncpy(m.data, "User ID not found\n", strlen("User ID not found\n"));
            break;
        case ERR_PASS_NO_MATCH:
            PRINT("User Password does not match\n");
            m.type = LO_NAK;
            strncpy(m.data, "User Password does not match\n", strlen("User Password does not match\n"));
            break;
        default:
            PRINT("Server Error Occurred\n");
            exit(0);
    }
    deliver_message(&m, fd);
}

/*Some issue with this need to come back here*/
void exitserver(Message* msg, int fd) {
    char s[MAX_NAME];
    snprintf(s, MAX_NAME, "%s", msg->source);
    printf("%s\n", msg->source);
    unsigned id = atoi(s);

    // Find the user associated
    User_List* user = find_active_user_fd(fd);
    Session * userSession;
    if (user != NULL)
        userSession = find_session(user->session_id);

    // Remove user from the session
    if (userSession != NULL) {
        remove_user_from_session(userSession, user);

        // If it is the last user then close the session
        if (is_session_empty(userSession))
            close_session(userSession->id);
    } else {
        FD_CLR(fd, &master);
    }
    // Delete User from UserList
    bool deleteSuccess = delete_user(user->user.id);
    print_active_users();
    if (!deleteSuccess)
        fprintf(stderr, "ERROR: unable to delete user with id %d", user->user.id);
    else
        close(fd);
}

void join(Message* msg, int fd) {
    unsigned id = atoi(msg->source);
    unsigned session_id = atoi(msg->data);

    Message r;

    // Check user and session
    User_List* user = find_active_user(id);
    Session* session = find_session(session_id);

    if (user == NULL) {
        r.type = JN_NAK;
        strncpy(r.data, "User Invalid\n", strlen("User Invalid\n"));
        assert(0); //I believe this doesn't ever occur or should never occur
    } else if (session == NULL) {
        r.type = JN_NAK;
        strncpy(r.data, "Session Doesn't Exist\n", strlen("Session Doesn't Exist\n"));
    } else if (user->session_id >= 0) {
        r.type = JN_NAK;
        strncpy(r.data, "Joined Session Already\n", strlen("Joined Session Already\n"));
    } else {
        r.type = JN_ACK;
        PRINT("%s\n", msg->data);
        strncpy(r.data, msg->data, sizeof (msg->data));

        // Add user to the session and vice versa
        user->session_id = session_id;
        add_user_to_session(session, &user->user);
        assert(fd == user->fd);
        FD_SET(fd, &session->client_ports);
        if (fd > session->fd_max)
            session->fd_max = fd;
        PRINT("Added User %d to Session %d\n", id, session_id);
    }

    deliver_message(&r, fd);
}

void leave_sess(Message* msg, int fd) {
    unsigned id = atoi(msg->source);

    // Find the user associated
    User_List* user = find_active_user(id);
    assert(user != NULL);

    unsigned session_id = user->session_id;
    if (session_id == -1) {
        PRINT("User Has not Joined Any session\n");
        return;
    }
    // Check that the session exists
    Session* session = find_session(session_id);
    assert(session != NULL);

    // Remove the session and the user from the session
    user->session_id = -1;
    remove_user_from_session(session, user);

    // If it is the last user then close the session
    if (is_session_empty(session))
        close_session(session->id);
}

void new_sess(Message* msg, int fd) {
    unsigned id = atoi(msg->source);
    unsigned session_id = atoi(msg->data);

    // Find the user associated
    User_List* user = find_active_user(id);
    assert(user != NULL);

    // Check that the session exists
    Session* session = find_session(session_id);
    Message r;
    if (session != NULL) {
        strncpy(r.data, "Session Already Exists\n", strlen("Session Already Exists\n"));
        r.type = NS_NAK;
    } else {
        PRINT("Session %d added\n", session_id);
        r.type = NS_ACK;
        open_session(session_id);

    }
    deliver_message(&r, fd);


    //If the session didn't exist but then made one then join the session
    if (session == NULL) {
        session = find_session(session_id);
        r.type = JN_ACK;
        PRINT("%s\n", msg->data);
        strncpy(r.data, msg->data, sizeof (msg->data));

        // Add user to the session and vice versa
        user->session_id = session_id;
        add_user_to_session(session, &(user->user));
        assert(fd == user->fd);
        FD_SET(fd, &session->client_ports);
        if (fd > session->fd_max)
            session->fd_max = fd;
        PRINT("Added User %d to Session %d\n", id, session_id);
        deliver_message(&r, fd);
    }
}

void query(Message* msg, int fd) {
    print_active_users();
    print_active_sessions();

    char sess_str[MAXDATASIZE];
    get_session_string(sess_str);
    // Need to pack the information about users and sessions back to the client
    Message m;
    m.type = QU_ACK;
    strncpy(m.data, sess_str, MAX_DATA);

    deliver_message(&m, fd);
}

void message(Message* msg, int fd) {
    unsigned id = atoi(msg->source);

    // Find the user associated
    User_List* user = find_active_user(id);
    assert(user != NULL);

    int session_id = user->session_id;

    // If the user has not joined the session
    if (session_id == -1)
        return;

    Session* session = find_session(session_id);
    assert(session != NULL);

    PRINT("BCAST: %s", msg->data);

    for (int i = 0; i <= session->fd_max; i++) {

        if (FD_ISSET(i, &session->client_ports)) {
            if (i != fd) {
                deliver_message(msg, i);
            }
        }
    }

}

/* Handles the message */
void handle_client_message(Message* msg, int fd) {

    switch (msg->type) {
        case CONNECT:
            break;
        case LOGIN:
            login(msg, fd);
            break;
        case EXIT:
            exitserver(msg, fd);
            break;
        case JOIN:
            join(msg, fd);
            break;
        case LEAVE_SESS:
            leave_sess(msg, fd);
            break;
        case NEW_SESS:
            new_sess(msg, fd);
            break;
        case MESSAGE:
            message(msg, fd);
            break;
        case QUERY:
            query(msg, fd);
            break;
        default:
            fprintf(stderr, "Incorrect packet type sent by client. Received %d", (int) msg->type);
    }
}


