/*
 * i2c_sensor.h
 *
 * Created: 2023-11-21 09:00:51
 *  Author: alija148
 */ 


#ifndef I2C_SENSOR_H_
#define I2C_SENSOR_H_


#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/twi.h>


#define SENSOR_SLAVE_ADDRESS 0x41
#define BUFFER_SIZE 32

extern volatile uint8_t sensor_data_buffer[BUFFER_SIZE];
extern volatile uint8_t received_data_buffer[BUFFER_SIZE];
extern volatile uint8_t sensor_index;
extern volatile uint8_t received_index;

void init_Sensor_Slave();
void send_Sensor_Data();
void receive_Data();




#endif /* I2C_SENSOR_H_ */