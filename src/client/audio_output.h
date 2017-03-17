#ifndef AUDIO_INPUT_H
#define AUDIO_INPUT_H

#define AUDIO_OUTPUT_N "default"
#define CHUNK_PER_BUFFER 2
#define BUFFER_CHUNK 1024
#define BUFFER_LEN CHUNK_PER_BUFFER * BUFFER_CHUNK
#define SAMPLE_RATE 44100
#define CHANNEL_COUNT 2

#define MIN_AVAILABLE_COUNT 4096

extern short buf[BUFFER_LEN];

void setup_playback();

void send_buffer_to_output();

void end_playback();

#endif /* AUDIO_INPUT_H */

