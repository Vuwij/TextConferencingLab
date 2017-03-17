#ifndef AUDIO_PACKET_H
#define AUDIO_PACKET_H

#define AUDIO_PACKET_LENGTH     4 * 1024

#include "constants.h"

typedef struct audio_packet {
    char source[MAXDATASIZE];
    short data[AUDIO_PACKET_LENGTH];
} AudioPacket;

#endif /* AUDIO_PACKET_H */

