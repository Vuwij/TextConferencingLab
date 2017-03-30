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
#include "ssl_common.h"
#include "client/status.h"

#define INT_STRING_LEN 10

int deliver_message(Message* message, SSL *ssl) {
    message->size = sizeof (message->data);

    if (DEBUG_MSG) print_message(message);

    int numbytes = SSL_write(ssl, message, sizeof (Message));
    switch (SSL_get_error(ssl, numbytes)) {
        case SSL_ERROR_NONE:
            if (sizeof (Message) != numbytes)
                err_exit("Incomplete write!");
            break;
        default:
            berr_exit("SSL write problem");
    }
    return numbytes;
}

Message* receive_message(SSL *ssl) {
    Message* msg = (Message*) malloc(sizeof (Message));

    int numbytes;
    // Read the server's response
    numbytes = SSL_read(ssl, msg, sizeof (Message));
    if (numbytes < 0)
        berr_exit("SSL read\n");
    else if (numbytes == 0)
        berr_exit("Client: Server just hung up\n");

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
