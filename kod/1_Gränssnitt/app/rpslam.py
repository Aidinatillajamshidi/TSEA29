from breezyslam.algorithms import RMHC_SLAM
from breezyslam.sensors import RPLidarA1 as LaserModel
from breezyslam.sensors import Laser
from breezyslam.vehicles import WheeledVehicle
from rplidar import RPLidar as Lidar
from roboviz import MapVisualizer
from scipy.interpolate import interp1d


from app.rp_master import read_from_sensor, write_to_styr


import numpy as np
import plotly.graph_objects as go
import math
import time

########################################################################
#Declare global variables
lastMapState = None
localPreviousDirLeft = None
localPreviousDirRight = None 
########################################################################
#Lidar class for correct laser modell
class RPLidarA2(Laser):
    def __init__(self):
        # RPLidar A2 specifications
        # Number of distance measurements per scan 
        # Max scans 360, lower if crash
        scan_size = 360 
        # A2 has 360 angles
        detection_angle = 360 
        # Distance between the lidar scanner and the center of rotation
        distance_no_detection_mm = 8000 # This is the max range in millimeters
        # Scan rate (scans per second)
        scan_rate_hz = 15  #Might be 10Hz

        # Call constructor with these parameters
        super(RPLidarA2, self).__init__(
            scan_size=scan_size,
            scan_rate_hz=scan_rate_hz,
            detection_angle_degrees=detection_angle,
            distance_no_detection_mm=distance_no_detection_mm)

########################################################################
#Vehicle class
class MyRobot(WheeledVehicle):
    
    def __init__(self, wheelRadiusMillimeters, halfAxleLengthMillimeters, numberOfMarks): 
        super(MyRobot, self).__init__(wheelRadiusMillimeters, halfAxleLengthMillimeters)
        self.numberOfMarks = numberOfMarks
     #Integrated opdometry calculations here instead
    def extractOdometry(self, timestamp, leftWheelCounts, rightWheelCounts):
        global localPreviousDirLeft, localPreviousDirRight
        wheelDiameter = self.wheelRadiusMillimeters*2
        
        timestampSeconds = timestamp
        # Extract wheel command if forward or backwards to get correct calculations
        leftDir, rightDir = localPreviousDirLeft, localPreviousDirRight
        print(f'LeftDir: {leftDir} | RightDir: {rightDir}')

        #Calculate distance traveled by each wheel
        leftRotations = leftWheelCounts / (self.numberOfMarks*2)
        rightRotations = rightWheelCounts / (self.numberOfMarks*2)
    
        leftWheelDistance = leftRotations*math.pi*wheelDiameter
        rightWheelDistance = rightRotations*math.pi*wheelDiameter
        
        #Check if backwards or forward
        if not leftDir:
            leftWheelDistance = 0 - leftWheelDistance
        if not rightDir:
            rightWheelDistance = 0 - rightWheelDistance

        #Calculate turn angle (if any)    
        leftWheelDegrees = (leftWheelDistance/ wheelDiameter)* (360/math.pi)
        rightWheelDegrees = (rightWheelDistance/ wheelDiameter)* (360/math.pi)
        return(timestampSeconds, leftWheelDegrees, rightWheelDegrees)


########################################################################
# To sort out old elements from map (Not in use right now)
def calculate_Map(newMapState):
    global lastMapState
    if lastMapState is None:
        lastMapState = newMapState
        return {str((x, y)): newMapState[y, x]
                for y in range(newMapState.shape[0])
                for x in range(newMapState.shape[1])}
                
    diff = newMapState != lastMapState
    y_indices, x_indices = np.where(diff)
    newData = {str((x, y)): newMapState[x, y]for y, x in zip(y_indices, x_indices)}
    lastMap = newMapState
    return newData 

########################################################################
# Generate Figure of the map variables, using plotly 
def create_Slam_Map(slamMap, xPos, yPos):
    fig = go.Figure()
    xPos = [xPos]
    yPos = [yPos]
    
    colorScale = [[1, 'black'], [0, 'white']]
    
    fig.add_trace(go.Heatmap(z=slamMap, colorscale='greys' , showscale=False))
    fig.add_trace(go.Scatter(x=xPos, y=yPos, mode = 'markers+lines', name='Robot'))
    fig.update_layout(xaxis_title = 'X', yaxis_title='Y') # title='SLAM VIS', 
    return fig

