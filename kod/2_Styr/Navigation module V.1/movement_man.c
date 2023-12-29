#include "navigation.h"

///////////////////////////////////////////////////////////////////////////////////////////
/* Dev-notes */

// Read the code below but don't do anything until i give you instructions.
// Direction (PORTA0 = RIGHT, PORTA1 = LEFT, 1 FORWARD, 0 BACKWARDS)

///////////////////////////////////////////////////////////////////////////////////////////
/* Global variables for different purposes */

uint8_t wheelpair_speed_right = 128; // DEFAULT
uint8_t wheelpair_speed_left = 128; // DEFAULT

uint8_t right_speed = 0;
uint8_t left_speed = 0;

///////////////////////////////////////////////////////////////////////////////////////////
/* Functions for direction */

void move_forward(uint8_t DIRECTION)
{
	wheelpair_speed_right = 128;
	wheelpair_speed_left = 128;
	TCCR0A |= (1<<COM0A1) | (0<<COM0A0); // COM0Ax = Right wheels // Clear OC0A on compare match set OC0A at BOTTOM // non-inverting mode
	TCCR0A |= (1<<COM0B1) | (0<<COM0B0); // COM0Bx = Left wheels // Clear
	PORTA |= (1<<PORTA0) | (1<<PORTA1); // Set forward as standard
	setSpeed(wheelpair_speed_right, wheelpair_speed_left);
}

void move_backward(uint8_t DIRECTION)
{
	wheelpair_speed_right = 128;
	wheelpair_speed_left = 128;
	TCCR0A |= (1<<COM0A1) | (0<<COM0A0); // COM0Ax = Right wheels // Clear OC0A on compare match set OC0A at BOTTOM // non-inverting mode
	TCCR0A |= (1<<COM0B1) | (0<<COM0B0); // COM0Bx = Left wheels // Clear
	PORTA |= (0<<PORTA0) | (0<<PORTA1); // Set forward as standard
	setSpeed(wheelpair_speed_right, wheelpair_speed_left);
}

void move_right(uint8_t DIRECTION)
{
	wheelpair_speed_right = 128;
	wheelpair_speed_left = 128;
	TCCR0A |= (1<<COM0A1) | (0<<COM0A0); // COM0Ax = Right wheels // Clear OC0A on compare match set OC0A at BOTTOM // non-inverting mode
	TCCR0A |= (1<<COM0B1) | (0<<COM0B0); // COM0Bx = Left wheels // Clear
	PORTA |= (1<<PORTA0) | (0<<PORTA1); // Set forward as standard
	setSpeed(wheelpair_speed_right, wheelpair_speed_left);
}

void move_left(uint8_t DIRECTION)
{
	wheelpair_speed_right = 128;
	wheelpair_speed_left = 128;
	TCCR0A |= (1<<COM0A1) | (0<<COM0A0); // COM0Ax = Right wheels // Clear OC0A on compare match set OC0A at BOTTOM // non-inverting mode
	TCCR0A |= (1<<COM0B1) | (0<<COM0B0); // COM0Bx = Left wheels // Clear
	PORTA |= (0<<PORTA0) | (1<<PORTA1); // Set forward as standard
	setSpeed(wheelpair_speed_right, wheelpair_speed_left);
}

void move_slight_r(uint8_t DIRECTION)
{
	wheelpair_speed_right = 128;
	wheelpair_speed_left = 64;
	TCCR0A |= (1<<COM0A1) | (0<<COM0A0); // COM0Ax = Right wheels // Clear OC0A on compare match set OC0A at BOTTOM // non-inverting mode
	TCCR0A |= (1<<COM0B1) | (0<<COM0B0); // COM0Bx = Left wheels // Clear
	PORTA |= (1<<PORTA0) | (1<<PORTA1); // Set forward as standard
	setSpeed(wheelpair_speed_right, wheelpair_speed_left);
}

void move_slight_l(uint8_t DIRECTION)
{
	wheelpair_speed_right = 64;
	wheelpair_speed_left = 128;
	TCCR0A |= (1<<COM0A1) | (0<<COM0A0); // COM0Ax = Right wheels // Clear OC0A on compare match set OC0A at BOTTOM // non-inverting mode
	TCCR0A |= (1<<COM0B1) | (0<<COM0B0); // COM0Bx = Left wheels // Clear
	PORTA |= (1<<PORTA0) | (1<<PORTA1); // Set forward as standard
	setSpeed(wheelpair_speed_right, wheelpair_speed_left);
}

void set_right_speed(uint8_t duty_threshold) // NY
{
	OCR0A = duty_threshold;
	right_speed = duty_threshold;
}

void set_left_speed(uint8_t duty_threshold) // NY
{
	OCR0B = duty_threshold;
	left_speed = duty_threshold;
}

void set_speed(uint8_t duty_threshold) // NY
{
	set_left_speed(duty_threshold);
	set_right_speed(duty_threshold);
}

void setSpeed(uint8_t currentSpeed_r, uint8_t currentSpeed_l)
{
	OCR0A = currentSpeed_r;
	OCR0B = currentSpeed_l;
}

///////////////////////////////////////////////////////////////////////////////////////////
/* Function for keybinds to move manually (W-A-S-D) */

void move_man(uint8_t DIRECTION)
{
	switch (DIRECTION)
	{
		case 1: // Forward
			move_forward(DIRECTION);
		break;
	
		case 2: // Reverse
			move_backward(DIRECTION);
		break;

		case 3: // Right 360 degree rotation
			move_right(DIRECTION);
		break;

		case 4: // Left 360 degree rotation
			move_left(DIRECTION);
		break;

		case 5: // Slight right turn - 60 degrees
			move_slight_r(DIRECTION);
		break;

		case 6: // Slight left turn - 60 degrees
			move_slight_l(DIRECTION);
		break;
		
		case 7:
			// Här ska vi ha en "STOPP" case
		break;
	}
}


