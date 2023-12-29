/*
 * sensor_slave.h
 *
 * Created: 2023-11-21 14:38:57
 *  Author: jakpa609
 */ 

#ifndef I2C_SENSOR_H
#define I2C_SENSOR_H

#include "main.h"

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#define SENSOR_SLAVE_ADDRESS 0x35
#define BUFFER_SIZE 4

//volatile uint8_t sensor_data_buffer[BUFFER_SIZE];
volatile uint8_t sensor_data_buffer[BUFFER_SIZE];
volatile uint8_t received_data_buffer[BUFFER_SIZE];
volatile uint8_t sensor_index;
volatile uint8_t received_index;

void init_Sensor_Slave();
void send_Sensor_Data();
void receive_Data();
ISR(TWI_vect);

#endif // I2C_SENSOR_H