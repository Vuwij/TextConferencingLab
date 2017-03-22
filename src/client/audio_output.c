#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <poll.h>
#include <alsa/asoundlib.h>
#include <math.h>

#include "audio_output.h"
#include "../constants.h"

short buf[BUFFER_LEN];
snd_pcm_t *playback_handle;

int playback_callback(snd_pcm_sframes_t nframes) {
    int err;

    printf("playback callback called with %ld frames\n", nframes);

    /* ... fill buf with data ... */

    if ((err = snd_pcm_writei(playback_handle, buf, nframes)) < 0) {
        fprintf(stderr, "write failed (%s)\n", snd_strerror(err));
    }

    return err;
}

void setup_playback() {
    PRINT("Starting Playback\n");
    
    for (int i = 0; i < BUFFER_LEN; i++)
        buf [i] = 5000; // sine wave value generation

    snd_pcm_hw_params_t *hw_params;
    snd_pcm_sw_params_t *sw_params;
    int nfds;
    int err;
    struct pollfd *pfds;
    
    /* Hardware Parameters */
    if ((err = snd_pcm_open(&playback_handle, AUDIO_OUTPUT_N, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf(stderr, "cannot open audio device %s (%s)\n",
                AUDIO_OUTPUT_N,
                snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
        fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_hw_params_any(playback_handle, hw_params)) < 0) {
        fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf(stderr, "cannot set access type (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_hw_params_set_format(playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
        fprintf(stderr, "cannot set sample format (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    int rate = SAMPLE_RATE;
    if ((err = snd_pcm_hw_params_set_rate_near(playback_handle, hw_params, &rate, 0)) < 0) {
        fprintf(stderr, "cannot set sample rate (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_hw_params_set_channels(playback_handle, hw_params, CHANNEL_COUNT)) < 0) {
        fprintf(stderr, "cannot set channel count (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_hw_params(playback_handle, hw_params)) < 0) {
        fprintf(stderr, "cannot set parameters (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    snd_pcm_hw_params_free(hw_params);
    
    /* Software Parameters */
    if ((err = snd_pcm_sw_params_malloc(&sw_params)) < 0) {
        fprintf(stderr, "cannot allocate software parameters structure (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_sw_params_current(playback_handle, sw_params)) < 0) {
        fprintf(stderr, "cannot initialize software parameters structure (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_sw_params_set_avail_min(playback_handle, sw_params, MIN_AVAILABLE_COUNT)) < 0) {
        fprintf(stderr, "cannot set minimum available count (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_sw_params_set_start_threshold(playback_handle, sw_params, 0U)) < 0) {
        fprintf(stderr, "cannot set start mode (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    if ((err = snd_pcm_sw_params(playback_handle, sw_params)) < 0) {
        fprintf(stderr, "cannot set software parameters (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    
    if ((err = snd_pcm_prepare(playback_handle)) < 0) {
        fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
                snd_strerror(err));
        exit(1);
    }
    
}

void send_buffer_to_output() {
    // Prepares the audio interface for use
    snd_pcm_sframes_t frames_to_deliver;
    
    // Wait 1 second for the playback to be ready
    int err;
    if ((err = snd_pcm_wait(playback_handle, 1000)) < 0) {
        fprintf(stderr, "poll failed (%s)\n", strerror(errno));
        end_playback();
        return;
    }
    
    /* find out how much space is available for playback data */
    if ((frames_to_deliver = snd_pcm_avail_update(playback_handle)) < 0) {
        if (frames_to_deliver == -EPIPE) {
            fprintf(stderr, "an xrun occured\n");
            end_playback();
            return;
        } else {
            fprintf(stderr, "unknown ALSA avail update return value (%d)\n", frames_to_deliver);
            end_playback();
            return;
        }
    }

    frames_to_deliver = frames_to_deliver > BUFFER_CHUNK ? BUFFER_CHUNK : frames_to_deliver;

    /* deliver the data */
    if (playback_callback(frames_to_deliver) != frames_to_deliver) {
        fprintf(stderr, "playback callback failed\n");
        end_playback();
    }
}

void end_playback() {
    snd_pcm_close(playback_handle);
}