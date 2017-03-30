#ifndef SESSIONS_H
#define SESSIONS_H

#include "session.h"
#include "user_list.h"

typedef struct sess_list {
    Session session;
    struct sess_list* next;
} Session_List;

Session* open_session(char* id);

Session* find_session(char* id);

void close_session(char* id);

void add_user_to_session(Session* session, User* user);

void remove_user_from_session(Session* session, User_List* user);

User** get_users_in_session(Session* session);

bool is_session_empty(Session* session);

void print_active_sessions();

void get_session_string(char *sess_str);

void start_call(Session* session);

void end_call(Session* session);

#endif /* SESSIONS_H */

