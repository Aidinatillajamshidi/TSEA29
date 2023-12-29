from app.lidar import scanDistanceAngleManual, lidarStart, stopLidar, lidar_process, lidarDisconnect, get_and_calc_odometer
from app.rp_master import read_from_sensor, write_to_styr, initiate_i2c
#from app.manAutoMode import runAuto
from app.runAuto import runAuto

import RPi.GPIO as GPIO
import app.rpslam
from app.rpslam import slam_process, mapbytes_to_plotly

import multiprocessing 
from multiprocessing import Process, Event, Manager, Queue

from rplidar import RPLidar

import plotly.graph_objects as go
from plotly.io import to_json

from fastapi import FastAPI
from fastapi import Request
from fastapi import Response
from fastapi.staticfiles import StaticFiles
from fastapi.responses import StreamingResponse
from fastapi.responses import HTMLResponse
from fastapi.templating import Jinja2Templates



from typing import Generator
import asyncio
import json
import time

#///////////////////////////////////////////////////////////////////////////////////////////
# Dont really know how what the following 5-10 lines does, something about initiating the web app
# Not so good in webdev and front-end
# Includes template folder and static folder to fastAPI
#///////////////////////////////////////////////////////////////////////////////////////////
app = FastAPI()

@app.get("/", response_class=HTMLResponse)
async def read_root(request: Request):
    return templates.TemplateResponse("index.html", {"request": request})
    
templates = Jinja2Templates(directory="templates")
app.mount("/static", StaticFiles(directory="static"), name="static")

#///////////////////////////////////////////////////////////////////////////////////////////
# This part is for declaring variables

#For multiprocess
#Declare core variables 
autoProcess = None
lidarProcess = None
slamProcess = None

#///////////////////////////////////////////////////////////////////////////////////////////
# Declare Queue variables
sensorQueue = None
xythetaQueue = None
figQueue = None
gridQueue = None

# ///////////////////////////////////////////////////////////////////////////////////////////
# Declare manager variable(s)
manager = None
autoVariable = None
oneScanVariable = None
lidarDict = None
controlVariable = None
previousDirLeft = None
previousDirRight = None

# ///////////////////////////////////////////////////////////////////////////////////////////
# Event to handle shutdown of app
shutdown_event = None

#///////////////////////////////////////////////////////////////////////////////////////////
# Global variables 
# sensor data variables (Mostly used for debugging)
dxy = None
theta = None

#///////////////////////////////////////////////////////////////////////////////////////////
# Function to initilize global/multiprocess variables
async def declare_variables():
    #Queue variables
    global sensorQueue, xythetaQueue, figQueue, gridQueue, manager, autoVariable, oneScanVariable, lidarDict, controlVariable, dxy,theta, previousDirLeft, previousDirRight
    # sensorQueue contains lidar data (angle and distance), odometer data (average distance and angle)
    sensorQueue = Queue()
    #xythetaQueue contains (x,y) coordinates of robots current position and angle 
    xythetaQueue = Queue()
    #figQueue contans figure to be plotted on website
    figQueue = Queue()
    gridQueue = Queue()
    
    #Manager variables
    manager = Manager()
    #Updates motor direction, used to correctly calculate odometry data
    previousDirLeft = manager.Value('b', False)
    previousDirRight = manager.Value('b', False)
    #Variable to keep track of auto or manual mode
    autoVariable = manager.Value('b', False)
    #Variable to scan in manual mode
    oneScanVariable = manager.Value('b', False)
    # lidarDict (also) contains angle and corresponding distance at each angle (for eazy access)
    lidarDict = manager.dict({angle: None for angle in range(361)})
    # controlVariable contains control variables (left and right wheel speed and direction for each wheel pair)
    # Debugg
    controlVariable = manager.dict({
        'leftWheelDistance': 0,
        'rightWheelDistance': 0,
        'leftDir': 0,
        'rightDir':0 
    })
    
    dxy = 0
    theta = 0

