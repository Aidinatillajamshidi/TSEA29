/*
 * sensormodul_atmel.c
 *
 * Created: 2023-10-30 14:38:10
 * Author : thewe344
 */ 

#include "main.h"
#include "init.h"
#include "ir_module.h"

volatile uint8_t MCUSR_data = 0;  
volatile float adcData = 0.0; //This is a slave variable and will be sent to master
volatile int whatSensor = 0; //This will be updated from MASTER
volatile int leftWheelRotations = 0; //This will be updated by the reflex sensors
volatile int rightWheelRotations = 0; //This will be updated by the reflex sensors
volatile float totalCmRight = 0.0; // How many cm right side has moved, based on right reflex sensor
volatile float totalCmLeft = 0.0; // How many cm left side has moved, based on left reflex sensor
volatile uint16_t adcValue = 0;
float PI = 3.141592653589793238462643;

int main () {
	MCUSR_data |= MCUSR;
	init();
	while(1){
		//IF no conversion is active check.
		if((ADCSRA & (1 << ADSC)) == 0){
			switch (whatSensor){
				case 1:
					ADC_port_enable_and_convert(IR1_30_INPUT_PIN);//Left IR sensor 
					break;
				case 2:
					ADC_port_enable_and_convert(IR2_30_INPUT_PIN); // Ir sensor forward
					break;
				case 3:
					ADC_port_enable_and_convert(IR_80_INPUT_PIN); // Right IR sensor
					break;
				case 4:
					ADC_port_enable_and_convert(REFLEX1_INPUT_PIN);// Reflex sensor 1 
					break;
				case 5:
					ADC_port_enable_and_convert(REFLEX2_INPUT_PIN); // Reflex sensor 2
					break;
				default:
					break;
			}
		}
	}
}

ISR(BADISR_vect) { 
	while(1); 
}


//This reads selected IR sensor from the pins
void ADC_port_enable_and_convert(volatile uint8_t input_pin){
	//Set input channel for which IR sensor to ADMUX
	ADMUX |= (ADMUX & 0xF8) | (input_pin & 0x07); //First expression stores values set on the high bits, second value select channel where result will come
	// Start ADC conversion
	ADCSRA |= (1 << ADSC);
}


//To read input from ADC, IR sensors or reflex sensors
ISR(ADC_vect) {
	//Read ADC result
	adcValue = ADCL;
	adcValue |= (ADCH << 8);

	//Handle the ADC result as needed
	adcData = (adcValue * 5.0)/1024.0;
	switch (whatSensor){
		case 1: // If one of the two 30cm IR sensors selceted (must write of left or right)
			adcData = (adcData / 0.6) * 0.05;
			adcData = 1 / adcData;
			adcData = adcData - 0.25 ;
		case 2: // If one of the two 30cm IR sensors selceted (must write of left or right)
			adcData = (adcData / 0.6) * 0.05;
			adcData = 1 / adcData;
			adcData = adcData - 0.25 ;
			break;
		case 3: // If IR sensor 80cm selected
			adcData = (adcData / 0.6) * 0.05;
			adcData = 2 / adcData;
			adcData = (adcData - 0.25) * 2;
			break;
		case 4: // If Reflex sensor 1 selected
			if (adcData < 2.5)
			{
				leftWheelRotations++;
			}
			totalCmLeft = leftWheelRotations * 0.62 * PI;
			break;
		case 5: // If Reflex sensor 2 selected
			if (adcData < 2.5)
			{
				rightWheelRotations++;
			}
			totalCmRight = rightWheelRotations * 0.62 * PI;
			break;
	}
}


