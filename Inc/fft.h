#ifndef FFT_H
#define FFT_H

#include "arm_math.h"
#include <stdint.h>

// Define our Interleaved Complex Data Type
typedef struct {
    float32_t real;
    float32_t imag;
} complex_t;


void calculate_fft(complex_t *buffer, uint32_t N);
void calculate_ifft(complex_t *buffer, uint32_t N);
void apply_frequency_filter(complex_t *buffer, uint32_t N);

// The Master Wrapper Function
void process_sdr_signal(complex_t *buffer, uint32_t N);

#endif // FFT_H