#///////////////////////////////////////////////////////////////////////////////////////////
# Function that will be called when App starts, initiate multicoreprocess
async def start_processes():
    global autoProcess, lidarProcess, slamProcess
    if lidarProcess and lidarProcess.is_alive():
        print("status: Lidar process is already running.")
    else:
        lidarProcess = multiprocessing.Process(target=lidar_process, args= (shutdown_event, sensorQueue, autoVariable, lidarDict, oneScanVariable))
        lidarProcess.start()
        print("Status Lidar process started and sensor task")
        
    if slamProcess and slamProcess.is_alive():
        print("status: SLAM process is already running.")
    else:
        slamProcess = multiprocessing.Process(target=slam_process, args= (shutdown_event, sensorQueue, autoVariable, xythetaQueue, figQueue, gridQueue, oneScanVariable, previousDirLeft, previousDirRight))
        slamProcess.start()
        print("Status SLAM process started and sensor task")
    
    if autoProcess and autoProcess.is_alive():
        print("status: Auto process is already running.")
        on_shutdown()
    else:
        autoProcess = multiprocessing.Process(target=runAuto, args=(shutdown_event, autoVariable, xythetaQueue, gridQueue,  previousDirLeft, previousDirRight, lidarDict))
        autoProcess.start()
        print("Status Auto process started and sensor task")


#///////////////////////////////////////////////////////////////////////////////////////////
# Function that gets called when app closes tho shutdown and join multicoreprocess
async def join_proccesses():
    global autoProcess, lidarProcess, slamProcess
    
    if autoProcess:
        autoProcess.join()
        autoProcess = None
        print("status Auto process stopped.")
    else:
        print("status Auto process not running.")

    if slamProcess:
        slamProcess.join()
        slamProcess = None
        print("status SLAM process stopped.")
    else:
        print("status SLAM processnot running.")
        
    if lidarProcess:
        lidarProcess.join()
        lidarProcess = None
        print("status Lidar process stopped.")
    else:
        print("status Lidar processnot running.")

# ////////////////////////////////////////////////////////////////////////////////////////
# Physical switch handler
def switch_Switch(channel):
    global autoVariable
    autoVariable.value = not autoVariable.value
    print(f"Toggle autoVariable: {autoVariable.value}")
    
#Set GPIO mode to BCM
GPIO.setmode(GPIO.BCM)
# Set reset pin as output
GPIO.setup(27, GPIO.IN,pull_up_down=GPIO.PUD_UP)
GPIO.add_event_detect(27, GPIO.BOTH, callback = switch_Switch, bouncetime= 1000)

# ////////////////////////////////////////////////////////////////////////////////////////
#On startup of server
@app.on_event("startup")
async def on_startup():
    global shutdown_event
    print('Welcome to RObot hell') 
    shutdown_event = Event()
    await initiate_i2c()
    await declare_variables()
    await start_processes()
    #start server event task to perodically update info boxes on website
    #asyncio.create_task(periodic_task()) 
    
#///////////////////////////////////////////////////////////////////////////////////////////
#Shutting down server
@app.on_event("shutdown")
async def on_shutdown():
    global shutdown_event
    shutdown_event.set()
    GPIO.cleanup()
    await join_proccesses()
#///////////////////////////////////////////////////////////////////////////////////////////
# SSE for map
@app.get('/slam-visualization')
async def get_slam_vis():
    async def event_map_gen():
        # Run aslong as autoVariable is set (AutoMode)
        while autoVariable.value or oneScanVariable.value:
            fig = figQueue.get()
            jsonMapData = to_json(fig, validate = False)
            
            yield f'data: {jsonMapData}\n\n'
            await asyncio.sleep(2) #call every sencond

    # Check if Auto,if return map to website, else do something?
    if autoVariable.value or oneScanVariable.value:
        return StreamingResponse(event_map_gen(), media_type="text/event-stream")
    else:
        return Response(status_code=204)

