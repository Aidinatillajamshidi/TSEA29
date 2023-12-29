#ifndef INIT_I2C_SLAVE_H
#define INIT_I2C_SLAVE_H

#include <avr/interrupt.h>  // Include the necessary header for ISR
  

///////////////////////////////////////////////////////////////////////////////////////////
/* Functions */


#define STYR_SLAVE_ADDRESS 0x42
#define BUFFER_SIZE 1
volatile uint8_t styr_data_buffer[BUFFER_SIZE];
volatile uint8_t received_data_buffer[BUFFER_SIZE];
volatile uint8_t styr_index;
volatile uint8_t received_index;

void init_Styr_Slave();
void send_Styr_Data();
void receive_Data();
ISR(TWI_vect);
int init_i2c_slave();



///////////////////////////////////////////////////////////////////////////////////////////
#endif /* INIT_I2C_SLAVE_H */