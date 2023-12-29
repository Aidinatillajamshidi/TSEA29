#ifndef ODOMETER_H
#define ODOMETER_H
#include "main.h"


uint8_t numberOfMarks = 10; // Number of marks on wheel
float wheelDiameter = 6.2; // Diameter of wheel in cm
float timeInterval = 1.0; // Time interval to measure wheel
float PI = 3.141592653589793238462643;


volatile int reflex1Counter = 0;
volatile int reflex2Counter = 0;

// Left wheel information
volatile uint8_t leftWheelSpeed = 0; //This is a slave variable and will be sent to master
volatile uint8_t leftWheelDistance = 0; //This is a slave variable and will be sent to master

//Right wheel information
volatile uint8_t rightWheelSpeed = 0; //This is a slave variable and will be sent to master
volatile uint8_t rightWheelDistance = 0; //This is a slave variable and will be sent to master



ISR(TIMER1_COMPA_vect);
ISR(INT0_vect);
ISR(INT1_vect);

#endif