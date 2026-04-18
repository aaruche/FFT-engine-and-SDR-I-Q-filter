#include "fft.h"
#include <math.h>

#define PI 3.14159265359f

// ---1. MASTER FUNCTION ---
void process_sdr_signal(complex_t *buffer, uint32_t N)
{
    // Step 1: Time -> Frequency
    calculate_fft(buffer, N);

    // Step 2: Destroy the noise
    apply_frequency_filter(buffer, N);

    // Step 3: Frequency -> Time
    calculate_ifft(buffer, N);
}

// --- 2. THE FILTER ---
void apply_frequency_filter(complex_t *buffer, uint32_t N)
{
    // Bandpass filter: Keep bins 2 to 8. (Bin 5 is safely inside!)
    for (int i = 0; i < N; i++)
    {
        if (i < 2 || i > 8)
        {
            buffer[i].real = 0.0f;
            buffer[i].imag = 0.0f;
        }
    }
}

// --- 3. THE INVERSE FFT ---
void calculate_ifft(complex_t *buffer, uint32_t N)
{
    // 1. Conjugate the input (Flip the sign of the Imaginary parts)
    for (int i = 0; i < N; i++) {
        buffer[i].imag = -buffer[i].imag;
    }

    // 2. Run Forward FFT
    calculate_fft(buffer, N);

    // 3. Conjugate again and scale the volume down by N
    for (int i = 0; i < N; i++) {
        buffer[i].real = buffer[i].real / N;
        buffer[i].imag = -buffer[i].imag / N;
    }
}


// --- 4. THE FORWARD FFT ENGINE ---
void calculate_fft(complex_t *buffer, uint32_t N)
{
    uint32_t num_stages = 0;
    uint32_t temp_N = N;

    // 1.1  figure out how many bits to reverse; ' log2(N) in C '
    // Do this ONCE outside the main loop
    while (temp_N > 1)
    {
        num_stages++;
        temp_N = temp_N >> 1; // shifting right by 1 is basically div by 2
    }

    for (int i = 0; i < N; i++)
    {
        uint32_t temp_i = i;     // Clone i BEFORE the engine starts
        uint32_t reversed_i = 0; // Empty bin MUST start at 0 for each new 'i'

        // BIT REVERSAL ALG
        for (int it = 0; it < num_stages; it++)
        {
            // Step 2: Make room in the output bin by shifting left
            reversed_i = (reversed_i << 1);

            // Step 1 & 3: Extract the right-most bit of temp_i and drop it into reversed_i
            reversed_i = reversed_i | (temp_i & 1);

            // Step 4: Shift the clone right so the next bit is ready
            temp_i = temp_i >> 1;
        }

        // SWAPPING THE BITS
//        if ( i < reversed_i )
//        {
//        	float32_t temp = signal_array[i]; // Dereference : is  baically *(signal_array + i)
//        	signal_array[i] = signal_array[reversed_i];
//        	signal_array[reversed_i] = temp;
//        }

        // SWAPPING THE BITS (Now doing it directly on the buffer struct!)
        if ( i < reversed_i )
        {
            // Swap Real parts
            float32_t temp_R = buffer[i].real;
            buffer[i].real = buffer[reversed_i].real;
            buffer[reversed_i].real = temp_R;

            // Swap Imaginary parts (To keep them synchronized)
            float32_t temp_I = buffer[i].imag;
            buffer[i].imag = buffer[reversed_i].imag;
            buffer[reversed_i].imag = temp_I;
        }
    }

    //--- Twiddle Factors and Butterfly MatH ---
    for (int i = 1; i <= num_stages; i ++ )
    {
        int num_elements; 		// no. of elements in each group at each stages
        int dist_elements;		// dist btw each elements in a group @ each stage (basically half of num_elements)
        num_elements = 1 << i ;
        dist_elements = num_elements >> 1;

        for (int j = 0; j < N  ; j+= num_elements  )
        {
            for (int k = j; k < j + dist_elements ; k ++  )
            {
                int bot_index = k + num_elements/2; 			// bot index is the second competitor; while first competitor is index k
                float32_t theta = ((-PI) * (k-j)) / dist_elements ;

                // BUTTERFLY MATH
                float32_t W_R; //  Twiddle Factors : cosine
                float32_t W_I; //  Twiddle Factors : real (sine)

                W_R = cosf(theta);
                W_I = sinf(theta);

                float32_t temp_R;
                float32_t temp_I;
                // (A + iB) x (C + iD)
                temp_R = (buffer[bot_index].real * W_R) - (buffer[bot_index].imag * W_I);
                temp_I = (buffer[bot_index].real * W_I) + (buffer[bot_index].imag * W_R);

                // Now you have the Top competitor (buffer[k]), and the newly spun Bottom competitor (temp_R and temp_I).
                // The rule of the butterfly is:
//				The New Top = Old Top + Temp
//				The New Bottom = Old Top - Temp

                // The Cross (The Butterfly)
                // CRITICAL We calculate the New Bottom FIRST!
                // If we updated buffer[k].real first, the original Top value would be destroyed!
                buffer[bot_index].real = buffer[k].real - temp_R;
                buffer[bot_index].imag = buffer[k].imag - temp_I;

                // safe to overwrite the Top competitor
                buffer[k].real = buffer[k].real + temp_R;
                buffer[k].imag = buffer[k].imag + temp_I;
            }
        }
    }
}
