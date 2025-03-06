import asyncio
import serial_asyncio
import struct

from config import SERIAL_PORT, SERIAL_BAUD_RATE, CHUNK_SIZE, SCREEN_SIZE
from utils import get_pixels, get_pixel_chunks, terminate, split_message

class SerialHandler(asyncio.Protocol):
    def __init__(self):
        self.queue = None
        self.transport = None
        self.buffer = ""
        self.buffer_lock = asyncio.Lock()
        self.serial_lock = asyncio.Lock()
        self.last_image_chunks = None
    
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
        async with self.buffer_lock:
            try:
                messages = data.decode()  # decode binary data into messages
            except UnicodeDecodeError:
                print("couldn't decode data from serial:", data)
                terminate()
            # data is not guaranteed to be a full message
            # so we buffer it and split it into messages
            self.buffer += messages
            # print("buffer is", repr(self.buffer))
            for message in self.buffer.split("\r\n")[:-1]:
                # print("received message from serial:", repr(message))
                self.queue.put_nowait(("serial", message))
            
            # if the last message is complete, clear the buffer
            if (self.buffer[:-2] == "\r\n"):
                self.buffer = ""
            # else if there's an incomplete message, keep it in the buffer
            else:
                self.buffer = self.buffer.split("\r\n")[-1]

    async def send_message(self, type, payload):
        async with self.serial_lock:
            message = type + ';'
            if payload is not None:
                if isinstance(payload, list):
                    payload = " ".join([str(item) for item in payload])
                message += payload
            message += "\n"  # add newline

            print("sending message to serial:", repr(message))

            data = message.encode()  # encode message into binary
            self.transport.write(data)

    async def send_message_bytes(self, type, payload):
        # print("sending pixels image", payload[0], payload[1], payload[2], len(payload)-3)
        type_encoded = type.encode()
        payload_encoded = b''.join([struct.pack('H', i) for i in payload])
        message = type_encoded + (';'.encode()) + payload_encoded + ('\n'.encode())

        # print("sending bytes message to serial:", repr(message))

        self.transport.write(message)

    async def send_image_reset(self):
        await self.send_message("RESET_SCREEN", None)
        await asyncio.sleep(0.8)  # wait for screen to reset...

    async def send_image(self, image_url):
        print("sending image to serial")
        chunks = get_pixel_chunks(image_url)
        if (chunks is None):
            return
        
        # save chunks so i can resend them later
        self.last_image_chunks = chunks

        # first send an image reset
        await self.send_image_reset()

        # aquire lock to prevent other messages from interfering with the string of PIXELs
        async with self.serial_lock:
            for chunk in chunks:
                payload = [chunk["x"], chunk["y"], chunk["size"]]
                payload.extend(chunk["pixels"])
                await self.send_message_bytes("PIXELS", payload)
                # wait for a bit to not overflow the buffer
                # await asyncio.sleep(0.01)
                return  # TODO remvoe
    
    async def resend_chunks(self, x, y):
        if (self.last_image_chunks is None):
            return

        # determine which chunk i am being asked for
        index = (y // CHUNK_SIZE) * (SCREEN_SIZE // CHUNK_SIZE) + (x // CHUNK_SIZE)

        # the first chunk to be sent always has issues for some reason
        # if (index == 0): return

        print("RESENDING CHUNKS", x, y, index, len(self.last_image_chunks))

        async with self.serial_lock:
            # sleep first so i can see what's happening
            await asyncio.sleep(5)
            # send the asked for chunk and the next one
            for j in range(2 if index < len(self.last_image_chunks) - 1 else 1):
                print("resending chunk", j)
                chunk = self.last_image_chunks[index + j]
                payload = [chunk["x"], chunk["y"], chunk["size"]]
                payload.extend(chunk["pixels"])
                await self.send_message_bytes("PIXELS", payload)
                # wait for a bit to not overflow the buffer
                await asyncio.sleep(0.02)

    async def send_expression(self, name):
        # first send an image reset
        await self.send_image_reset()

        # get the expression
        pixels = get_pixels(f"faces/{name.lower()}.png")
        # all the pixels should fit into one message
        await self.send_message_bytes("EXPRESSION", pixels)

    def connection_lost(self, error):
        print("disconnected from serial: ", error)
        terminate()

    async def process_message(self, message, socket):
        # handle messages from serial
        type, payload = split_message(message)

        if type == "HEADPAT":
            # forward to websocket
            await socket.send_message(type, None)
        elif type == "WAVE":
            # forward to websocket
            await socket.send_message(type, {"name": payload[0], "value": int(payload[1])})
        
        elif type == "RESEND_CHUNKS":
            # tell serial to resend image chunks
            await self.resend_chunks(int(payload[0]), int(payload[1]))
        
        elif type == "LOG":
            print("arduino logged:", repr(" ".join(payload)))
        elif type == "ERROR":
            print("arduino reported error:", repr(" ".join(payload)))
        else:
            print("unknown message from serial:", type)
