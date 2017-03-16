#ifndef CLIENT_H
#define CLIENT_H

#include "../constants.h"
#include "../message.h"
#include "../packet_type.h"
#include "status.h"
#include "audio_input.h"

extern Status status;

bool login(char* client_id, char* password, char* server_ip, int server_port);

bool logout();

bool join_session(char *session_id);

bool leave_session();

bool create_session(char *session_id);

bool list();

bool quit();

bool send_message(char* message);

bool start_call();

bool end_call();

#endif /* CLIENT_H */

