/*
 * sensormodul_atmel.c
 *
 * Created: 2023-10-30 14:38:10
 * Author : thewe344
 */ 

#include "main.h"
#include "init.h"
//#include "odometer.h"
#include "sensor_slave.h"

volatile uint8_t leftWheelDistance = 2;
volatile uint8_t rightWheelDistance =5;

volatile uint8_t MCUSR_data = 0;  
volatile float adcData = 0.0;
volatile uint8_t cntr = 0;
volatile uint32_t sensorI2CPack = 0;  //This is a slave variable and will be sent to master


void packData(volatile uint8_t cntr, volatile float adcData, volatile uint8_t leftWheelDistance, volatile uint8_t rightWheelDistance){
	sensor_data_buffer[0] = 6; //cntr;
	sensor_data_buffer[1] = 4;//(uint8_t)adcData;
	sensor_data_buffer[2] = 5;//leftWheelDistance;
	sensor_data_buffer[3] = 8;//rightWheelDistance;
	
	
	//return tempPack;
}


int main () {
	MCUSR_data |= MCUSR;
	DDRB = 0XFF;
	init();
	while(1){
		//IF no conversion is active check.
				//	PORTB = 0;
					//PORTB = 10;
				//	PORTB = 0;
		if((ADCSRA & (1 << ADSC)) == 0){
			switch (cntr){
				case 1:
					ADC_port_enable_and_convert(IR1_30_INPUT_PIN);//Left IR sensor 
					cntr += 1; 
					break;
				case 2:
					ADC_port_enable_and_convert(IR2_30_INPUT_PIN); // Ir sensor forward
					cntr += 1; 
					break;
				case 3:
					ADC_port_enable_and_convert(IR_80_INPUT_PIN); // Right IR sensor
					cntr = 1; 
					break;
				default:
					cntr = 1;
					break;
			}
		}
		//sensor_data_buffer = {0x12,0x13,0x14,0x14};
		packData(cntr, adcData, leftWheelDistance, rightWheelDistance);
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
	volatile uint16_t adcValue = 0;
	//Read ADC result
	adcValue = ADCL;
	adcValue |= (ADCH << 8);
	//Handle the ADC result as needed
	adcData = (adcValue * 5.0)/1024.0;
	switch (cntr){
		case 1: // If one of the two 30cm IR sensors selceted
			adcData = (adcData / 0.6) * 0.05;
			adcData = 1 / adcData;
			adcData = adcData - 0.25 ;
			break;
		case 2: // If one of the two 30cm IR sensors selceted
			adcData = (adcData / 0.6) * 0.05;
			adcData = 1 / adcData;
			adcData = adcData - 0.25 ;
			break;
		case 3: // If sensor IR sensor 80cm selected
			adcData = (adcData / 0.6) * 0.05;
			adcData = 2 / adcData;
			adcData = adcData - 0.25 ;
			break;
	}
}