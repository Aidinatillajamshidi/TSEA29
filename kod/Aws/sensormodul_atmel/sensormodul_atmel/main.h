#ifndef MAIN_H
#define MAIN_H

#include <avr/wdt.h>
#include <avr/io.h>
#include <avr/interrupt.h> 


//Send IR Pulse
void ADC_port_enable_and_convert(volatile uint8_t);
uint8_t check_ADC(volatile uint8_t, uint8_t);
ISR(BADISR_vect);
ISR(ADC_vect);

#endif