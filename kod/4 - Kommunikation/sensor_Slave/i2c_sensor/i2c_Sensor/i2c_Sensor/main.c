/*
 * i2c_sensor_v1.c
 *
 * Created: 2023-11-16 15:50:42
 * Author : alija148
 */ 
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#define SENSOR_SLAVE_ADDRESS 0x42
#define BUFFER_SIZE 32
volatile uint8_t sensor_data_buffer[BUFFER_SIZE];
volatile uint8_t received_data_buffer[BUFFER_SIZE];
volatile uint8_t sensor_index = 0;
volatile uint8_t received_index = 0;
/*
TWCR(TWI Control Register): TWINT|TWEA|TWSTA|TWSTO|TWWC|TWEN|- |TWIE|
7=TWI_Intrrupt_Flag, 6= TWI_Enable_Acknoledge_Bit, 5=TWI_Start_Condition_Bit
4= TWI_Stop_Condition_Bit, 3=TWI_Write_Collision_Flag, 2=TWI_Enable_Bit, 1=Reserved_Bit
0 =TWI_Interrupt Enable.

*/


void init_Sensor_Slave() {
	// S�tt (TWEN) TWI Enable register=1,(TWEA)TWI Enable Acknowledge Bit=1,(TWSTA) TWI Start condition Bit=0,(TWSTO) TWI Stop Condition Bit=0,
	TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT) | (1 << TWIE);
	// S�tt den egna I2C-adressen
	TWAR = SENSOR_SLAVE_ADDRESS << 1;
	sei();
}

void send_Sensor_Data() {
	// Check if TWI is ready to transmit
	if ((TWCR & (1 << TWINT))) {
		// Load the data from the buffer to TWDR using sensor_index
		TWDR = sensor_data_buffer[sensor_index];
		// Clear TWINT to start the data transmission
		TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWIE);
		// Increment the sensor_index for the next sensor's data
		sensor_index++;

		// Check if we have reached the end of the buffer
		if (sensor_index == BUFFER_SIZE) {
			// Reset the index to start from the beginning of the buffer
			sensor_index = 0;
		}
	}
}

void receive_Data() {
	// Receive data and store it in the buffer using received_index
	received_data_buffer[received_index] = TWDR;
	// Increment the received_index for the next received byte
	received_index++;

	// Check if we have reached the end of the buffer
	if (received_index < BUFFER_SIZE) {
		// If it's not the last byte, send ACK and clear TWINT to start the data transmission
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		} 
	else {
		// If it's the last byte, prepare to send NACK and clear TWINT
		TWCR = (1 << TWINT) | (0 << TWEA) | (1 << TWEN) | (1 << TWIE);
		// Reset the index to start from the beginning of the buffer
		received_index = 0;
	}
}



ISR(TWI_vect) {
	//TWI status flags
	switch (TW_STATUS) {
		//Case TWI slave transfer slave_address (0xA8)
		case TW_ST_SLA_ACK:
		// Load the data to be sent
		TWDR = sensor_data_buffer[sensor_index];
		// Print received SLA for debugging
		printf("Received SLA: %x\n", TWAR >> 1);
		// ...
		//checking if the sensor_index reached to BUFFER_SIZE
		if(sensor_index == BUFFER_SIZE){
			sensor_index = 0;
		}
		else
		// Increment the index for the next byte
		sensor_index++;
		// Set TWI status to transmit mode
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;
		
		//Case (0xB0) Arbitration lost in Slave Transmit, SLA received with ACK
		case TW_ST_ARB_LOST_SLA_ACK:
			if (sensor_index == BUFFER_SIZE - 1) {
				TWDR = sensor_data_buffer[sensor_index];
				// If it's the last byte, prepare to send NACK
				TWCR = (1 << TWINT) | (0 << TWEA) | (1 << TWEN) | (1 << TWIE);
				} 
			else {
				TWDR = sensor_data_buffer[sensor_index];
				// If it's not the last byte, send ACK
				TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
			}
		break;
		
		//Case (0xB8) TWI slave transfer data
		case TW_ST_DATA_ACK:
			if (sensor_index == BUFFER_SIZE - 1) {
				TWDR = sensor_data_buffer[sensor_index];
				// If it's the last byte, prepare to send NACK
				TWCR = (1 << TWINT) | (0 << TWEA) | (1 << TWEN) | (1 << TWIE);
			}
			else {
				TWDR = sensor_data_buffer[sensor_index];
				// If it's not the last byte, send ACK
				TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
			}
		//case (0xC0)
		case TW_ST_DATA_NACK:
			// Release the TWI peripheral to be ready for a new start condition
			TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);

			break;
		//case 0xC8
		case TW_ST_LAST_DATA:
			
			TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT) | (1 << TWIE);
			break;

		//case 0x60 TWI slave receiver slave_address
		case TW_SR_SLA_ACK:
			// Set TWI status to receive mode with acknowledgment (ACK)
			TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
			break;

		//Case 0x68
		case TW_SR_ARB_LOST_SLA_ACK:
			 // Release the TWI peripheral to be ready for a new start condition
			 TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;
		//case 0x70
		case TW_SR_GCALL_ACK:

		break;
		//case 0x78
		case TW_SR_ARB_LOST_GCALL_ACK:

		break;
		
		//case 0x80
		case TW_SR_DATA_ACK:
		// Read received data and send ACK for the next byte
		received_data_buffer[received_index] = TWDR;

		// Increment the index for the next received byte
		received_index++;

		if (received_index < (BUFFER_SIZE - 1)) {
			// If it's not the last byte, send ACK
			TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
			} 
		else {
			// If it's the last byte, prepare to send NACK
			TWCR = (1 << TWINT) | (0 << TWEA) | (1 << TWEN) | (1 << TWIE);
		}
		break;

		//case 0x88
		case TW_SR_DATA_NACK:
		// Receive data
		receive_Data();
		// Set TWI status to ready for a new start condition
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;
		//case 0x90
		case TW_SR_GCALL_DATA_ACK:
		receive_Data();
		// Set TWI status to ready for a new start condition
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;

		//case 0x98
		case TW_SR_GCALL_DATA_NACK:
		receive_Data();
		// Set TWI status to ready for a new start condition
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;
		//case 0xF8
		case TW_NO_INFO:

		break;
		//case 0x00
		case TW_BUS_ERROR:
		// Release the internal hardware and clear TWSTO
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;
		//In the case where a STOP condition or repeated START condition has been received while still addressed as a slave
		case TW_SR_STOP:
		// Set TWI status to ready for a new start condition
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		
		break;

		default:
			init_Sensor_Slave();
		
		break;
	}

	// Clear the TWI interrupt flag
	TWCR |= (1 << TWINT);
}

int main () {
	
	init_Sensor_Slave();
	while(1){
		//IF no conversion is active check.
		
	}
}