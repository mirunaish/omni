# O.M.N.I. client

## Set up instructions

1. Make sure you're using Python version 3.13.1 (i recommend using pyenv)
2. Make sure `client/config.py` has the correct server URL and serial port
3. `cd ./client`
4. Create a virtual environment: `python -m venv venv`
5. Activate the virtual environment: `source venv/bin/activate` on Mac,
   `venv/Scripts/activate` on Windows
6. Install requirements with `python -m pip install -r requirements.txt`
7. `python client.py` to run the client
