#ifndef AUDIO_OUTPUT_H
#define AUDIO_OUTPUT_H

#include "../audio_packet.h"

#define AUDIO_OUTPUT_N "default"
#define BUFFER_LEN PACKET_SIZE
#define MIN_AVAILABLE_COUNT PACKET_SIZE

typedef struct audio_packet AudioPacket;
extern AudioPacket outpacket;

void setup_playback();

void send_buffer_to_output();

void end_playback();

#endif /* AUDIO_OUTPUT_H */

