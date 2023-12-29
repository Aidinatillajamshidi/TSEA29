#include "navigation.h"
#include "init_i2c_slave.h"
#include "init_pwm.h"

volatile uint8_t DIRECTION;
volatile uint8_t oldDirection;
volatile uint8_t ANGLE = 1;
volatile uint8_t man_or_auto = 0;

void sumo_init() /* Init function for Sumo-robot */
{
	init_PWM();
	init_Styr_Slave();
	sei();
	dataRecieved = 0;
}

//wheelSpeedDirectionLR = leftWheelSpeed, rightWheelSpeed, leftWheelDirection, rightWheelDirection
void styrUnit(uint8_t leftWheelSpeed, uint8_t rightWheelSpeed, uint8_t leftWheelDirection, uint8_t rightWheelDirection){
	if((leftWheelSpeed == 0) && (rightWheelSpeed == 0)){
		TCCR0A |= (0<<COM0A1) | (0<<COM0A0); // COM0Ax = Right wheels // Clear OC0A on compare match set OC0A at BOTTOM // non-inverting mode
		TCCR0A |= (0<<COM0B1) | (0<<COM0B0); // COM0Bx = Left wheels // Clear
	}else{
		TCCR0A |= (1<<COM0A1) | (0<<COM0A0); // COM0Ax = Right wheels // Clear OC0A on compare match set OC0A at BOTTOM // non-inverting mode
		TCCR0A |= (1<<COM0B1) | (0<<COM0B0); // COM0Bx = Left wheels // Clear
	};
	
	if (leftWheelDirection == rightWheelDirection) { //Checks if wheel sides have same directions
		if(!leftWheelDirection){ // sets direction "backwards"
			PORTA &= ~(1<<PORTA0) & ~(1<<PORTA1);
			} else if(leftWheelDirection) { //Sets direction "forward"
			PORTA |= (1<<PORTA0) | (1<<PORTA1);
		};
	}else if(leftWheelDirection != rightWheelDirection){ // sets direction if wanna turn fast right or left
		if(rightWheelDirection && !leftWheelDirection){
			PORTA &=  ~(1<<PORTA1);
			PORTA |=  (1<<PORTA0);
		}else if(!rightWheelDirection && leftWheelDirection){
			PORTA &=  ~(1<<PORTA0);
			PORTA |=  (1<<PORTA1);
		}
	};
	OCR0B = leftWheelSpeed; // Set speed of right wheel
	OCR0A = rightWheelSpeed;  // Set speed of right wheel
	
}

///////////////////////////////////////////////////////////////////////////////////////////
/* Main-functions for driving Sumo */

int main(void)
{
	sumo_init();
	while (1){
		if (dataRecieved == 1){
			styrUnit(received_data_buffer[0], received_data_buffer[1], received_data_buffer[2], received_data_buffer[3]);
			dataRecieved = 0;
		}
	};
	return 0;
}

ISR(BADISR_vect) {
	while(1);
}

///////////////////////////////////////////////////////////////////////////////////////////
