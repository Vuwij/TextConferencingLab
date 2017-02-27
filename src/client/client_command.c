#include <stdio.h>
#include <stdlib.h>
#include "client.h"

void login(int client_id, char* password, char* server_ip, int server_port) {
    printf("Logging into %d with password %s at %s %d\n", client_id, password, server_ip, server_port);
}

void logout() {
    printf("Logging out\n");
}

void join_session(unsigned session_id) {
    printf("Joining session %d\n", session_id);
}

void leave_session() {
    printf("Leaving session\n");
}

void create_session(unsigned session_id) {
    printf("Creating session %d\n", session_id);
}

void list() {
    printf("Listing Sessions\n");
}

void quit() {
    printf("Quitting\n");
    exit(1);
}

void send_message(char* message) {
    
}