#ifndef MAIN_H
#define MAIN_H

#include <avr/wdt.h>
#include <avr/io.h>
#include <avr/interrupt.h> 
#include <stdio.h>

#define STATE_FIRST_RUN 3
#define STATE_HIGH 1
#define STATE_LOW 0


// Debugging variable
volatile uint8_t MCUSR_data;
volatile uint8_t lastPinState;
// adcData will store ir sensor value
volatile float adcData;
volatile uint8_t lastADCReflex1;
volatile uint8_t lastADCReflex2;
// Counter for keeping track of What IR sensor is read from
volatile uint8_t ircntr;
/*
uint8_t numberOfMarks = 10; // Number of marks on wheel
float wheelDiameter = 6.2; // Diameter of wheel in cm
float timeInterval = 1.0; // Time interval to measure wheel
float PI = 3.141592653589793238462643;
*/

volatile uint16_t reflex1Counter;
volatile uint16_t reflex2Counter;

// Left wheel information
volatile uint8_t leftWheelSpeed; //This is a slave variable and will be sent to master
volatile uint16_t leftWheelDistance; //This is a slave variable and will be sent to master

//Right wheel information
volatile uint8_t rightWheelSpeed; //This is a slave variable and will be sent to master
volatile uint16_t rightWheelDistance; //This is a slave variable and will be sent to master

//ISR(PCINT6);
//ISR(PCINT7);

//Send IR Pulse, activate adcconverter on correct port
//void adcPortEnableAndConvert();

//Handles rouge interupts
ISR(BADISR_vect);

//Handles interupts from adc, this will be used to read IR sensor value
ISR(ADC_vect);
void reset_distance_buff(void);
// Function to pack data in to 32bit
void packData(volatile uint8_t, volatile uint8_t);//, volatile uint8_t , volatile uint8_t


#endif