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

volatile uint8_t pinTracker = 0;

void packData(volatile uint8_t leftCountMarks, volatile uint8_t rightCountMarks){ //volatile uint8_t ircntr, volatile float adcData, 
	
	sensor_data_buffer[0] = leftCountMarks;// highByteLeftDist;
	sensor_data_buffer[1] = rightCountMarks; //lowByteLeftDist;
}


#include <avr/io.h>

#define THRESHOLD 512 // Example threshold for high/low detection

volatile uint8_t count_PA6 = 0; // Counter for odometer on PA6
volatile uint8_t count_PA7 = 0; // Counter for odometer on PA7
uint8_t last_state_PA6 = 0;      // Last state for PA6
uint8_t last_state_PA7 = 0;      // Last state for PA7
uint8_t first_read_PA6 = 1;      // Flag for the first read for PA6
uint8_t first_read_PA7 = 1;

void ADC_init() {
	// Set reference voltage and prescaler
	ADMUX = (1 << REFS0); // AVcc as reference
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Enable ADC and set prescaler
}

uint16_t ADC_read(uint8_t channel) {
	ADMUX = (ADMUX & 0xF8) | (channel & 0x07); // Select ADC channel
	ADCSRA |= (1 << ADSC); // Start conversion
	while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
	return ADC;
}

void process_odometer(uint8_t channel, volatile uint8_t *count, uint8_t *last_state, uint8_t *first_read) {
	uint16_t adc_value = ADC_read(channel);
	uint8_t current_state = (adc_value > THRESHOLD) ? 1 : 0;

	if (*first_read) {
		*first_read = 0; // Clear the first read flag after the first read
		} else if (*last_state != current_state) {
		*count += 1; // Increment counter on state change
	}
	
	*last_state = current_state;
}

int main(void) {
	 // Initialize ADC
	MCUSR_data |= MCUSR;
	DDRB = 0XFF;

	init();
	ADC_init();
	while (1) {
		// Process each odometer
		process_odometer(6, &count_PA6, &last_state_PA6, &first_read_PA6); // PA6
		process_odometer(7, &count_PA7, &last_state_PA7, &first_read_PA7); // PA7
		packData(count_PA6, count_PA7);
		// Add delay if necessary
	}
}

void reset_distance_buff(){
	count_PA6 = 0; // Counter for odometer on PA6
	count_PA7 = 0;
}
