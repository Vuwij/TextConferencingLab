#ifndef MESSAGE_H
#define	MESSAGE_H

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

const char* serialize_message(Message* message);

Message deserialize_message(char* buf, int size);

#endif	/* MESSAGE_H */

