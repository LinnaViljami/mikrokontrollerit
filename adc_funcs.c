/*
 * MikrokontrolleritHarkka.c
 *
 * Created: 3.4.2019 14.27.10
 * Author : linnav
 */ 

#include <avr/io.h>
#include <stdint.h> 

void initADC(void)
{
	DIDR0 |= 0x01;
	
	//disable pull-ups
	PORTC &= ~(1<<PORTC0);
	
	// set as a input
	//DDRC &= ~(1<<DDC0);
	
	
	// clock prescaler 128, ADCSRA [ADPS 2:0] -> 0b111
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	
	// choose internal Vcc reference, ADMUX[REFS1:REFS0] -> 0b01
	ADMUX |= (1<<REFS0);
	
	// enable AD-converter in ADCSRA
	ADCSRA |= (1<<ADEN);
	
	// init AD channel 0
	// disable channel 0 input buffer
	//DIDR0 |= (1<<ADC4D);
	// write 1 to corresponding output pin
	//PORTC |= (1<<PORTC4);

	// choose channel 0, ADMUX[MUX3:MUX0] -> 0b0000
	// default is OK
	//ADMUX |= (1<<MUX2);
	
	
	
	
	// use only high byte ADCSRA[ADLAR] -> 1
	ADMUX |= (1<<ADLAR);

	

	// enable AD-converter in Power Reduction Register, PRR[PRADC] -> 0
	PRR &= ~(1<<PRADC);
	
}

uint8_t readADC8(void)
{
	// start conversion ADCSRA[ADSC] -> 1
	ADCSRA |= (1<<ADSC);

	// poll ADCSRA[ADSC] -> 1
	while (ADCSRA & (1<<ADSC)) ;
	
	//result = ADCH;
	
	//ADCSRA |= (1<<ADIF);
	// read the 8 low order bits
	//result = ADCW;
	// read the 2 high order bits and shift them up
	return ADCH;
}
