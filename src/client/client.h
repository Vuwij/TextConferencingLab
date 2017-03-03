#ifndef CLIENT_H
#define	CLIENT_H

#include "../constants.h"
#include "../message.h"
#include "../packet_type.h"
#include "status.h"

extern Status status;

bool login(int client_id, char* password, char* server_ip, int server_port);

bool logout();

bool join_session(unsigned session_id);

bool leave_session();

bool create_session(unsigned session_id);

bool list();

bool quit();

bool send_message(char* message);

#endif	/* CLIENT_H */

