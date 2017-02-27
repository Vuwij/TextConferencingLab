#ifndef SESSIONS_H
#define	SESSIONS_H

#include "session.h"

typedef struct session_list {
    Session session;
    struct session_list* next; 
} Session_List;

extern Session_List session_list;

Session* open_session(int id);

Session* find_session(int id);

void close_session(int id);

void add_user_to_session(Session* session, User* user);

void remove_user_from_session(Session* session, User* user);

User** get_users_in_session(Session* session);

bool session_is_empty(Session* session);

void print_active_sessions();

#endif	/* SESSIONS_H */

