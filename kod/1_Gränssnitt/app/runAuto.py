from app.rp_master import write_to_styr

import time
import heapq
import math

import numpy as np

from datetime import datetime

########################################################################
def heuristic(a, b):
    # Using Manhattan distance as heuristic
    return abs(a[0] - b[0]) + abs(a[1] - b[1])

########################################################################
#Function to add distance from wall to path and frontier
def is_near_obstacle(grid, x, y, safetyMargin=2): # pixels= 200 SM= 2, pixels= 300 SM= 3, pixels= 500 SM= 5 
    for dx in range(-safetyMargin, safetyMargin):
        for dy in range(-safetyMargin, safetyMargin):
            nx, ny = x + dx, y + dy
            if 0 <= nx < len(grid) and 0<= ny < len(grid[0]) and grid[nx][ny] != 0:
                return True
    return False

########################################################################
#Function To search neighbours for A*
def get_neighbors(grid, node):
    neighbors = []
    print(f'In start of get_neighbors for node: {node}')
    for dx, dy in [(-1, 0), (1, 0), (0, -1), (0, 1)]:  # Adjacent neighbors
        x, y = node[0] + dx, node[1] + dy
        if 0 <= x < len(grid) and 0 <= y < len(grid[0]):#grid[x][y] == 1:
            if is_near_obstacle(grid, x, y):
                neighbors.append((x, y))
    print(f'Neighbors found: {neighbors}')
    return neighbors

########################################################################
# Function to calculate path to given goal from frontier using A*
def a_star(grid, start, goal):
    print(f"{datetime.now().strftime('%H:%M:%S ')} Starting A* from {start} to {goal}\n")
    open_set = []
    heapq.heappush(open_set, (0, start))
    came_from = {}
    g_score = {start: 0}
    f_score = {start: heuristic(start, goal)}

    while open_set:
        current = heapq.heappop(open_set)[1]
        print(f"{datetime.now().strftime('%H:%M:%S ')} Current node in A*: {current}\n")

        if current == goal:
            print(f'current == goal: {current == goal}')
            return reconstruct_path(came_from, current)

        for neighbor in get_neighbors(grid, current):
            print(f'Checking neighbor: {neighbor}')
            tentative_g_score = g_score[current] + 1
            if neighbor not in g_score or tentative_g_score < g_score[neighbor]:
                came_from[neighbor] = current
                g_score[neighbor] = tentative_g_score
                f_score[neighbor] = tentative_g_score + heuristic(neighbor, goal)
                print(f"tentative_g_score: {tentative_g_score}\n")
                print(f"Updated scores: g_score[{neighbor}] = {g_score[neighbor]}, f_score[{neighbor}] = {f_score[neighbor]}")

                if neighbor not in [i[1] for i in open_set]:
                    heapq.heappush(open_set, (f_score[neighbor], neighbor))
                    print(f"Added {neighbor} to open_set")
        print(f"open_set: {[i[1] for i in open_set]}")
    print("No path found to the goal.")
    return None

########################################################################
# Function to reconstruct path from nodes in A*
def reconstruct_path(came_from, current):
    path = []
    while current in came_from:
        path.append(current)
        current = came_from[current]
    path.reverse()
    print(path)
    return path

########################################################################
# Control function for robot
def translate_to_wheel_commands(turnAngleRadians, distance, lidarDict):
    max_cell_distance = 100 
    max_speed = 255

    # Calculate speed proportional to the distance to travel
    speed = int((distance / max_cell_distance) * max_speed)
    speed = max(min(speed, max_speed), 100)

    # Adjust speed for turning // Currently do nothing
    if abs(turnAngleRadians) > math.pi / 4:  # Turning more than 45 degrees
        turnSpeed = speed 
    else:
        turnSpeed = speed
    
    print(f"turnAngleRadians: {turnAngleRadians}, speed: {speed}, turnSpeed: {turnSpeed}\n")
    print(f"Moving Forward | turnAngleRadians:{turnAngleRadians}, speed: {speed}, turnSpeed: {turnSpeed}\n")

                             # Forward                                                        Back
    if (315 <= (turnAngleRadians)<=360 or  0<=(turnAngleRadians)<= 45) or (135 <= turnAngleRadians <= 225):
        left_speed = right_speed = speed
        if (135 <= (turnAngleRadians) <= 225): # If backwards
            left_direction = right_direction = 0
        else: #Else forward
            left_direction = right_direction = 1

        if turnAngleRadians > 0:  # Turning right
            left_speed = turnSpeed 
        elif turnAngleRadians < 0:  # Turning left
            right_speed = turnSpeed 
        
        print(f"turnAngleRadians: {turnAngleRadians}, speed: {speed}, turnSpeed: {turnSpeed}\n")

                 #           Left                       # Right 
    elif (45) < (turnAngleRadians) < (135) or (225) < (turnAngleRadians) < (315):
        if (45) < (turnAngleRadians) < (135):
            left_speed, right_speed = turnSpeed , speed
            left_direction, right_direction = 0, 1
        else:
            left_speed, right_speed = speed, turnSpeed 
            left_direction, right_direction = 1, 0

        if turnAngleRadians != 0:
            # Adjust for diagonal or curved paths
            left_speed = right_speed = turnSpeed 
        print(f"Right/Left : {turnAngleRadians}, speed: {speed}, turnSpeed: {turnSpeed}\n")
        
    else:  # Stop or undefined direction
        print(f"{datetime.now().strftime('%H:%M:%S ')} Stop or undefined direction\n")
        return 0, 0, 0, 0

    return left_speed, right_speed, left_direction, right_direction

