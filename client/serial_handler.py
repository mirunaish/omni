import asyncio
import serial_asyncio

from config import SERIAL_PORT, SERIAL_BAUD_RATE

def split_message(message):
    words = message.split(' ', 1)  # split off first word from others
    
    if len(words) > 1:
        first_word, remaining_message = words
        return first_word, remaining_message
    else:
        # no space. only one word
        return message, None
        

class SerialHandler(asyncio.Protocol):
    def __init__(self):
        self.queue = None
        self.transport = None
    
    async def connect(self, queue, loop):
        self.queue = queue
        try:
            await serial_asyncio.create_serial_connection(loop, lambda: self, SERIAL_PORT, SERIAL_BAUD_RATE)
        except Exception as e:
            print("couldn't connect to serial:", e)
    
    def connection_made(self, transport):
        print("serial connected")
        self.transport = transport

    # serial_asyncio handles receiving messages
    def data_received(self, data):
        message = data.decode().strip()  # decode binary data into message
        print("received message from serial: ", message)
        self.queue.put_nowait(("serial", message))

    async def send_message(self, type, payload):
        if payload is None:
            message = type
        else:
            if isinstance(payload, list):
                payload = " ".join(payload)
            message = type + " " + payload
        
        print("sending message to serial: ", message)
        data = message.encode()  # encode message into binary
        self.transport.write(data)

    def connection_lost(self, exc):
        print("disconnected from serial: ", exc)

    async def process_message(self, message, socket):
        # handle messages from serial
        type, payload = split_message(message)
        if type == "HEADPAT":
            # forward to websocket
            await socket.send_message(type, payload)
        
