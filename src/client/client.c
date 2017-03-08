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
    PRINT("  /list\n");
    PRINT("  /quit\n");
    PRINT("  <text>\n");
}

int main(int argc, char *argv[]) {
    if (argc > 2) usage(argv[0]);
    if (argc == 2) print_src = (PRINT_SRC) atoi(argv[1]);
    PRINT("Started\n");
    
    // Status of the client
    bool logged_in;
    bool in_session;
    
    // Information that is inputted
    char server_ip[MAXBUFSIZE];
    int server_port;
    int client_id;
    char password[MAXBUFSIZE];
    int session_id;
    
    // The main parse loop
    char input[100];
    char command[MAXBUFSIZE];
   
    // File descriptor for standard input
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 50000;
    fd_set master;
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(STDIN, &master);
    int fdmax = STDIN;
    
    while (1) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }
        
        if(fdmax != STDIN) {
            if(FD_ISSET(status.sockfd, &read_fds)) {
                Message* msg = receive_message(status.sockfd);
                PRINT("%s: %s\n", msg->source, msg->data);
            }
        }
        
        if (FD_ISSET(STDIN, &read_fds)) {
            if(fgets(input, 100, stdin) == NULL) continue;
        
            // Login
            if (sscanf(input, "%s %d %s %s %d", command, &client_id, password, server_ip, &server_port) == 5) {;
                if(strcmp(command, "/login") != 0) {
                    input_usage();
                    continue;
                }
                if(logged_in) {
                    PRINT("Already logged into a session\n");
                    continue;
                }
                bool success = login(client_id, password, server_ip, server_port);
                if(success) {
                    logged_in = true;
                    FD_SET(status.sockfd, &master);
                    fdmax = status.sockfd;
                }
            }
            // Create and Join Session
            else if (sscanf(input, "%s %d", command, &session_id) == 2) {
                if(strcmp(command, "/joinsession") == 0) {
                    if(!logged_in) {
                        PRINT("Not logged into any server\n");
                        continue;
                    }
                    if(in_session) {
                        PRINT("Already in session\n");
                        continue;
                    }
                    bool success = join_session(session_id);
                    if(success) in_session = true;
                }
                else if(strcmp(command, "/createsession") == 0) {
                    if(!logged_in) {
                        PRINT("Not logged into any server\n");
                        continue;
                    }
                    if(in_session) {
                        PRINT("Already in session\n");
                        continue;
                    }
                    create_session(session_id);
                }
                else {
                    input_usage();
                }
            }
            // Logout, list, quit or message
            else if (sscanf(input, "%s", command) == 1) {
                if(strcmp(command, "/logout") == 0) {
                    if(!logged_in) {
                        PRINT("Not logged into any server\n");
                        continue;
                    }
                    if(in_session) {
                        bool success = leave_session();
                        if(success) in_session = false;
                        else continue;
                    }
                    bool success = logout();
                    if(success) logged_in = false;
                }
                else if(strcmp(command, "/leavesession") == 0) {
                    if(!logged_in) {
                        PRINT("Not logged into any server\n");
                        continue;
                    }
                    if(!in_session) {
                        PRINT("Not entered into any session\n");
                        continue;
                    }
                    leave_session();
                    in_session = false;
                }
                else if(strcmp(command, "/list") == 0) {
                    if(!logged_in) {
                        PRINT("Not logged into any server\n");
                        continue;
                    }
                    list();
                }
                else if(strcmp(command, "/quit") == 0) {
                    if(in_session) {
                        bool success = leave_session();
                        if(success) in_session = false;
                        else continue;
                    }
                    if(logged_in) {
                        bool success = logout();
                        if(success) logged_in = false;
                        else continue;
                    }
                    quit();
                }
                else {
                    if(!logged_in) {
                        input_usage();
                        continue;
                    }
                    if(!in_session) {
                        input_usage();
                        continue;
                    }
                    send_message(command);
                }
            }
            else {
                input_usage();
            }
        }
    }
    
    return 0;
}