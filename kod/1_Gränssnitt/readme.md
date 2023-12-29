Skapade en venv (virtual enviroment, in python) som kommer köra FastAPI.

1. Allt är fixat på pin, i terminal gå till nästa steg. Mappen ligger i TSEA29/docs.....

2. För att köra venv kör "source venv/bin/activate" i mappen "W.I.P-JATAKA/1_gränssnitt".

5. Nu när hemsidan körs på pin. Först koppla upp pin och en dator på samma nätverk, ex dela från telefon.

3. Sen kör hemsidan, inte i vertual enviromenten (venv) "uvicorn main:app --host 0.0.0.0 --port 8000"

6. tror adressen ni ska ansluta till är http://192.168.72.3:8000/ om inte, kör ifconfig i pins terminal, ipaddressen vi söker är
    - wlan0: inet xxx.xxx.xx.x sen porten 8000


