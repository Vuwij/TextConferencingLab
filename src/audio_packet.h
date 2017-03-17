#ifndef AUDIO_PACKET_H
#define AUDIO_PACKET_H

#include "constants.h"
#include "client/audio_output.h"

typedef struct audio_packet {
    char source[MAXDATASIZE];
    short data[BUFFER_CHUNK];
} AudioPacket;

void send_audio_packet(AudioPacket* packet);

void recieve_audio_packet(AudioPacket* packet);

#endif /* AUDIO_PACKET_H */

