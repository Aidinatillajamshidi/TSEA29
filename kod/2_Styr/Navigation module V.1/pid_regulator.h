#ifndef PID_REGULATOR_H
#define PID_REGULATOR_H

#include <stdint.h>
#include <avr/interrupt.h>

volatile uint8_t pid_active = 0;
volatile uint8_t kp = 6;
volatile uint8_t kd = 5;
volatile int16_t prev_error;

///////////////////////////////////////////////////////////////////////////////////////////
/* Dev-notes */

// Read the code below but don't do anything until i give you instructions.

///////////////////////////////////////////////////////////////////////////////////////////
/* Functions */

void pid_activate();
void pid_deactivate();
void pid_set(uint8_t, uint8_t);
void straight_line(int16_t);
int16_t pid_calculation(int16_t);

///////////////////////////////////////////////////////////////////////////////////////////
#endif /* PID_REGULATOR_H*/