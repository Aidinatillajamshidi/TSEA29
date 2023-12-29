from rplidar import RPLidar
from app.rp_master import read_from_sensor, write_to_styr

import math

########################################################################
# Small functions to handle on/off/disconnect lidar
def lidarDisconnect(lidar):
    lidar.stop()
    lidar.stop_motor()
    lidar.disconnect()

def stopLidar(lidar):
    lidar.stop()
    lidar.stop_motor()

def lidarStart(lidar):
    lidar.start_motor()
    return "Starting motor"
########################################################################
#Oner function to calculate odometrydata, later integrated in to vehicle class in rpslam.py
def get_and_calc_odometer():
    wheelBase = 163 #Distance between center of wheels in mm
    wheelDiameter = 62 #in mm
    numberOfMarks = 10 

    dirData = write_to_styr('getDir')
    leftDir, rightDir = dirData[0], dirData[1]
    print(f'LeftDir: {leftDir} | RightDir: {rightDir}')
    sensorData = read_from_sensor()
    lwdData = sensorData[1]
    rwdData = sensorData[0]
    
    leftRotations = lwdData /(numberOfMarks*2) # // using floor, maybe not?
    rightRotations = rwdData /(numberOfMarks*2)
    
    leftWheelDistance = leftRotations * math.pi * wheelDiameter
    rightWheelDistance = rightRotations * math.pi * wheelDiameter
    
    print(f'leftWheelDistance before: {leftWheelDistance} | rightWheelDistance before: {rightWheelDistance} ')
    if not leftDir:  
        leftWheelDistance  = 0 - leftWheelDistance 
        print(f'leftWheelDistance after: {leftWheelDistance}')      
    if not rightDir:
        rightWheelDistance  = 0- rightWheelDistance 
        print(f'rightWheelDistance after: {rightWheelDistance} ')
        
    dthetaRadians = (rightWheelDistance - leftWheelDistance)/wheelBase
    dxy = (leftWheelDistance + rightWheelDistance)/2

    return dxy, dthetaRadians

########################################################################
#This function now in use, sends count to function in veichle class
def get_odometer():
    sensorData = read_from_sensor()
    lwdData = sensorData[1]
    rwdData = sensorData[0]
    return lwdData, rwdData

########################################################################
#Get Lidar data for slam
def getScansForSlam(lidar, lidarDict):
    distance_mm = []
    angles_deg = []
    for i, scan in enumerate(lidar.iter_scans()):
        for quality, angle, distance in scan:
            if quality > 0 and i != 0: # Skipps firsdt scan becauser bad quality
                angles_deg.append(angle)
                distance_mm.append(distance)
                roundAngle = round(angle)
                lidarDict[roundAngle] = (distance)
        if i >0:
            lidar.stop()
            break
    return distance_mm, angles_deg

########################################################################
#Lidar process, will run on one core. This is for SLAM and auto
def lidar_process(event, queue, autoVariable, lidarDict, oneScanVariable):
    lidar = RPLidar('/dev/ttyUSB0')
    info = lidar.get_info()
    print(info)
    health = lidar.get_health()
    print(health)
    while not event.is_set():
        while(autoVariable.value or oneScanVariable.value):
            distances, angles = getScansForSlam(lidar, lidarDict)
            #dxy, dthetaRadians = get_and_calc_odometer()
            lwdData, rwdData = get_odometer()
            #lidarDict['dxy'] = dxy
            #lidarDict['dthetaRadians'] = dthetaRadians
            #queue.put((distances, angles, dxy, dthetaRadians))
            queue.put((distances, angles, lwdData, rwdData))
        if (not autoVariable.value or not oneScanVariable.value):
            stopLidar(lidar)
            
    #Disconnect and stop lidar
    lidarDisconnect(lidar)

# Alternative function with iterator, not tested 

""" def lidar_process(event, queue, autoVariable, lidarDict, controlVariable):
    lidar = RPLidar('/dev/ttyUSB0')
    lidar.start_motor()
    iterator = lidar.iter_scans()

    while not event.is_set():
        if autoVariable.value:
            try:
                items = next(iterator)  # Fetch the next scan
                distances = [item[2] for item in items if item[0] > 0]  # Quality filtering
                angles = [item[1] for item in items if item[0] > 0]
                dxy, dthetaRadians = get_and_calc_odometer(controlVariable)
                roundAngle = round(angles)
                lidarDict[roundAngle] = (distance)
                lidarDict['dxy'] = dxy
                lidarDict['dthetaRadians'] = dthetaRadians
                queue.put((distances, angles, dxy, dthetaRadians))
            except StopIteration:
                break  # Stop if no more scans

        else:
            time.sleep(0.1)  # Adjust the sleep time as needed

    lidar.stop()
    lidar.stop_motor()
    lidar.disconnect() """

########################################################################
# Debbugf functions
def scanDistanceAngleManual(lidarDict, lidar):
    for i, scan in enumerate(lidar.iter_scans()):
        print('%d: Got %d measurments' % (i, len(scan)))
        for quality, angle, distance in scan:
            roundAngle = round(angle)
            lidarDict[roundAngle] = (distance/1000)
            #print(f'Angle: {roundAngle} | Distance: {lidarDict[roundAngle]}')
        if i > 5:
            lidar.stop()
            lidar.stop_motor()
            break
    for key, value in lidarDict.items():
        print(f"Angle: {key} | Distance: {value}")
        
########################################################################

def scanDistanceAngleAuto(lidarDict):
    for i, scan in enumerate(lidar.iter_scans()):
        print('%d: Got %d measurments' % (i, len(scan)))
        for quality, angle, distance in scan:
            
            roundAngle = round(angle)
            lidarDict[roundAngle] = (distance/1000)
            #print(f'Angle: {roundAngle} | Distance: {lidarDict[roundAngle]}')
        if i > 5:
            break
    for key, value in lidarDict.items():
        print(f"Angle: {key} | Distance: {value}")

########################################################################
