//#include "navigation.h"
#include "init_pwm.h"

///////////////////////////////////////////////////////////////////////////////////////////
/* Dev-notes */

// Read the code below but don't do anything until i give you instructions.

///////////////////////////////////////////////////////////////////////////////////////////
/* Function - init PWM */
/* 1. Set data direction of OC0 (PB3, PB4) */
/* 2. Set specific PWM frequency on timer clock for our timer */
/* 3. Specifying the mode of operation to fast PWM */

void init_PWM(void)
{	
	DDRB |= (1 << PB3) | (1 << PB4); // Point 1
	DDRA |= 0xFF; // For direction
	
	TCCR0B |= (1<<CS01); // Point 2 - CLK_IO/8
	
	TCCR0A |= (1<<WGM01) | (1<<WGM00); // Point 3 - MODE 3 (Fast PWM)
}
///////////////////////////////////////////////////////////////////////////////////////////