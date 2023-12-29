#include "init.h"
#include "sensor_slave.h"

void init(){
	wdt_disable();
	initilize_ports();
	init_ADC();
	//interupt_init();
	init_Sensor_Slave();
	//sensor_index=0;
	//received_index=0;
	//timer_setup();
	sei();
	lastADCReflex1 = STATE_FIRST_RUN;
	lastADCReflex2 = STATE_FIRST_RUN;
}

void initilize_ports(){
	// Initilize the input pins to IR sensors and reflex sensors
	DDRA &= ~(1 << IR1_30_INPUT_PIN | 1 << IR2_30_INPUT_PIN |1 << IR_80_INPUT_PIN);
	
	//------------------------------------------------------------
	//Initilize digital pins for reflex sensor
	DDRA &= ~(1 << REFLEX1_INPUT_PIN) | (1 << REFLEX2_INPUT_PIN);
	// Enable pull-up resistor for above
	PORTA |= (1 << REFLEX1_INPUT_PIN) | (1 << REFLEX2_INPUT_PIN);
	//------------------------------------------------------------
	
	// Add Unused pins
	//IF more pins need to be activated
}

// Initializing for ADC
void init_ADC(){
	// Set 5V internal ref
	ADMUX &= ~(1<< REFS1);
	ADMUX |= (1<< REFS0);
	// Set Result left adjusted -- changed to right
	ADMUX &= ~(1 << ADLAR);
	// Activate ADC
	ADCSRA |= (1 << ADEN);
	//Disable auto-trigger
	ADCSRA &= ~(1<<ADATE);
	//Enable interupt from ADC,
	ADCSRA |= (1 << ADIE);
	//Set ADC frequency division factor
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);
	
	// ADSC -set to one for start conversion, ADSC will read as high as long as conversion is active
}
