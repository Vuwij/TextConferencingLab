#ifndef AUDIO_PACKET_H
#define AUDIO_PACKET_H

#include <sys/time.h>

#include "constants.h"
#include "client/audio_output.h"

typedef struct audio_packet {
    char source[MAXDATASIZE];
    struct timeval time;
    short data[BUFFER_CHUNK];   //1024
} AudioPacket;

void send_audio_packet(AudioPacket* packet);

void recieve_audio_packet(AudioPacket* packet);

#endif /* AUDIO_PACKET_H */

