#ifndef I2C_SENSOR_H
#define I2C_SENSOR_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include "main.h"
#include "init.h"


#define SENSOR_SLAVE_ADDRESS 0x41
#define BUFFER_SIZE 8
volatile uint8_t sensor_data_buffer[BUFFER_SIZE];
volatile uint8_t received_data_buffer[BUFFER_SIZE];
volatile uint8_t sensor_index = 0;
volatile uint8_t received_index = 0;

void init_Sensor_Slave(uint8_t sensor_Slave_Address);
void send_Sensor_Data();
void receive_Data();
ISR(TWI_vect);


#endif  // I2C_SENSOR_H
