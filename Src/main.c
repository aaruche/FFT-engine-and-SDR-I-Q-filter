#include "stm32l4xx.h"
#include "uart.h"
#include <stdio.h>
#include "arm_math.h"
#include "systick.h"
#include <math.h>

#include "fft.h" // This now holds your struct definition!

#define FFT_SIZE 256
#define PI 3.14159265359f

// 1. Instantiate a SINGLE array of our new Struct
complex_t sdr_buffer[FFT_SIZE];

// Prototypes for main.c only
static void fpu_enable(void);
void prep_fft_data(complex_t *buffer);
void verify_signal_accuracy(complex_t *buffer);

uint32_t g_before, g_after, g_time_taken;

int main()
{
    // Hardware Init
    fpu_enable();
    uart_tx_init();
//    systick_counter_init();

    // 1. Generate the test data into our struct array
    prep_fft_data(sdr_buffer);

    // 2. Run the entire DSP pipeline with ONE function call
//    g_before = SysTick->VAL;

    process_sdr_signal(sdr_buffer, FFT_SIZE);

//    g_after = SysTick->VAL;
//    g_time_taken = g_before - g_after;

    // 3. Verify the output
    verify_signal_accuracy(sdr_buffer);

    while(1)
    {
        // System loop
    }
}

// --- Data Generation (Updated to use the struct pointer) ---
void prep_fft_data(complex_t *buffer)
{
    for (int i = 0; i < FFT_SIZE; i++)
    {
        buffer[i].real = cosf(2.0f * PI * 5.0f * (float)i / (float)FFT_SIZE);
        buffer[i].imag = sinf(2.0f * PI * 5.0f * (float)i / (float)FFT_SIZE);
    }
}

// --- Verification (Updated to use the struct pointer) ---
void verify_signal_accuracy(complex_t *buffer)
{
    printf("Index | Orig I (Real) | Recv I | Orig Q (Imag) | Recv Q \r\n");
    printf("----------------------------------------------------------\r\n");
    for (int i = 0; i < 20; i++)
    {
        float orig_R = cosf(2.0f * PI * 5.0f * (float)i / (float)FFT_SIZE);
        float orig_I = sinf(2.0f * PI * 5.0f * (float)i / (float)FFT_SIZE);

        float recv_R = buffer[i].real;
        float recv_I = buffer[i].imag;

        printf("%d | %f | %f | %f | %f \r\n", i, orig_R, recv_R, orig_I, recv_I);
    }
}

static void fpu_enable(void)
{
    SCB->CPACR |= (1U << 20);
    SCB->CPACR |= (1U << 21);
    SCB->CPACR |= (1U << 22);
    SCB->CPACR |= (1U << 23);
}
