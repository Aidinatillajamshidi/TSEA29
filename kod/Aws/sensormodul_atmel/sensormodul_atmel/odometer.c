#include "odometer.h"

uint8_t numberOfMarks = 10; // Number of maarks on wheel
float wheelDiameter = 6.2; // Diameter of wheel in cm
float timeInterval = 1.0 // Time interval to measure wheel
float PI = 3.141592653589793238462643;

volatile int reflex1Counter = 0;
volatile int reflex2Counter = 0;

// Left wheel information
float leftWheelSpeed = 0; //This is a slave variable and will be sent to master
float leftWheelDistance = 0; //This is a slave variable and will be sent to master

//Right wheel information
float rightWheelSpeed = 0; //This is a slave variable and will be sent to master
float rightWheelDistance = 0; //This is a slave variable and will be sent to master


ISR(TIMER1_COMPA_vect){
	float leftRotations = reflex1Counter / (2.0 * numberOfMarks);
	float rightRotations = reflex2Counter / (2.0 * numberOfMarks);
	
	float rpmLeft = leftRotations * (60/timeInterval);
	float rpmRight = rightRotations * (60/timeInterval);

	leftWheelDistance = leftRotations * PI * wheelDiameter;
	rightWheelDistance = leftRotations * PI * wheelDiameter;
	
	//Reset wheel couter
	reflex1Counter = 0;
	reflex2Counter = 0;
	
	//Reset timer
	TCNT1 = 0;
}

//Interupt handler for PD2 aka reflex sensor 1
ISR(INT0_vect){

	reflex1Counter += 1;

}

//Interupt handler for PD3 aka reflex sensor 2
ISR(INT1_vect){

	reflex2Counter += 1;
	
}