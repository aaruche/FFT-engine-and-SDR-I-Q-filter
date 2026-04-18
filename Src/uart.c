
#include "uart.h"

#define GPIOA_EN 			(1U<<0)
#define UART2_EN			(1U<<17)
#define CR1_TE				(1U<<3)
#define CR1_UE				(1U<<0)
#define SR_TXE 				(1U<<7)

#define SYS_FREQ			4000000
#define APB1_CLK 			SYS_FREQ

#define UART_BAUDRATE		115200

static void uart2_set_baudrate(uint32_t periph_clk, uint32_t baudrate);
void uart2_write(int ch);


/* Redirects characters from printf() to USART2 by sending each byte
 * through uart2_write(). This allows standard printf output to appear
 * on the UART terminal connected to the PC.
 */
int __io_putchar(int ch)
{
	uart2_write(ch);
	return ch;
}



//function for TX
void uart_tx_init(void)
{
	// *****CONFIGURE UART GPIO Pin *****
	// 1. ENABLE Clock Access to GPIO A
	RCC->AHB2ENR |= GPIOA_EN ;

	// 2. SET PA2 mode to Alternate Function Mode(GPIO MODER)
	GPIOA->MODER &= ~(1U<<4); // 				op:  0000 0010 0000
 	GPIOA->MODER |= (1U<<5);

	// 3. SET PA2 ALternate Function type to UART_TX (AF07) - found in Alternate function table
 	GPIOA->AFR[0] |= (1U<<8);//					op: 0111 0000 0000
 	GPIOA->AFR[0] |= (1U<<9);
 	GPIOA->AFR[0] |= (1U<<10);
 	GPIOA->AFR[0] &= ~(1U<<11);


	// *****CONFIGURE UART *****
	// 4. Enable Clock Access to UART2
 	RCC->APB1ENR1 |= UART2_EN;

	// 5. 3. Configure Baudrate
 	uart2_set_baudrate(APB1_CLK, UART_BAUDRATE);

	// 6. Configure Transfer Direction
 	USART2->CR1 = CR1_TE;

	// 7. ENABLE UART module
 	USART2->CR1 |= CR1_UE;




}

//Finally to TRANSMIT data through UART
void uart2_write(int ch)
{
	// Make sure transmit data Reg is empty
	while(!(USART2->ISR & SR_TXE));				//TO READ registers and specific bits of a reg
	// so we are reading the 'SR_TXE ith bit' (7th bit) in 'USART2->SR  reg' by an AND operator
	// If SRE_TXE is not 1 then they would return a zero, if so then the while loop would return 1 nd run forvever until it return a 0

	// Once empty, write to Transmit data Reg
	USART2->TDR = ch & 0xFF;
}



// 5. 1. Configure Baudrate
static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate)
{
	 return ((periph_clk + (baudrate/2U)) / baudrate);
 }

// 5. 2. Configure Baudrate
static void uart2_set_baudrate(uint32_t periph_clk, uint32_t baudrate)
{
	USART2->BRR = compute_uart_bd (periph_clk, baudrate);
}
