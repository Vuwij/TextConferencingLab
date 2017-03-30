#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>
#include <stdbool.h>

#include "client.h"
#include "../constants.h" 
#include "../ssl_common.h"

#define STDIN 0

void usage(char *program) {
    fprintf(stderr, "Usage: %s (print source id)\n", program);
    exit(1);
}

void input_usage() {
    PRINT("Invalid Entry, possible usages listed\n");
    PRINT("  /login <client ID> <password> <server-IP> <server-port>\n");
    PRINT("  /logout\n");
    PRINT("  /joinsession <session ID>\n");
    PRINT("  /leavesession\n");
    PRINT("  /createsession <session ID>\n");
    PRINT("  /startcall\n");
    PRINT("  /joincall\n");
    PRINT("  /list\n");
    PRINT("  /quit\n");
    PRINT("  <text>\n");
}

fd_set master;
int fdmax = STDIN;

int main(int argc, char *argv[]) {
    if (argc > 2) usage(argv[0]);
    if (argc == 2) print_src = (PRINT_SRC) atoi(argv[1]);
    PRINT("Started\n");

    // Status of the client
    bool logged_in = false;
    bool in_session = false;
    bool call_started = false;

    // User Input
    char server_ip[MAXBUFSIZE];
    int server_port;
    char client_id[MAXBUFSIZE];
    char password[MAXBUFSIZE];
    char session_id[MAXDATASIZE];

    // Buffer for parsing command and arguments
    char input[100];
    char command[MAXBUFSIZE];

    // File descriptor for standard input
    fd_set read_fds;
    status.voicefd = -1;
    FD_ZERO(&read_fds);
    FD_ZERO(&master);
    FD_SET(STDIN, &master);

    int ret = 0;

    /*************************************************************************
     * SSL will be done when logging in @TCP connection initiated
     ************************************************************************/
    while (1) {
        read_fds = master; // copy it
        // If select returns -1 there was an error, 0 timeOut
        ret = select(fdmax + 1, &read_fds, NULL, NULL, NULL);
        if (ret == -1) {
            perror("select - client");
            exit(4);
        }
        if (ret < 0) {
            PRINT("Server Abnormally Closed Connection\n");
            exit(0);
        }

        // If connection to the server is established
        if (fdmax != STDIN) {
            // Check if there is something to be printed from the network socket
            if (FD_ISSET(status.sockfd, &read_fds)) {
                if (status.ssl != NULL) {
                    Message* msg = receive_message(status.ssl);
                    PRINT("%s: %s\n", msg->source, msg->data);
                }
                Message* msg = receive_message(status.sockfd);

                if (msg->type == ST_CONF_INIT) {
                    join_call();
                    continue;
                }

                PRINT("%s: %s\n", msg->source, msg->data);
                free(msg);
            }
            // Check if there is a voice packet to be played
            if (FD_ISSET(status.voicefd, &read_fds)) {
                int numbytes;
                pthread_mutex_lock(&udp_port_lock);
                if ((numbytes = recvfrom(status.voicefd, &outpacket, sizeof(AudioPacket), 0,
                        (struct sockaddr *) status.udp->ai_addr, &status.udp->ai_addrlen)) == -1) {
                    perror("c: recvfrom");
                    exit(1);
                }
                pthread_mutex_unlock(&udp_port_lock);
                send_buffer_to_output();
            }
        }

        // Check if there was an user input
        if (FD_ISSET(STDIN, &read_fds)) {
            if (fgets(input, 100, stdin) == NULL) continue;
            if (input[0] == '\n') continue; // ignore line feed

            if (input[0] != '/') {
                if (!logged_in) {
                    PRINT("Not logged into a server\n");
                    continue;
                }
                if (!in_session) {
                    PRINT("Not joined a session yet\n");
                    continue;
                }
                send_message(input);
            } else {
                if (sscanf(input, "%s %s %s %s %d", command, client_id, password, server_ip, &server_port) == 5) {
                    if (strncmp(command, "/login", strlen("/login")) != 0) {
                        input_usage();
                        continue;
                    }
                    if (logged_in) {
                        PRINT("Already logged into a session\n");
                        continue;
                    }
                    bool success = login(client_id, password, server_ip, server_port);
                    if (success) {
                        logged_in = true;
                        FD_SET(status.sockfd, &master);
                        fdmax = status.sockfd;
                    }
                }// Join and Create Session
                else if (sscanf(input, "%s %s", command, session_id) == 2) {
                    if (strncmp(command, "/joinsession", strlen("/joinsession")) == 0) {

                        if (!logged_in) {
                            PRINT("Not logged into any server\n");
                            continue;
                        }
                        if (in_session) {
                            PRINT("Already in session\n");
                            continue;
                        }
                        bool success = join_session(session_id);
                        if (success) in_session = true;
                    } else if (strncmp(command, "/createsession", strlen("/createsession")) == 0) {
                        if (!logged_in) {
                            PRINT("Not logged into any server\n");
                            continue;
                        }
                        if (in_session) {
                            PRINT("Already in session\n");
                            continue;
                        }
                        if (create_session(session_id))
                            in_session = true;
                    } else {
                        input_usage();
                    }
                }// Logout, list, quit or message
                else if (sscanf(input, "%s", command) == 1) {
                    if (strncmp(command, "/logout", strlen("/logout")) == 0) {
                        if (!logged_in) {
                            PRINT("Not logged into any server\n");
                            continue;
                        }
                        if (in_session) {
                            bool success = leave_session();
                            if (success) in_session = false;
                            else continue;
                        }
                        bool success = logout();
                        if (success) {
                            logged_in = false;
                            FD_CLR(status.sockfd, &master);
                            status.sockfd = -1;
                        }
                    } else if (strncmp(command, "/leavesession", strlen("/leavesession")) == 0) {
                        if (!logged_in) {
                            PRINT("Not logged into any server\n");
                            continue;
                        }
                        if (!in_session) {
                            PRINT("Not entered into any session\n");
                            continue;
                        }
                        leave_session();
                        assert(in_session == true);
                        in_session = false;
                    } else if (strncmp(command, "/list", strlen("/list")) == 0) {
                        if (!logged_in) {
                            PRINT("Not logged into any server\n");
                            continue;
                        }
                        list();
                    } else if (strncmp(command, "/startcall", strlen("/startcall")) == 0) {
                        if (!logged_in) {
                            PRINT("Not logged into any server\n");
                            continue;
                        }
                        if (!in_session) {
                            PRINT("Not entered into any session\n");
                            continue;
                        }
                        if (call_started) {
                            PRINT("Already in call\n");
                            continue;
                        }
                        start_call();
                    } else if (strncmp(command, "/joincall", strlen("/joincall")) == 0) {
                        if (!logged_in) {
                            PRINT("Not logged into any server\n");
                            continue;
                        }
                        if (!in_session) {
                            PRINT("Not entered into any session\n");
                            continue;
                        }
                        if (!call_started) {
                            PRINT("No active calls in session\n");
                            continue;
                        }
                        join_call();
                    } else if (strncmp(command, "/quit", strlen("/quit")) == 0) {
                        if (in_session) {
                            bool success = leave_session();
                            if (success) in_session = false;
                            else continue;
                        }
                        if (logged_in) {
                            bool success = logout();
                            if (success) logged_in = false;
                            else continue;
                        }
                        quit();
                    } else
                        input_usage();
                } else {
                    input_usage();
                }
            }
        }
    }
    return 0;
}
