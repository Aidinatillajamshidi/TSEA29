#include "init.h"


void init(){
	wdt_disable();
	initilize_ports();
	init_ADC();
	interupt_init();
	timer_setup();
}

//CTC timer for wheel measurment (16-bit timer/counter)
void timer_setup(){
	//Set timer to CTC OSR1A
	TCCR1B |= (1 << WGM12);
	// Set prescaler value 1024
	TCCR1B |= (1 << CS12) |(1 << CS10);
	// Count to one sec (16Mhz/1024/1) - 1
	OCR1A = 15624;
	//Interupt on compare match
	TIMSK1 |= (1 << OCIE1A);
}

void initilize_ports(){
	// Initilize the input pins to IR sensors and reflex sensors
	DDRA &= ~(1 << IR1_30_INPUT_PIN | 1 << IR2_30_INPUT_PIN |1 << IR_80_INPUT_PIN);
	
	//------------------------------------------------------------
	//Initilize digital pins for reflex sensor
	DDRD &= ~(1 << REFLEX1_INPUT_PIN_DIGI) | (1 << REFLEX2_INPUT_PIN_DIGI);
	// Enable pull-up resistor for above
	PORTD |= (1 << REFLEX1_INPUT_PIN_DIGI) | (1 << REFLEX2_INPUT_PIN_DIGI);
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

void interupt_init(){
	//------------------------------------------------------------
	//Testing interupt on digital pins
	//Enable rising edge on INT= and INT1, interupt on digital pins PD2 and PD3
	EICRA |= (1 << ISC00) //| (1 << ISC01); PD2 changed to any logical change
	EICRA |= (1 << ISC10) //| (1 << ISC11); PD3 changed to any logical change
	//Enable INT0 and INT1 for interupt pins
	EIMSK |= (1 << INT0) | (1 << INT1);
	//------------------------------------------------------------
	sei();
}