########################################################################
#updates current orientation // Not currently in use
def update_orientation_in_radians(currentOrientation, turnAngleRadians):
    # Update the orientation by adding the turn angle
    # Ensure that the orientation stays within the range [0, 2*pi)
    newOrientation = (currentOrientation + turnAngleRadians) % (2 * math.pi)
    return newOrientation

########################################################################
# Calculate how much robot must turn // Returns in degrees
def calculate_turn_angle(currentOrientation, targetOrientation):
    # Calculate the minimal turn angle from current_orientation to target_orientation
    turnAngle = math.degrees(targetOrientation) - currentOrientation
    return turnAngle

########################################################################
#This returns in radians and normalized
def get_target_orientation_in_radians(current, nextPosition):
    delta_x = nextPosition[0] - current[0]
    delta_y = nextPosition[1] - current[1]
    # Calculate the angle in radians
    angle = math.atan2(delta_y, delta_x)
    # Normalize the angle between 0 and 2*pi
    angle = angle % (2 * math.pi)
    return angle

########################################################################
#Function to handle movements calculations and so on depending on pssition and orrientation
def navigate_path(path, currentOrientation, xythetaQueue, gridQueue,autoVariable,  previousDirLeft, previousDirRight,lidarDict):
    previousLeftSpeed = None
    previousRightSpeed = None
    previousLeftDirection = None
    previousRightDirection = None
    for i in range(len(path) - 1):
        
        if not autoVariable.value:
            return 0
             
        if i == 0:
            currentPosition = path[i]
        else:
            currentPosition = (int(xPos), int(yPos))
            
        nextPosition = path[i + 1]
        
        print(f"{datetime.now().strftime('%H:%M:%S ')} Navigate path, current position: {currentPosition}, next position: {nextPosition}\n")
        
        targetOrientation = get_target_orientation_in_radians(currentPosition, nextPosition)
        print(f"{datetime.now().strftime('%H:%M:%S ')} targetOrientation: {targetOrientation}\n")
        
        turnAngleRadians = calculate_turn_angle(currentOrientation, targetOrientation)# degrees now
        turnAngleRadians = turnAngleRadians  % 360
        print(f"{datetime.now().strftime('%H:%M:%S ')} turnAngleRadians: {turnAngleRadians}\n")
        
        distance = math.sqrt((nextPosition[0] - currentPosition[0]) ** 2 + (nextPosition[1] - currentPosition[1]) ** 2)
        print(f"{datetime.now().strftime('%H:%M:%S ')} distance: {distance}\n")
        
        # Translate direction to wheel speeds and directions
        leftSpeed, rightSpeed, leftDirection, rightDirection = translate_to_wheel_commands(turnAngleRadians, distance,lidarDict)
        print(f"{datetime.now().strftime('%H:%M:%S ')} Commands sent: Left Speed: {leftSpeed}, Right Speed: {rightSpeed}, Left Direction: {leftDirection}, Right Direction: {rightDirection}\n")
        # Send commands to robot if changed

        if (leftSpeed != previousLeftSpeed) or (rightSpeed != previousRightSpeed) or (leftDirection != previousLeftDirection) or (rightDirection != previousRightDirection):
            previousDirLeft.value, previousDirRight.value = write_to_styr([rightSpeed, leftSpeed, leftDirection, rightDirection])
            previousLeftSpeed = leftSpeed 
            previousRightSpeed = rightSpeed
            previousLeftDirection = leftDirection
            previousRightDirection = rightDirection
            
        # Update current orientation based on movement
        xPos, yPos, currentOrientation = xythetaQueue.get()
        # Normalize to 0 .. 360 degrees 
        currentOrientation = currentOrientation % 360
        wasteManager = gridQueue.get()
    #previousDirLeft.value, previousDirRight.value = write_to_styr([0,0,0,0])

########################################################################
# Function to find frontiers
def identify_frontiers(grid, explored): 
    frontiers = []
    rows = len(grid[0])
    cols = len(grid) 
    
    for y in range(rows):
        for x in range(cols):
            if grid[x][y] == 1 and is_adjacent_to_unknown(grid, x, y, explored) and is_near_obstacle(grid, x, y): #and (x,y) not in explored:
                frontiers.append((x,y))
    return frontiers