########################################################################
# Function to conver slam data to plotly data    
def mapbytes_to_plotly(mapbyte, mapSize):
    mapGrid = np.array(mapbyte).reshape(mapSize,mapSize)
    normalizedGrid = 1 - (mapGrid/255)
    #Transpose to change orientation on how plot display on website
    #normalizedGrid = np.transpose(normalizedGrid)[::-1]
    return normalizedGrid

########################################################################
# Main process to handle slam algorithm
def slam_process(event, queue, autoVariable, xythetaQueue, figQueue, gridQueue, oneScanVariable, previousDirLeft, previousDirRight):
    global localPreviousDirLeft, localPreviousDirRight
    #standard variables used to init slam object
    MAP_SIZE_PIXELS = 200
    MAP_SIZE_METERS = 10
    holeWidthMm = 400
    
    wheelRadiusMillimeters = 31
    halfAxleLengthMillimeters = 82.5
    numberOfMarks = 10

    # Ideally we could use all 250 or so samples that the RPLidar delivers in one 
    # scan, but on slower computers you'll get an empty map and unchanging position
    # at that rate.
    MIN_SAMPLES   = 100

    #Initiate vehicle with our specification
    myRobot = MyRobot(wheelRadiusMillimeters, halfAxleLengthMillimeters, numberOfMarks) #

    # Init SLAM object
    slam = RMHC_SLAM(RPLidarA2(), MAP_SIZE_PIXELS, MAP_SIZE_METERS, hole_width_mm =holeWidthMm)

    # Initialize empty map
    mapbytes = bytearray(MAP_SIZE_PIXELS * MAP_SIZE_PIXELS)

    # We will use these to store previous scan in case current scan is inadequate
    previous_distances = None
    previous_angles    = None
    dxy = 0
    dtheta = 0
    
    # Tim e variable to calculate odometry
    lastUpdatedTime = time.time()
    #Run for aslong as the app is active
    while not event.is_set():
        # But only preform SLAM while in autoMode
        while(autoVariable.value or oneScanVariable.value):
            currentTime = time.time()
            elapsedTime = currentTime - lastUpdatedTime
            lastUpdatedTime = currentTime
            
            # Getting data from Lidar
            distances, angles, leftWheelCounts, rightWheelCounts = queue.get()
            # Getting direction from wheels
            localPreviousDirLeft, localPreviousDirRight = previousDirLeft, previousDirRight
            
            print(f'PreviousLeft: {previousDirLeft} | PreviousRight: {previousDirRight}')
            print(f'localPreviousLeft: {localPreviousDirLeft} | localPreviousRight: {localPreviousDirRight}')
            print(f'LeftCount: {leftWheelCounts} | RightCount: {rightWheelCounts}')

            # Here we get correct data extracted with odometry data
            dxy, dtheta, dt = myRobot.computePoseChange(currentTime, leftWheelCounts, rightWheelCounts)
            
            if len(distances) > MIN_SAMPLES:
                # Here we interpolate the missing angles 
                f = interp1d(angles, distances, kind='linear', fill_value = 'extrapolate')
                distances = list(f(np.linspace(0,359,360)))
                #Send to SLAM
                slam.update(distances, pose_change=(dxy, dtheta, dt))#, scan_angles_degrees=previous_angles
                previous_distances = distances.copy()
                previous_angles    = angles.copy()

            # If not adequate, use previous
            elif previous_distances is not None:
                slam.update(previous_distances, pose_change=(dxy, dtheta, dt))#, scan_angles_degrees=previous_angles

            # Get current robot position
            xPos, yPos, theta = slam.getpos()
            xPos = xPos/50
            yPos = yPos/50
            theta = theta

            print(f'X : {xPos} | Y : {yPos} | Theta : {theta}')
            # Put coordinates in Queue
            xythetaQueue.put((xPos, yPos, theta))

            # Update mapbytes
            slam.getmap(mapbytes)
            # Put bytearray in queue
            gridQueue.put(mapbytes)
            #Convert to numpy array
            numpyMapBytes= mapbytes_to_plotly(mapbytes, MAP_SIZE_PIXELS)

            #create figure to be sent to web
            fig = create_Slam_Map(numpyMapBytes , xPos, yPos)
            figQueue.put(fig)

 
