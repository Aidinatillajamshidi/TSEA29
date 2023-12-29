// robot.js

///////////////////////////////////////////////////////////////////////////////////////////
// This part stores values used in various functions

// Keeps track of mode selected to avoid sending unnecessary data
var autoManual = 'manual';
var manScan = false;
// Object to track the state of control buttons
const buttonsPressed = {
  forward: false,
  back: false,
  left: false,
  right: false
};

//Logs what keys are pressed
const keysPressed = {
  ArrowUp: false,
  ArrowDown: false,
  ArrowLeft: false,
  ArrowRight: false
};


///////////////////////////////////////////////////////////////////////////////////////////
// The following functions handles the directions and bindings

//Maps keypress directions to correct buttonPressed
function mapKeyToDirection(key) {
  switch (key) {
      case 'ArrowUp': return 'forward';
      case 'ArrowDown': return 'back';
      case 'ArrowLeft': return 'left';
      case 'ArrowRight': return 'right';
      default: return null;
  }
}

// Function to determine the robot's direction based on buttons pressed
function determineDirection() {
  // Combine button states to determine the direction
  if (buttonsPressed.forward && buttonsPressed.left) {
    return 'forward-left';
  } else if (buttonsPressed.forward && buttonsPressed.right) {
    return 'forward-right';
  } else if (buttonsPressed.back && buttonsPressed.left) {
    return 'back-left';
  } else if (buttonsPressed.back && buttonsPressed.right) {
    return 'back-right';
  } else if (buttonsPressed.forward) {
    return 'forward';
  } else if (buttonsPressed.back) {
    return 'back';
  } else if (buttonsPressed.left) {
    return 'left';
  } else if (buttonsPressed.right) {
    return 'right';
  } else {
    return null;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////

// Function to stop the robot
async function stopRobot() {
  try {
    const response = await fetch(`/stopDirection/${"stop"}`, { method: 'POST' });
    const data = await response.json();
    logCommand('Response: ' + JSON.stringify(data)); // Log the response from the server in the web log
  } catch (error) {
    logCommand('Error: ' + error.message); // Log errors in the web log
  }
}

///////////////////////////////////////////////////////////////////////////////////////////

// Function to move the robot based on the current state
async function moveRobot() {
  const direction = determineDirection();
  if (autoManual === 'manual'){
    if (direction) {
      console.log('Move:', direction);
      logCommand('Move: ' + direction);
      try {
        const response = await fetch(`/direction/${direction}`, { method: 'POST' });
        const data = await response.json();
        logCommand('Response: ' + JSON.stringify(data)); // Log the response from the server in the web log
      } catch (error) {
          logCommand('Error: ' + error.message); // Log errors in the web log
      }
    } else {
      // Stop the robot if no direction is determined (i.e., no buttons are pressed)
      stopRobot();
      logCommand('Move: ' + direction);
    }
  }else{
    logCommand('Toggle manual mode to control robot');
  }
}

///////////////////////////////////////////////////////////////////////////////////////////

// Update the movement state for both button and keyboard inputs
function updateMovementState(inputType, action, direction) {

  if (inputType === 'button') {
    // Handle button press
    buttonsPressed[direction] = action === 'start';
  } else if (inputType === 'keyboard') {
    // Handle keyboard press
      buttonsPressed[mapKeyToDirection(direction)] = keysPressed[direction];
    }
  

  if (buttonsPressed.forward || buttonsPressed.back || buttonsPressed.left || buttonsPressed.right) {
    moveRobot();
  } else {
    stopRobot();
  } 
}

///////////////////////////////////////////////////////////////////////////////////////////
// The following functions handles the input from the website, keypressed/relaesed, touch or button

function handleButtonEvent(event, action, direction) {
  console.log('hello')
  event.preventDefault(); // Prevent default behavior like scrolling
  updateMovementState('button', action, direction);
}

// Event listener for keydown
document.addEventListener('keydown', (e) => {
  if (e.key in keysPressed && !keysPressed[e.key]) {
    keysPressed[e.key] = true;
    updateMovementState('keyboard', null, e.key);
  }
});

//Event listener for keyup
document.addEventListener('keyup', (e) => {
  if (e.key in keysPressed) {
    keysPressed[e.key] = false;  
    updateMovementState('keyboard', null, e.key);
  }
});

///////////////////////////////////////////////////////////////////////////////////////////
//function for changhing between auto/maualy

async function sendCommand(command) {
  console.log('sending command')
  let inUse = command === autoManual;
  //let toggleInterval = (command === 'auto');
  if (!inUse) {
    autoManual = command;
    if (autoManual === 'auto'){
      startSSE();
    }else if (autoManual === 'manual'){
      stopSSE();
    }
    logCommand('Toggle: ' + command);
    try {
      const response = await fetch(`/command/${command}`, { method: 'POST' });
      const data = await response.json();
      logCommand('Response: ' + JSON.stringify(data)); // Log the response from the server in the web log
    } catch (error) {
      logCommand('Error: ' + error.message); // Log errors in the web log
    }
  }else{
    logCommand('Toggle: ' + command + ' already set!');
  }
}

///////////////////////////////////////////////////////////////////////////////////////////
// Function that handles logging event on website
function logCommand(message) {
  const commandLog = document.getElementById('commandLog');
  // Create a new log entry
  const logEntry = document.createElement('div');
  logEntry.textContent = message;
  // Append the new log entry to the command log
  commandLog.appendChild(logEntry);
  // Scroll to the bottom of the log to make the latest entry visible
  commandLog.scrollTop = commandLog.scrollHeight;
}

///////////////////////////////////////////////////////////////////////////////////////////
// Testing server event
// Left wheel distance // Now prints whateve we want in the infobox 1

const lwsEvtSource = new EventSource("/lws-stream");

lwsEvtSource.onmessage = function(event) {
  const lwsSpeed = JSON.parse(event.data);
  // Update html
  const lwsSpeedLog = document.getElementById('lwsSpeedLog');
  lwsSpeedLog.textContent = lwsSpeed;
};

lwsEvtSource.onerror = function(event) {
  console.error("LWS EventSource failed.");
  logCommand("LWS EventSource failed.");
  const lwsSpeedLog = document.getElementById('lwsSpeedLog');
  lwsSpeedLog.textContent = "Disconnected";

  setTimeout(() => {
    lwsEvtSource.close();
    lwsEvtSource = new EventSource("/lws-stream");
    // Re-add the onmessage handler
    lwsEvtSource.onmessage = function(event) {
    const lwsSpeed = JSON.parse(event.data);
    lwsSpeedLog.textContent = lwsSpeed;
  };
  }, 5000); 
};

///////////////////////////////////////////////////////////////////////////////////////////
// right wheel distance // Now prints whateve we want in the infobox 2
const rwsEvtSource = new EventSource("/rws-stream");

rwsEvtSource.onmessage = function(event) {
  const rwsSpeed = JSON.parse(event.data);
  // Update html
  const rwsSpeedLog = document.getElementById('rwsSpeedLog');
  rwsSpeedLog.textContent = rwsSpeed;
};

rwsEvtSource.onerror = function(event) {
  console.error("RWS EventSource failed.");
  logCommand("RWS EventSource failed.");
  const rwsSpeedLog = document.getElementById('rwsSpeedLog');
  rwsSpeedLog.textContent = "Disconnected";

  setTimeout(() => {
    rwsEvtSource.close();
    rwsEvtSource = new EventSource("/rws-stream");
    // Re-add the onmessage handler
    rwsEvtSource.onmessage = function(event) {
    const rwsSpeed = JSON.parse(event.data);
    rwsSpeedLog.textContent = rwsSpeed;
  };
  }, 5000); 
};


//Function http request to Lidar scan function
async function startScan(){
  let scan = (manScan === false);
  manScan = (manScan === false);
  
  if (scan === true){
    logCommand('Starting manual scan of room');
  }else if (scan === false){
    logCommand('Stoping manual scan of room');
  }
  
  if (scan === true){
    startSSE();
  }else if (scan === false){
    stopSSE();
  }

  try {
      const response = await fetch(`/scan`, { method: 'POST' });
      const data = await response.json();
      logCommand('Response: ' + JSON.stringify(data)); // Log the response from the server in the web log
  } catch (error) {
      logCommand('Error: ' + error.message); // Log errors in the web log
  }
}


///////////////////////////////////////////////////////////////////////////////////////////
//Part down here handles server side of map plotting // Not currently in use, couldnt get it to work correctly, hate js
///////////////////////////////////////////////////////////////////////////////////////////
//Function thats be called when app starts
function initializeMapAtStart(){
  creatMapVis(currentMapState);
}

///////////////////////////////////////////////////////////////////////////////////////////
//sort in new data and update currentMapState array
function applyNewData(newData){
  console.log(newData)
  for(const [key, value] of Object.entries(newData)){
    const [x,y] = key.split(',').map(Number);
    console.log('updating x: ${x}, y:${y} with value: ${value}')
    if (!isNaN(x) && !isNaN(y) && y >= 0 && y < currentMapState.length && x >= 0 && x < currentMapState[y].length){
      currentMapState[y][x] = value;
    }else{
      console.error('Invalid indices or value: x=${x}, y=${y}, value=${value}')
    }

  }
}
/*
///////////////////////////////////////////////////////////////////////////////////////////
//Function to inti map variables when server starts
function creatInitMapState(width, height){
  let mapState = new Array(height);
  for (let y = 0; y < height; y++){
    mapState[y] = new Array(width).fill(0);
  }
  return mapState
}

///////////////////////////////////////////////////////////////////////////////////////////
//Creates the map when server starts
function creatMapVis(initialMapState){
  var plotlyData = convertMapStateToPlotly(initialMapState);
  Plotly.newPlot('map', plotlyData.data, plotlyData.layout)
}

///////////////////////////////////////////////////////////////////////////////////////////
//Updates the map trough out
function updateMapVis(currentMapState){
  var plotlyData = convertMapStateToPlotly(currentMapState);
  Plotly.react('map', plotlyData.data, plotlyData.layout)
}

///////////////////////////////////////////////////////////////////////////////////////////
//Converts array data to plotable thingy
function convertMapStateToPlotly(mapState){
  var data = [
    {
      z: mapState,
      type: 'heatmap',
      colorscale: 'Greys',
      showscale: false
    }
  ];
  
  var layout = {
    //title: 'SLAM map'
    xaxis:{
      autorange: true
    },
    yaxis: {
      autorange: true,
      scaleeanchor: 'x',
      scaleratio: 1
    }
  };
  return {data: data, layout: layout}
}
*/

///////////////////////////////////////////////////////////////////////////////////////////
function startSSE(){
  eventSourceMap = new EventSource('/slam-visualization');
  
  eventSourceMap.onmessage = function(event){
    var data = JSON.parse(event.data);
    Plotly.newPlot('map', data.data, data.layout);
    //var newData = JSON.parse(event.data);
    //applyNewData(newData);
    //updateMapVis(currentMapState);
  };
  
  eventSourceMap.onerror = function(event){
    console.log('SSE failed.')
  };
}


function stopSSE(){
  if(eventSourceMap){
    eventSourceMap.close();
    eventSourceMap = null
  }
}

///////////////////////////////////////////////////////////////////////////////////////////

var eventSourceMap;
//var currentMapState = creatInitMapState(800, 800);

///////////////////////////////////////////////////////////////////////////////////////////
/*
document.addEventListener('DOMContentLoaded', function(){
  initializeMapAtStart();
});

///////////////////////////////////////////////////////////////////////////////////////////

//old
var eventSourceMap = new EventSource('/slam-visualization');

eventSourceMap.onmessage = function(event){
    var data = JSON.parse(event.data);
    Plotly.newPlot('map', data.data, data.layout);
}
*/