########################################################################
# Function to check if next to is unknown
def is_adjacent_to_unknown(grid, x, y, explored):
    for dx, dy  in [(-1, 0), (1, 0), (0, -1), (0, 1),(-1,1),(-1,-1),(1,1),(1,-1)]:
        nx, ny = x + dx, y + dy
        if (0 <= nx < len(grid)) and (0 <= ny < len(grid[0])):
            if grid[nx][ny] == 2: # and explored[nx][ny]:
                return True
    return False

########################################################################
# Function to select wich frontier to choose
def select_frontier(frontiers, current_position):
    # Select the nearest frontier to the current position
    furthest_frontier = None
    max_distance = float('inf')
    for frontier in frontiers:
        distance = calculate_distance(current_position, frontier)
        if distance < max_distance:
            furthest_frontier = frontier
            max_distance = distance

    return furthest_frontier

########################################################################
def calculate_distance(point1, point2):
    return math.sqrt((point1[0] - point2[0])**2 + (point1[1] - point2[1])**2)

########################################################################
# Never got this far :(
def return_to_start(grid, current_position, start_position):
    # Use A* algorithm to find the shortest path back to the start position
    return_path = a_star(grid, current_position, start_position)
    return return_path

########################################################################
#Function to update grid we get from slam, never got it to work
def update_grid_from_slam(grid, slam_data):
    for data_point in slam_data:
        x, y, is_obstacle = data_point
        if is_valid_coordinate(grid, x, y):
            if is_obstacle:
                grid[x][y] = 0 
            else:
                grid[x][y] = 1 
def is_valid_coordinate(grid, x, y):
    return 0 <= x < len(grid) and 0 <= y < len(grid[0])
########################################################################
# Converts byte array we get from SLAM to grid map for our auto mode
def convert_to_grid(gridQueue):
    thresholdFree = 100 #or 127 without that scaling boi 0.5
    thresholdUnknown = 127
    gridMap = gridQueue
    gridMap  = np.array(gridMap).reshape(200,200)
    #gridMap  = np.transpose(gridMap)[::-1]
    #normalizedGrid = 1 - (gridMap /255)
    
    binaryGrid = np.zeros_like(gridMap, dtype=int)
    gridSize = gridMap.shape[0]
    
    for y in range(gridSize):
        for x in range(gridSize):
            if gridMap[x,y] < thresholdFree:
                binaryGrid[x,y] = 1
            elif gridMap[y,x] < thresholdUnknown:
                binaryGrid[x,y] = 2
            else:
                binaryGrid[x,y] = 0

    return binaryGrid
########################################################################
# Function that extract frontiers, calls a* and navigation path
def run_exploration(xythetaQueue, gridQueue, autoVariable, previousDirLeft, previousDirRight, lidarDict): #grid, startPosition, 

    explored = []#np.zeros((100,100),dtype=int)
    xPos, yPos, currentOrientation = xythetaQueue.get()
    gridMap = gridQueue.get()
    startPosition = (int(xPos), int(yPos))
    home = startPosition
    
    firstLoop = True
    
    while True:
        if not autoVariable.value:
            return 0 

        if firstLoop:
            currentPosition = startPosition
            firstLoop = False
        else:
            xPos, yPos, currentOrientation = xythetaQueue.get()
            currentPosition = (int(xPos), int(yPos))
            gridMap = gridQueue.get()
        
        grid = convert_to_grid(gridMap)
        frontiers = identify_frontiers(grid, explored)
        
        # If no fronteirs where found return home
        if not frontiers:
            break
            
        targetFrontier = select_frontier(frontiers,currentPosition)
        pathToFrontier = a_star(grid, currentPosition, targetFrontier)
        
        if pathToFrontier == None:
            return 0
        navigate_path(pathToFrontier, currentOrientation, xythetaQueue, gridQueue, autoVariable,  previousDirLeft, previousDirRight, lidarDict)

        explored.append(pathToFrontier)
        
    #grid = convert_to_grid(gridQueue.get())
    #xPos, yPos, currentOrientation = xythetaQueue.get()
    #if (int(xPos),int(yPos)) != home:  
     #   pathHome = a_star(grid, currentPosition, home)
     #   navigate_path(pathHome, currentOrientation)
    return 0

########################################################################
# Main process of auto navigation 
def runAuto(event, autoVariable, xythetaQueue, gridQueue, previousDirLeft, previousDirRight, lidarDict):
    while not event.is_set():
        if autoVariable.value:
            run_exploration(xythetaQueue, gridQueue, autoVariable, previousDirLeft, previousDirRight, lidarDict)
            print('Done')
            autoVariable.value = False

    return 0
