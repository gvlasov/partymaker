
// djent.c
#include <alsa/asoundlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SAMPLE_RATE 48000
#define FORMAT SND_PCM_FORMAT_FLOAT_LE
#define CHANNELS 1

void generate_djent_attack(float *buffer, int frames, float freq, float attack_time_sec) {
    float drive = 4.0f; // сатурация

    for (int i = 0; i < frames; i++) {
        float t = (float)i / SAMPLE_RATE;
        float env = expf(-5.0f * t / attack_time_sec); // экспоненциальное затухание
        float base = sinf(2.0f * M_PI * freq * t);
        float click = (t < 0.002f) ? 0.8f : 0.0f; // атака — короткий щелчок
        float combined = click + env * base;
        buffer[i] = tanhf(combined * drive); // сатурация
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <frequency Hz> <attack length ms> <interval ms>\n", argv[0]);
        return 1;
    }

    float freq = atof(argv[1]);
    int attack_ms = atoi(argv[2]);
    int interval_ms = atoi(argv[3]);

    int attack_frames = SAMPLE_RATE * attack_ms / 1000;
    int total_frames = SAMPLE_RATE * interval_ms / 1000;

    if (attack_frames > total_frames) {
        fprintf(stderr, "Attack length must be ≤ interval.\n");
        return 1;
    }

    float *buffer = calloc(total_frames, sizeof(float));
    if (!buffer) {
        perror("calloc");
        return 1;
    }

    generate_djent_attack(buffer, attack_frames, freq, attack_ms / 1000.0f);
    // остальной буфер — тишина

    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;

    if (snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        perror("snd_pcm_open");
        free(buffer);
        return 1;
    }

    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, FORMAT);
    snd_pcm_hw_params_set_channels(handle, params, CHANNELS);
    snd_pcm_hw_params_set_rate(handle, params, SAMPLE_RATE, 0);
    snd_pcm_hw_params(handle, params);
    snd_pcm_hw_params_free(params);
    snd_pcm_prepare(handle);

    while (1) {
        snd_pcm_writei(handle, buffer, total_frames);
    }

    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    free(buffer);
    return 0;
}
