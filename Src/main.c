
#include "stm32l4xx.h"
#include "signals.h"
#include "uart.h"
#include <stdio.h>

extern float _5hz_signal[HZ_5_SIG_LEN];
float g_in_sig_smple; 					// input signal sample

static void plot_input_signal(void);
static void pseudo_dly(int dly) ;


int main()
{
	//Enabling FLoating pint Unit: Enable CP10 and CP 11 -> Full Access
//	SCB->CPACR |= ((3UL << 10*2) |(3UL << 11*2) );
	// OR (Same logic)
	SCB->CPACR |= (1U << 20);
	SCB->CPACR |= (1U << 21);
	SCB->CPACR |= (1U << 22);
	SCB->CPACR |= (1U << 23);

	// Initialise the UART
	uart_tx_init();

    while(1)
    {
    	printf("Hello from stm32 ... \n\r");
//    	plot_input_signal();
    }
}

static void plot_input_signal(void)
{
	for (int i= 0 ; i< HZ_5_SIG_LEN; i++)
	{
		g_in_sig_smple = _5hz_signal[i];
		pseudo_dly(9000); 					// when we get one sample then we give a delay
	}

}

static void pseudo_dly(int dly) 			// creates an informal/inaccurate delay
{
	for (int i = 0 ; i < dly ; i ++ );
}

