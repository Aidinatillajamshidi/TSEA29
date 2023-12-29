from smbus2 import SMBus, i2c_msg
import time

#Declare local variables
i2cActive = None
localPreviousDirLeft = None
localPreviousDirRight = None

#Define the address of the two i2c_Sensor and i2c_Styr addresses.
i2c_styr_address = None
i2c_sensor_address = None
########################################################################
#Function to initilize i2c on RPI
async def initiate_i2c():
	global i2c_styr_address, i2c_sensor_address, i2cActive, localPreviousDirLeft, localPreviousDirRight
	print('here')
	localPreviousDirLeft = False
	localPreviousDirRight = False
	i2cActive = False

	#Define the address of the two i2c_Sensor and i2c_Styr addresses.
	i2c_styr_address = 0x42
	i2c_sensor_address = 0x35

########################################################################
#Function to read from sensormodule
def read_from_sensor():
	global i2cActive
	
	if i2cActive:
		print('I2C is active')
		return[0,0] #maybe false 
		
	with SMBus(1) as bus:
		register_addr = 0
		try:
			i2cActive = True
			#data = bus.read_block_data(i2c_sensor_address,register_addr, 3)
			data = i2c_msg.read(i2c_sensor_address,2)
			bus.i2c_rdwr(data)
			print(f'{list(data)}')
			return list(data)
		except Exception as e:
			print("error")
			return [0,0]
		finally:
			i2cActive = False

########################################################################
#Function to write to controlmodule
def write_to_styr(data):
	global localPreviousDirLeft, localPreviousDirRight, i2cActive

	if i2cActive:
		print('I2C active')
		return localPreviousDirLeft, localPreviousDirRight
	
	else:
		with SMBus(1) as bus:
			try:
				i2cActive = True
				print(f'Writing to previousdata: {data[-2:]}')
				localPreviousDirLeft, localPreviousDirRight = data[-2], data[-1]
				print(f'written leftDir: {localPreviousDirLeft} | Written rightDir: {localPreviousDirRight} should be {data[-2:]}')
				data = i2c_msg.write(i2c_styr_address,data)
				bus.i2c_rdwr(data)
				i2cActive = False
				return localPreviousDirLeft, localPreviousDirRight
			except Exception as e:
				print("i2c_sending_error")
			finally:
				i2cActive = False
