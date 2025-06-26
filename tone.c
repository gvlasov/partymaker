
// tone.c
#include <alsa/asoundlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SAMPLE_RATE 44100
#define NUM_CHANNELS 1
#define FORMAT SND_PCM_FORMAT_FLOAT_LE
#define BUFFER_FRAMES 1024
#define MAX_TONES 64

typedef struct {
    float frequency;
    float gain;
    double phase;
} Tone;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <freq1[xgain]> [<freq2[xgain]> ...]\n", argv[0]);
        return 1;
    }

    Tone tones[MAX_TONES];
    int num_tones = argc - 1;
    if (num_tones > MAX_TONES) {
        fprintf(stderr, "Max %d tones supported\n", MAX_TONES);
        return 1;
    }

    for (int i = 0; i < num_tones; i++) {
        char *arg = argv[i + 1];
        char *xptr = strchr(arg, 'x');
        if (xptr) {
            *xptr = '\0';
            tones[i].frequency = atof(arg);
            tones[i].gain = atof(xptr + 1);
        } else {
            tones[i].frequency = atof(arg);
            tones[i].gain = 1.0f;
        }
        tones[i].phase = 0.0;
    }

    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;

    if (snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        perror("snd_pcm_open");
        return 1;
    }

    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, FORMAT);
    snd_pcm_hw_params_set_channels(handle, params, NUM_CHANNELS);
    snd_pcm_hw_params_set_rate(handle, params, SAMPLE_RATE, 0);
    snd_pcm_hw_params(handle, params);
    snd_pcm_hw_params_free(params);
    snd_pcm_prepare(handle);

    float buffer[BUFFER_FRAMES];

    while (1) {
        for (int i = 0; i < BUFFER_FRAMES; i++) {
            float sample = 0.0f;
            for (int j = 0; j < num_tones; j++) {
                sample += tones[j].gain * sinf(2.0f * M_PI * tones[j].phase);
                tones[j].phase += tones[j].frequency / SAMPLE_RATE;
                if (tones[j].phase >= 1.0f)
                    tones[j].phase -= 1.0f;
            }
            buffer[i] = sample;
        }

        snd_pcm_writei(handle, buffer, BUFFER_FRAMES);
    }

    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    return 0;
}
