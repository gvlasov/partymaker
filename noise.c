
// noise.c
#include <alsa/asoundlib.h>
#include <fftw3.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define RATE 44100
#define FFT_SIZE 4096
#define FREQ_MIN 20.0
#define FREQ_MAX 20000.0

double slope_db_per_octave = 0;

void generate_colored_noise(int16_t *outbuf, int size, double slope_db) {
    double input[FFT_SIZE] = {0};

    for (int i = 0; i < size; ++i)
        input[i] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;

    fftw_complex *freq = fftw_malloc(sizeof(fftw_complex) * (FFT_SIZE / 2 + 1));
    fftw_plan plan_f = fftw_plan_dft_r2c_1d(FFT_SIZE, input, freq, FFTW_ESTIMATE);
    fftw_execute(plan_f);

    for (int i = 1; i < FFT_SIZE / 2 + 1; ++i) {
        double freq_hz = (double)i * RATE / FFT_SIZE;
        if (freq_hz < FREQ_MIN || freq_hz > FREQ_MAX) {
            freq[i][0] = 0.0;
            freq[i][1] = 0.0;
            continue;
        }
        double gain = pow(10.0, (slope_db / 20.0) * log2(freq_hz / 1000.0));
        freq[i][0] *= gain;
        freq[i][1] *= gain;
    }

    fftw_plan plan_i = fftw_plan_dft_c2r_1d(FFT_SIZE, freq, input, FFTW_ESTIMATE);
    fftw_execute(plan_i);

    double sum_squares = 0.0;
    for (int i = 0; i < size; ++i)
        sum_squares += input[i] * input[i];
    double rms = sqrt(sum_squares / size);
    double gain = rms > 0.0 ? (3000.0 / rms) : 1.0;

    for (int i = 0; i < size; ++i) {
        double s = input[i] * gain;
        if (s > 32767.0) s = 32767.0;
        if (s < -32768.0) s = -32768.0;
        outbuf[i] = (int16_t)s;
    }

    fftw_destroy_plan(plan_f);
    fftw_destroy_plan(plan_i);
    fftw_free(freq);
}

int main(int argc, char *argv[]) {
    if (argc >= 2)
        slope_db_per_octave = atof(argv[1]);

    srand(time(NULL));

    snd_pcm_t *pcm;
    snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_set_params(pcm,
                       SND_PCM_FORMAT_S16_LE,
                       SND_PCM_ACCESS_RW_INTERLEAVED,
                       1, RATE, 1, 500000);

    int16_t buffer[FFT_SIZE];

    while (1) {
        generate_colored_noise(buffer, FFT_SIZE, slope_db_per_octave);
        snd_pcm_writei(pcm, buffer, FFT_SIZE);
    }
}
