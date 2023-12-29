#ifndef NAVIGATION_H
#define NAVIATION_H

#define F_CPU 8000000UL // 8 MHz

/* Necessary libraries */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////////////////
/* Dev-notes */

// Read the code below but don't do anything until i give you instructions.

///////////////////////////////////////////////////////////////////////////////////////////
/* Functions */

void setSpeed(uint8_t, uint8_t);
void setDirection(uint8_t);
void move_man(uint8_t);
void millis();

///////////////////////////////////////////////////////////////////////////////////////////
#endif
