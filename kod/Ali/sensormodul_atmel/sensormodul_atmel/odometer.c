#include "odometer.h"


ISR(TIMER1_COMPA_vect){
	float leftRotations = reflex1Counter / (2.0 * numberOfMarks);
	float rightRotations = reflex2Counter / (2.0 * numberOfMarks);
	
	float rpmLeft = leftRotations * (60/timeInterval);
	float rpmRight = rightRotations * (60/timeInterval);

	float leftWheelDistanceF = leftRotations * PI * wheelDiameter;
	float rightWheelDistanceF = leftRotations * PI * wheelDiameter;
	
	leftWheelDistance = (uint8_t)leftWheelDistanceF;
	rightWheelDistance = (uint8_t)rightWheelDistanceF;

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