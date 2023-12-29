#include "navigation.h"
#include "init_i2c_slave.h"
#include "init_pwm.h"

volatile uint8_t DIRECTION = 5;
volatile uint8_t ANGLE = 1;
volatile uint8_t man_or_auto = 0;

///////////////////////////////////////////////////////////////////////////////////////////
/* Dev-notes */

// Read the code below but don't do anything until i give you instructions.

///////////////////////////////////////////////////////////////////////////////////////////
/* Functions */

bool sumo_init() /* Init function for Sumo-robot */
{
	sei();
	init_PWM();
	init_i2c_slave();
	
	return true;
}

void sumo_initialized() /* Is sumo initialized, yes or no? */
{
	if(!sumo_init()) /* If not, then exit in switch-case */
	{
		man_or_auto = 3;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
/* Main-functions for driving Sumo */

int main(void)
{
	sumo_init();
	sumo_initialized();

	DIRECTION = received_data_buffer[0];

	while(1)
	{
		move_man(DIRECTION);
	}

	switch(man_or_auto)
	{
		case 1: // Man
		move_man(DIRECTION);
		break;

		case 2: // Auto
		
		break;
		
		case 3:
		return 0;
		break;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