# ///////////////////////////////////////////////////////////////////////////////////////////
# Updates sensor data // Mostly for debbug but also used on website to see odometry data
async def periodic_task():
    global dxy, theta, odoQueue
    
    tempDxy = 0
    while True:
        tempDxy, theta = get_and_calc_odometer()
        print(f'tempDxy = {tempDxy} | dxy = {dxy}')
        dxy = tempDxy + dxy
        print(f'dxy + tempDxy = {dxy}')
        await asyncio.sleep(1)  # Wait for 0.2 second

#///////////////////////////////////////////////////////////////////////////////////////////
# Server event 
# Infobox 1 update sse
@app.get("/lws-stream")
async def lws_stream(request: Request):
    async def event_stream():
        while True:
            #lwsData = 1  # Replace this with the actual function call to get left wheel speed
            yield f"data: {json.dumps(dxy)}\n\n"
            await asyncio.sleep(1)

    return StreamingResponse(event_stream(), media_type="text/event-stream")

#///////////////////////////////////////////////////////////////////////////////////////////
# Infobox 2 update sse
@app.get("/rws-stream")
async def rws_stream(request: Request):
    async def event_stream():
        while True:
            #rwsData = 1  # Replace this with the actual function call to get left wheel speed
            yield f"data: {json.dumps(theta)}\n\n"
            await asyncio.sleep(1)

    return StreamingResponse(event_stream(), media_type="text/event-stream")

#///////////////////////////////////////////////////////////////////////////////////////////
# This handles the API request to toggle between manual/auto mode
@app.post("/command/{action}")
async def command(action: str):
    autoVariable.value = (action == 'auto')
    return {"status": "Command received: " + action}

#///////////////////////////////////////////////////////////////////////////////////////////
# Handles http/api request to stop motors on robot
@app.post("/stopDirection/{stop}")
async def stopDirection(stop: str):
    if stop == "stop":
        previousDirLeft.value, previousDirRight.value = write_to_styr([0,0,0,0])
        return {"status": "Command received: " + stop}

#///////////////////////////////////////////////////////////////////////////////////////////
# Handles direction http/api request to robot // Manual mode
@app.post("/direction/{direction}")
async def sendDirection(direction: str):
    if direction == 'forward':
        previousDirLeft.value, previousDirRight.value = write_to_styr([128,128,1,1])
        return {"status": "Direction received: " + direction}
    elif direction == 'back':
        previousDirLeft.value, previousDirRight.value =write_to_styr([128,128,0,0])
        return {"statuss": "Direction received: " + direction}
    elif direction == 'right':
        previousDirLeft.value, previousDirRight.value = write_to_styr([128,128,1,0])
        return {"status": "Direction received: " + direction}
    elif direction == 'left':
        previousDirLeft.value, previousDirRight.value = write_to_styr([128,128,0,1])
        return {"status": "Direction received: " + direction}
    elif direction == 'forward-right':
        previousDirLeft.value, previousDirRight.value =write_to_styr([64,128,1,1])
        return {"status": "Direction received: " + direction}
    elif direction == 'forward-left':
        previousDirLeft.value, previousDirRight.value =write_to_styr([128,64,1,1])
        return {"status": "Direction received: " + direction}
    elif direction == 'back-left':
        previousDirLeft.value, previousDirRight.value = write_to_styr([128,64,0,0])
        return {"status": "Direction received: " + direction}
    elif direction == 'back-right':
        previousDirLeft.value, previousDirRight.value = write_to_styr([64,128,0,0])
        return {"status": "Direction received: " + direction}

#///////////////////////////////////////////////////////////////////////////////////////////
#Functioon to toggle scan in manual mode
async def toggle_scan():
    oneScanVariable.value = not oneScanVariable.value
    
#///////////////////////////////////////////////////////////////////////////////////////////
# Handles scan command from web
@app.post("/scan")
async def scan():
    await toggle_scan()
    return {"status": "Starting scan" }

#///////////////////////////////////////////////////////////////////////////////////////////
