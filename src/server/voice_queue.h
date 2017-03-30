#ifndef VOICE_QUEUE_H
#define VOICE_QUEUE_H

#include "../audio_packet.h"

typedef struct voice_queue {
    AudioPacket* p;
    struct voice_queue* next;
} VoicePacketQueue;

#endif /* VOICE_QUEUE_H */

