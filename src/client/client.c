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

void usage(char *program) {
    fprintf(stderr, "Usage: %s\n", program);
    exit(1);
}

void input_usage() {
    printf("Invalid Entry, possible usages listed\n");
    printf("  /login <client ID> <password> <server-IP> <server-port>\n");
    printf("  /logout\n");
    printf("  /joinsession <session ID>\n");
    printf("  /leavesession\n");
    printf("  /createsession <session ID>\n");
    printf("  /list\n");
    printf("  /quit\n");
    printf("  <text>\n");
}

int main(int argc, char *argv[]) {
    if (argc != 1) usage(argv[0]);
    
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
   
    while (fgets(input, 100, stdin) != NULL) {
        // Login
        if (sscanf(input, "%s %d %s %s %d", command, &client_id, password, server_ip, &server_port) == 5) {;
            if(strcmp(command, "/login") != 0) {
                input_usage();
                continue;
            }
            printf("s\n");
            if(logged_in) {
                printf("Already logged into a session\n");
                continue;
            }

            login(client_id, password, server_ip, server_port);
            logged_in = true;
        }
        // Create and Join Session
        else if (sscanf(input, "%s %d", command, &session_id) == 2) {
            if(strcmp(command, "/joinsession") == 0) {
                if(!logged_in) {
                    printf("Not logged into any server\n");
                    continue;
                }
                if(in_session) {
                    printf("Already in session\n");
                    continue;
                }
                join_session(session_id);
                in_session = true;
            }
            else if(strcmp(command, "/createsession") == 0) {
                if(!logged_in) {
                    printf("Not logged into any server\n");
                    continue;
                }
                if(in_session) {
                    printf("Already in session\n");
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
                    printf("Not logged into any server\n");
                    continue;
                }
                if(in_session) {
                    leave_session();
                    in_session = false;
                }
                logout();
                logged_in = false;
            }
            else if(strcmp(command, "/leavesession") == 0) {
                if(!logged_in) {
                    printf("Not logged into any server\n");
                    continue;
                }
                if(!in_session) {
                    printf("Not entered into any session\n");
                    continue;
                }
                leave_session();
                in_session = false;
            }
            else if(strcmp(command, "/list") == 0) {
                if(!logged_in) {
                    printf("Not logged into any server\n");
                    continue;
                }
                list();
            }
            else if(strcmp(command, "/quit") == 0) {
                if(in_session) {
                    leave_session();
                    in_session = false;
                }
                if(logged_in) {
                    logout();
                    logged_in = false;
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
    
    return 0;
}