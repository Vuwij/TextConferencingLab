#ifndef MESSAGE_H
#define	MESSAGE_H

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

#include "packet_type.h"
// The message protocol that will be used for the transfer of data

#define MAX_NAME 100
#define MAX_DATA 1000

typedef struct {
    control_packet_t type;
    unsigned int size;
    unsigned char source[MAX_NAME];
    unsigned char data[MAX_DATA];
}Message;

int deliver_message(Message* message, int sockfd);

Message* receive_message(int sockfd);

void print_message(Message* message);

#endif	/* MESSAGE_H */

