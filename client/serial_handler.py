import asyncio
import serial_asyncio
import struct

from config import SERIAL_PORT, SERIAL_BAUD_RATE
from utils import get_pixels, get_pixel_chunks, terminate, split_message

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
        print("serial connected on", SERIAL_PORT)
        self.transport = transport

    # serial_asyncio handles receiving messages
    def data_received(self, data):
        asyncio.create_task(self.data_received_async(data))

    async def data_received_async(self, data):
        await self.bufferLock.acquire()
        try:
            messages = data.decode()  # decode binary data into messages
        except UnicodeDecodeError:
            print("couldn't decode data from serial")
            terminate()
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
        
        if type != "PIXEL" and type != "PIXEL_SCALED":
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

    async def send_message_bytes(self, type, payload):
        message = None
        type_encoded = type.encode()

        if payload is None:
            message = type_encoded
        else:
            if isinstance(payload, list):
                payload_encoded = b''.join([struct.pack('H', i) for i in payload])
            else:
                payload_encoded = payload.encode()
            message = type_encoded + (' '.encode()) + payload_encoded + ('\n'.encode())
        
        # if type != "PIXEL" and type != "PIXEL_SCALED":
        # print("sending message to serial:", message)

        self.transport.write(message)
    
    async def send_image(self, image_url):
        print("sending image to serial")
        chunks = get_pixel_chunks(image_url)
        
        for chunk in chunks:
            payload = [chunk["x"], chunk["y"], chunk["size"]]
            payload.extend(chunk["pixels"])
            await self.send_message("PIXEL", payload)
            # wait for a bit to not overflow the buffer
            await asyncio.sleep(0.01)

    async def send_image_bytes(self, image_url):
        print("sending image to serial")
        chunks = get_pixel_chunks(image_url)
        
        for chunk in chunks:
            payload = [chunk["x"], chunk["y"], chunk["size"]]
            payload.extend(chunk["pixels"])
            await self.send_message_bytes("PIXEL_BYTES", payload)
            # await self.send_message("PIXEL", payload)
            # wait for a bit to not overflow the buffer
            # await asyncio.sleep(0.01)
    
    async def send_image_scaled(self, image_url):
        pixels = get_pixels(image_url)
        print(len(pixels), "numbers")
        # send 25 pixels (each with 3 numbers) at a time
        for i in range(0, len(pixels), 25*3):
            await self.send_message("PIXEL_SCALED", pixels[i:i+25*3])
            # wait for a bit to not overflow the buffer
            await asyncio.sleep(0.01)

    def connection_lost(self, error):
        print("disconnected from serial: ", error)
        terminate()

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
            await self.send_image_bytes("oiia.jpg")
        
        elif type == "LOG":
            print("arduino logged:", payload)
        elif type == "ERROR":
            print("arduino reported error:", payload)
        else:
            print("unknown message from serial:", type)
