#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "message.h"
#include "constants.h"
#include "strings.h"
#include "string.h"

#define INT_STRING_LEN 10

int deliver_message(Message* message, int sockfd) {
    message->size = sizeof (message->data);

    if (DEBUG_MSG) print_message(message);

    int numbytes = 0;
    if ((numbytes = send(sockfd, message, sizeof (Message), 0)) == -1) {
        perror("client: sendto");
        exit(1);
    }
    return numbytes;
}

Message* receive_message(int sockfd) {
    Message* msg = (Message*) malloc(sizeof (Message));

    int numbytes;
    if ((numbytes = recv(sockfd, msg, sizeof (Message), 0)) <= 0) {
        if (numbytes == 0) PRINT("Client : Server Just Hung Up\n");
        else perror("recv");
        exit(1);
    }

    if (DEBUG_MSG) print_message(msg);

    return msg;
}

void print_message(Message* message) {
    PRINT("----------------\n");
    PRINT("TYPE: %d\n", message->type);
    PRINT("SIZE: %d\n", message->size);
    PRINT("SRC: %s\n", message->source);
    PRINT("DATA: %s\n", message->data);
    //char* s_msg = serialize_message(message);
    //PRINT("[%s]\n", s_msg);
    PRINT("----------------\n");
}
