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
#include "usart_funcs.c"
#include "pwm_funcs.c"
#include "adc_funcs.c"
#include "pid_funcs.c"
#include <stdbool.h>

#define FOSC 16000000	// Clock Speed
#define MYUBRR FOSC/16/BAUD-1	// USART Baud Rate Register, equation from datasheet
#define BUTTON_STATE _BV(PB0)     // = (1 << PB0)
#define BUTTON_STATE_DDR DDRB
#define BUTTON_STATE_PORT PORTB
#define LED_DDR DDRB
#define LED_PORT PORTB
#define BUTTON_LED _BV(PB1)     // = (1 << PB1)

uint8_t buttonState = 0;		// state of D0
uint8_t buttonPreviousState = 0;
uint8_t isPauseOn = 0;

pidData_t pwm_pid;

//KYSYMYKSIÄ!!!!
//kysymyksiä miten header tiedostot saa tähän atmeliin
// miten muunnat lukuja eri tyyppeihin
//miten tarkistat onko joku bitti 0 tai ei

volatile unsigned char pwmVoltageBeforePause = '5';

int main (void) {
	init_pwm();
	USART_Init(9600);
	BUTTON_state_pin_init();
	BUTTON_led_init();
	BUTTON_led_off();
	pid_Init(40,10,0,&pwm_pid);
	
	uint8_t ADCvalue;
	initADC();
	sei();
	
	while (1) {
		ADCvalue=readADC8();
		int16_t converted_ADC_value = ADCvalue;
		converted_ADC_value = (converted_ADC_value<<7);
		int16_t pwm_error_value = pid_Controller(get_pwm_target_value(),converted_ADC_value,&pwm_pid);
		calculate_new_pwm(pwm_error_value);
		_delay_ms(2900);
		uint8_t a;
		uint8_t pwm_value = get_pwm_register_value();
		for(a = 0; a<8; a++){
			if((ADCvalue & (0x01 << (7-a))) == 0x00){
				USART_Transmit('0');
			}
			else{	
				USART_Transmit('1');
			}
		}
		USART_Transmit(0x09);
	}
}

void BUTTON_led_init(){
	LED_DDR = BUTTON_LED;   // Set the LED pin as an output
	LED_PORT &= (~BUTTON_LED);
	
	
}

void BUTTON_led_on(){
	LED_PORT |= BUTTON_LED;
}

void BUTTON_led_off(){
	LED_PORT &= (~BUTTON_LED);
}

void BUTTON_state_pin_init(){
	BUTTON_STATE_DDR &= (~BUTTON_STATE); //DATA DIRECTION TO INPUT
	BUTTON_STATE_PORT &= (~BUTTON_STATE); // enable pullup (??)
	PCICR |= (1 << PCIE0); //enable interrupts to this pin
	PCMSK0 |= (1 << PCINT0); 
	
}



ISR(USART_RX_vect)
{
	unsigned char ReceivedByte;
	ReceivedByte = UDR0;
	UCSR0A & (~RXC0); // signal that data is received
	if(set_pwm_voltage(ReceivedByte)){
		USART_Transmit(ReceivedByte); // echo back to the computer
		USART_Transmit('!');
		pid_Reset_Integrator(&pwm_pid);
	}
}

ISR(PCINT0_vect)							// Pin change interrupt flag register PCIFR (0x3b)
{
	buttonState = (PINB & BUTTON_STATE);
	//do the stuff
	if(buttonState ==1 && buttonPreviousState==0){	//when pressed	
		if(isPauseOn == 0){
			isPauseOn++;
			pwmVoltageBeforePause = get_current_pwm_voltage();
			BUTTON_led_on();
			set_pwm_voltage('0');
		}else{
			isPauseOn = 0;
			BUTTON_led_off();
			set_pwm_voltage(pwmVoltageBeforePause);	
			
		}
	}
	buttonPreviousState = buttonState;
	
	/*
	buttonInput = PINB & BUTTON_STATE;		// read port D pin 0 (mask all other bits)
	if(buttonPreviousState==0) {					// if previous state was 0 then the change must be a rising edge
		// action
		buttonPinInput = PINB;					// read port D to the buffer variable
		buttonEdgeCounter++;						// count rising edges
		BUTTON_led_on();
	}
	else{
		BUTTON_led_off();
	}
	buttonPreviousState=buttonInput;
	*/
	
}