/*
 * MikrokontrolleritHarkka.c
 *
 * Created: 3.4.2019 14.27.10
 * Author : linnav
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/cpufunc.h>
#include <avr/interrupt.h>

#define FOSC 16000000	// Clock Speed
#define MYUBRR FOSC/16/BAUD-1	// USART Baud Rate Register, equation from datasheetFDDRC

void USART_Init( unsigned int baud)
{
	uint16_t ubrr;
	ubrr = FOSC/16/baud-1;
	/*Set baud rate, 16 bit register */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0) | (1 << UCSZ00) | (1 << UCSZ01); // 2 stop bits, 8 bit data
	UCSR0B |= (1 << RXCIE0); //enable interrupts
	
}

// polling, not good, should be done with an interrupt
unsigned char USART_Receive( void )
{
	uint8_t a;
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) )
	a++;
	/* Get and return received data from buffer */
	return UDR0;
}

void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) )
	;
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void transmit_word();