#ifndef AUDIO_PACKET_H
#define AUDIO_PACKET_H

#include <sys/time.h>

#include "constants.h"

#define SAMPLE_RATE     44100       // The size of the audio packet
#define PACKET_SIZE     1024        // The size of a single packet
#define CHANNEL_COUNT   1           // The number o fchannels

typedef struct audio_packet {
    char source[MAXDATASIZE];       // The source is the sender of the packet's Name
    unsigned packet_num;            // The nth packet that is sent
    struct timeval start_time;      // The start time of audio recording
    struct timeval end_time;        // The end time for audio recording
    short data[PACKET_SIZE];        // The recorded message out of 48000, here 1024 is used, so there should be 43.06640625 packets per second sent
} AudioPacket;

void send_audio_packet(AudioPacket* packet);

void recieve_audio_packet(AudioPacket* packet);

#endif /* AUDIO_PACKET_H */

