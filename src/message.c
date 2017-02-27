#include <stdio.h>
#include <stdlib.h>

#include "message.h"
#include "constants.h"
#include "strings.h"
#include "string.h"

#define INT_STRING_LEN 10

const char* serialize_message(Message* message) {
    int message_t_size = INT_STRING_LEN;
    int message_s_size = INT_STRING_LEN;
    int message_src_size = sizeof(message->source); 
    int message_data_size = message->size;
    
    char message_t_str[message_t_size];
    sprintf(message_t_str, "%d", (int) message->type);

    char size_str[message_s_size];
    sprintf(size_str, "%d", message->size);
    
    char* packet = (char *) malloc(message_t_size + message_s_size + message_src_size + message_data_size);
    
    int i = 0;
    memcpy(&packet[i], message_t_str, message_t_size);
    i += message_t_size;
    memcpy(&packet[i], size_str, message_s_size);
    i += message_s_size;
    memcpy(&packet[i], message->source, message_src_size);
    i += message_src_size;
    memcpy(&packet[i], message->data, message_data_size);
    
    return packet;
}

Message deserialize_message(char* buf, int size) {
    Message m;
    char str[MAXBUFSIZE] = "";
    
    int message_t_size = INT_STRING_LEN;
    int message_s_size = INT_STRING_LEN;
    int message_src_size = sizeof(m.source);
    
    int i = 0;
    memcpy(str, buf + i, message_t_size);
    i += message_t_size;
    m.type = (control_packet_t) atoi(str);
    
    memcpy(str, buf + i, message_s_size);
    i += message_s_size;
    m.size = atoi(str);
    
    memcpy(m.source, buf + i, message_src_size);
    i += message_src_size;
    
    memcpy(m.data, buf + i, m.size);
    
    return m;
}