#ifndef INIT_H
#define INIT_H
#include "main.h"

#define IR1_30_INPUT_PIN PA0  // Define PA0 as input pin for IR-sensor 1 30cm
#define IR1_30_OUTPUT_PIN PA1 // Define PA1 as output pin for IR-sensor 1 30cm

#define IR2_30_INPUT_PIN PA2 	// Define PA2 as input pin for IR-sensor 2 30cm
#define IR2_30_OUTPUT_PIN PA3 	// Define PA3 as output pin for IR-sensor 2 30cm

#define IR_80_INPUT_PIN PA4  // Define PA4 as input pin  for IR-sensor 80cm
#define IR_80_OUTPUT_PIN PA5 // Define PA5 as output pin  for IR-sensor 80cm

#define REFLEX1_INPUT_PIN PA6 // Define PA6 as input pin  for reflexsensor 1
#define REFLEX2_INPUT_PIN PA7 // Define PA7 as input pin  for reflexsensor 2

//#define REFLEX1_INPUT_PIN_DIGI PD2 // Define PA6 as input pin  for reflexsensor 1
//#define REFLEX2_INPUT_PIN_DIGI PD3 // Define PA7 as input pin  for reflexsensor 2


//Initializer
void init(void);
void initilize_ports(void);
void init_ADC(void);
//void interupt_init(void);
void timer_setup(void);
	
#endif
