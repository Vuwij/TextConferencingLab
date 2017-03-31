#ifndef CLIENT_H
#define CLIENT_H

#include "../constants.h"
#include "../message.h"
#include "../packet_type.h"
#include "status.h"
#include "audio_output.h"
#include "audio_input.h"

/* use these strings to tell the marker what is happening */
#define FMT_CONNECT_ERR "CLIENT: SSL connect error\n"
#define FMT_SERVER_INFO "CLIENT: %s %s %s\n"
#define FMT_OUTPUT "CLIENT: %s %s\n"
#define FMT_INCORRECT_CLOSE "CLIENT: Premature close\n"

#define PASSWORD "password"
#define KEY_FILE_PATH "certificate/alice.pem"
#define EXPECTED_HOST_NAME "Bob's server"
#define EXPECTED_SERVER_EMAIL "ece568bob@ecf.utoronto.ca"

extern Status status;
extern fd_set master;
extern int fdmax;

bool login(char* client_id, char* password, char* server_ip, int server_port);

bool logout();

bool join_session(char *session_id);

bool leave_session();

bool create_session(char *session_id);

bool list();

bool quit();

bool send_message(char* message);

bool start_call();

bool join_call();

/*******************************************************************************
 *  SSL Functions
 ******************************************************************************/
void verify_server_cert(SSL *ssl, char *host, char*email);

void clean_up(int sock, SSL *ssl);


#endif /* CLIENT_H */

