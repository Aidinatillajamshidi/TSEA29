#ifndef MAIN_H
#define MAIN_H

#include <avr/wdt.h>
#include <avr/io.h>
#include <avr/interrupt.h> 
#include <stdio.h>

//Send IR Pulse
void ADC_port_enable_and_convert(volatile uint8_t);
uint8_t check_ADC(volatile uint8_t, uint8_t);
ISR(BADISR_vect);
ISR(ADC_vect);

// Function to pack data in to 32bit
void packData(volatile uint8_t cntr, volatile float adcData, volatile uint8_t leftWheelDistance, volatile uint8_t rightWheelDistance);



/* to unpack data
uint32_t packedData = ...;  // The 32-bit packed data

// Unpacking the data
uint8_t cntr = packedData & 0x03;                          // Mask with 0x03 to get the 2-bit cntr
uint8_t adcData = (packedData >> 2) & 0xFF;                // Shift right by 2 and mask with 0xFF to get the 8-bit adcData
uint8_t leftWheelDistance = (packedData >> 10) & 0xFF;     // Shift right by 10 and mask with 0xFF to get the 8-bit leftWheelDistance
uint8_t rightWheelDistance = (packedData >> 18) & 0xFF;    // Shift right by 18 and mask with 0xFF to get the 8-bit rightWheelDistance

// Now the variables cntr, adcData, leftWheelDistance, and rightWheelDistance have their original values
*/

#endif