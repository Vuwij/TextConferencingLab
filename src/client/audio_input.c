#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <sys/socket.h>
#include <pthread.h>

#include "audio_input.h"
#include "../constants.h"
#include "status.h"
#include "../audio_packet.h"

snd_pcm_t *capture_handle;
AudioPacket audiopacket;

void setup_capture() {
    PRINT("Starting Capture\n");

    int i;
    int err;

    snd_pcm_hw_params_t *hw_params;

    if ((err = snd_pcm_open(&capture_handle, AUDIO_INPUT_N, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf(stderr, "cannot open audio device %s (%s)\n",
                AUDIO_INPUT_N,
                snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
        fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0) {
        fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf(stderr, "cannot set access type (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
        fprintf(stderr, "cannot set sample format (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    int srate = SAMPLE_RATE;
    if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &srate, 0)) < 0) {
        fprintf(stderr, "cannot set sample rate (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, 2)) < 0) {
        fprintf(stderr, "cannot set channel count (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0) {
        fprintf(stderr, "cannot set parameters (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    snd_pcm_hw_params_free(hw_params);

    if ((err = snd_pcm_prepare(capture_handle)) < 0) {
        fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
                snd_strerror(err));
        exit(1);
    }

}

pthread_t capture_t;
bool stop_capture = false;

void *capture(void *t) {
    int err, numbytes;
    stop_capture = false;
    while (1) {
        if ((err = snd_pcm_readi(capture_handle, &audiopacket.data, AUDIO_PACKET_S)) != AUDIO_PACKET_S) {
            fprintf(stderr, "read from audio interface failed (%s)\n",
                    snd_strerror(err));
            exit(1);
        }

        if ((numbytes = sendto(status.voicefd, &audiopacket.data, AUDIO_PACKET_S, 0,
                status.udp->ai_addr, status.udp->ai_addrlen)) == -1) {
            perror("client: sendto");
            exit(1);
        }
        
        // Update the time
        gettimeofday(&audiopacket.time, NULL);
        
        if (stop_capture) {
            snd_pcm_close(capture_handle);
            pthread_exit(NULL);
        }
    }
}

void open_capture() {
    long t = 1;
    stop_capture = false;
    
    // Define the source of the sender
    strcpy(audiopacket.source, status.client_id);
    
    int rc = pthread_create(&capture_t, NULL, capture, (void *) t);
    if (rc) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }
}

void close_capture() {
    stop_capture = true;
}