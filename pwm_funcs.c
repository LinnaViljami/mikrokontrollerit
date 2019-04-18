/*
 * MikrokontrolleritHarkka.c
 *
 * Created: 3.4.2019 14.27.10
 * Author : linnav
 */ 

#include <stdbool.h>

//5 step pwm driver
#define TABLESIZE 6
uint8_t table[TABLESIZE] = {0, 51, 102, 153, 204, 255};
uint8_t table_index = 5;

unsigned char currentPwmVoltage = 5;
int16_t target_pwm_value = INT16_MAX;




void init_pwm()
{
	TCCR0B |= 0b11;			// prescaler, mitä isompi luku, sen pienempi taajuus. Asetetaan 64
	TCCR0A |= (1<<COM0A1);	// clear IO pin on match, set at BOTTOM
	TCCR0A |= (1<<WGM00) | (1<<WGM01);	// Fast PWM mode WGM[2:0] = 0b011 laitetaan fast pwm pääälle asettamalla WGM00
	// ja WGM01 1:ksi
	
	DDRD |= (1<<PD6);		// enable output buffer of PIN 6 on port D
	OCR0A = 0xff;	// initial value for signal asetetaan pulssisuhteen arvo.
	currentPwmVoltage = '5';
	//kun 255 suhde on 100% kun 0 niin 0%
}

bool set_pwm_voltage(unsigned char voltage)
{
	if(voltage == '0'){
		table_index = 0;
		currentPwmVoltage = '0';
	}
	else if(voltage == '1'){
		table_index = 1;
		currentPwmVoltage = '1';
	}
	else if(voltage == '2'){
		table_index = 2;
		currentPwmVoltage = '2';
	}
	else if(voltage == '3'){
		table_index = 3;
		currentPwmVoltage = '3';
	}
	else if(voltage == '4'){
		table_index = 4;
		currentPwmVoltage = '4';
	}
	else if(voltage == '5'){
		table_index = 5;
		currentPwmVoltage = '5';
	}
	else{
		return false;
	}
	target_pwm_value = INT16_MAX/(TABLESIZE-1);
	target_pwm_value = target_pwm_value*table_index;
	set_pwm_register_value(target_pwm_value);
	return true;
}

void set_pwm_register_value(int16_t target_value){
	uint8_t converted_value = (target_value >> 7);
	OCR0A = converted_value;
}

unsigned char get_current_pwm_voltage(){
	
	return currentPwmVoltage;
}

int16_t get_pwm_target_value(){
	return target_pwm_value;
}

void calculate_new_pwm(int16_t error_term){
	uint8_t old_pwm_value = OCR0A;
	int32_t new_value = old_pwm_value;
	new_value = (new_value << 7);
	new_value = new_value + error_term;
	if(new_value > INT16_MAX){
		set_pwm_register_value(INT16_MAX);
	}
	else if(new_value < 0){
		set_pwm_register_value(0);
	}
	else{
		int16_t scaled_new_value = new_value;
		set_pwm_register_value(scaled_new_value);
	}
}

uint8_t get_pwm_register_value(){
	return OCR0A;
}