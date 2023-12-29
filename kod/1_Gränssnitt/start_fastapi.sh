#!/bin/bash

#navigate to project dir
cd /home/jataka/TSEA29/docs/W.I.P-JATAKA/1_Gränssnitt

# activate venv
source .venv/bin/activate

start_fastapi(){
	uvicorn main:app --host 0.0.0.0 --port 8000
}

if lsof -Pi :8000 -sTCP:LISTEN -t >/dev/null ; then
	echo "Port 8000 is in use. Trying to free it up..."
	sudo lsof -t -i tcp:8000 | xargs kill -9
fi

echo "Start FastAPI.."
start_fastapi
