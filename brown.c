
// brown.c
#include <alsa/asoundlib.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <math.h>

#define RATE 44100
#define BUFFER_SIZE 1024

int16_t brown_buffer[BUFFER_SIZE];

void generate_brown_noise(int16_t *buffer, int size, double *last_output) {
    for (int i = 0; i < size; ++i) {
        double white = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        *last_output += white * 0.02;
        if (*last_output > 1.0) *last_output = 1.0;
        if (*last_output < -1.0) *last_output = -1.0;
        buffer[i] = (int16_t)(*last_output * 32767);
    }
}

int main() {
    srand(time(NULL));

    snd_pcm_t *pcm;
    snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_set_params(pcm,
        SND_PCM_FORMAT_S16_LE,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        1, RATE, 1, 500000);

    double last_output = 0.0;

    while (1) {
        generate_brown_noise(brown_buffer, BUFFER_SIZE, &last_output);
        snd_pcm_writei(pcm, brown_buffer, BUFFER_SIZE);
    }
}
