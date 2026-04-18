#include "systick.h"
#include "core_cm4.h" // Provides the SysTick structure and bit masks

/* * The clock frequency should match your system.
 * Since your board defaults to 4MHz, use 4000000.
 */
#define clk    4000000

void systick_counter_init(void)
{
    /* 1. Disable SysTick before configuration */
    // This clears all control bits, ensuring a clean state
    SysTick->CTRL = 0;

    /* 2. Load the Reload Value Register */
    // (clk/1000) calculates the cycles needed for 1ms
    SysTick->LOAD = (clk / 1000) - 1;

    /* 3. Clear the Current Value Register */
    // Writing any value to VAL resets the counter to 0 and clears the COUNTFLAG
    SysTick->VAL = 0;

    /* 4. Select the internal clock source */
    // CLKSOURCE_Msk sets bit 2 to 1 (Processor Clock)
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

    /* 5. Enable the SysTick timer */
    // ENABLE_Msk sets bit 0 to 1
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}



/*-------------------------------------------------------------------------BARE METAL------------------------------------------------------------------------------------- */

//#include "systick.h"
//
//
//// Define the Base address
//#define SYST_BASE 			(0xE000E010UL)
//
//// Define registers using offsets correctly
//#define SYST_CSR 			(*(volatile uint32_t *)(SYST_BASE + 0x00))
//#define SYST_RVR 			(*(volatile uint32_t *)(SYST_BASE + 0x04))
//#define SYST_CVR 			(*(volatile uint32_t *)(SYST_BASE + 0x08))
//#define SYST_CALIB 			(*(volatile uint32_t *)(SYST_BASE + 0x0C))
//
//#define clk 				16000000
//
//#include "core_cm4.h"
//
//
//void systick_counter_init (void)
//{
//	// 1. Disable systick before config
//	SYST_CSR &= ~(1<<0);
//
//	// 2. load systick reload value register
//	SYST_RVR = (clk/1000)-1;
//
//	// 3. Clear the systick current value reg
//	SYST_CVR = 0;
//
//	// 4. Select internal clock source
//	SYST_CSR |= (1<<2);
//
//	// 5. Enable systick
//	SYST_CSR |= (1<<0);
//}
