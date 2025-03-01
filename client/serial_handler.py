import asyncio
import serial_asyncio

from config import SERIAL_PORT, SERIAL_BAUD_RATE
from utils import get_pixels

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
        self.buffer = ""
        self.bufferLock = asyncio.Lock()
    
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
        asyncio.create_task(self.data_received_async(data))

    async def data_received_async(self, data):
        await self.bufferLock.acquire()
        messages = data.decode()  # decode binary data into messages
        # data is not guaranteed to be a full message
        # so we buffer it and split it into messages
        self.buffer += messages
        # print("buffer is", repr(self.buffer))
        for message in self.buffer.split("\r\n")[:-1]:
            message = message.strip()  # remove any leading/trailing whitespace
            # print("received message from serial:", repr(message))
            self.queue.put_nowait(("serial", message))
        
        # if the last message is complete, clear the buffer
        if (self.buffer[:-2] == "\r\n"):
            self.buffer = ""
        # else if there's an incomplete message, keep it in the buffer
        else:
            self.buffer = self.buffer.split("\n")[-1]
        self.bufferLock.release()

    async def send_message(self, type, payload):
        if payload is None:
            message = type
        else:
            if isinstance(payload, list):
                payload = " ".join([str(item) for item in payload])
            message = type + " " + payload
        
        print("sending message to serial:", message)

        message += "\n"  # add newline
        data = message.encode()  # encode message into binary

        # if the message is bigger than 64 bytes, split it into multiple messages
        if len(data) > 64:
            # split the message into chunks of 64 bytes
            for i in range(0, len(data), 64):
                chunk = data[i:i+64]
                self.transport.write(chunk)
                # wait for a bit...
                await asyncio.sleep(0.01)
        else:
            self.transport.write(data)
    
    async def send_image(self, image_url):
        pixels = get_pixels(image_url)

        print("sending image to serial")
        # send 30 pixels at a time (3 numbers for each)
        chunk_size = 30
        for i in range(0, len(pixels), chunk_size*3):
            pixels_chunk = pixels[i:i+chunk_size*3]
            await self.send_message("PIXEL", pixels_chunk)
            # wait for a bit to not overflow the buffer
            await asyncio.sleep(0.01)

    def connection_lost(self, exc):
        print("disconnected from serial: ", exc)

    async def process_message(self, message, socket):
        # handle messages from serial
        type, payload = split_message(message)

        if type == "HEADPAT":
            # forward to websocket
            await socket.send_message(type, payload)
        elif type == "WAVE":
            # forward to websocket
            await socket.send_message(type, payload)

        elif type == "SEND_ME_THE_IMAGE":
            await self.send_image("oiia.jpg")
        
        elif type == "LOG":
            print("arduino logged:", payload)
        elif type == "ERROR":
            print("arduino reported error:", payload)
        else:
            print("unknown message from serial:", type)
        
