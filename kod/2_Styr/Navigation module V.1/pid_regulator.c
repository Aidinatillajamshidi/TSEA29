#include "pid_regulator.h"

///////////////////////////////////////////////////////////////////////////////////////////
/* Dev-notes */

// Read the code below but don't do anything until i give you instructions.

///////////////////////////////////////////////////////////////////////////////////////////
/* Functions */

void pid_activate()
{
	pid_active = 1;
}

/*---------------------------------------------------------------------*/
void pid_deactivate()
{
	pid_active = 0;
}

/*---------------------------------------------------------------------*/
void pid_set(uint8_t temp_kp, uint8_t temp_kd)
{
	kp = temp_kp;
	kd = temp_kd;
}

/*---------------------------------------------------------------------*/
void straight_line(int16_t pid_signal)
{
	if(pid_signal < 0)
	{
		//rotate_right();	
	}
	else
	{
		//rotate_left();
	}
}

/*---------------------------------------------------------------------*/
/* Equation used is: u[n] = kp*e[n] + kd*(e[n]  e[n-1])*/
int16_t pid_calculation(int16_t new_error)
{
	int16_t pid_signal = kp * new_error + kd * (new_error - prev_error);
	prev_error = new_error;
	
	return pid_signal;
}

///////////////////////////////////////////////////////////////////////////////////////////