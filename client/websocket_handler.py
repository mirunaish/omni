import asyncio
import websockets
import json

from config import SERVER_URL
from user_id_cache import load_id, save_id
from utils import terminate

class WebsocketHandler():
    def __init__(self):
        self.queue = None
        self.socket = None
    
    async def connect(self, queue):
        self.queue = queue

        # connect to websocket server
        try:
            self.socket = await websockets.connect(SERVER_URL)
            print("websocket connected")
        except Exception as e:
            print("couldn't connect to server:", e)
            return

        # load user id
        user_id = load_id()
        # send signup or login message... depending on whether i have a userId
        if user_id is None:
            await self.send_message("SIGNUP", None)
        else:
            await self.send_message("LOGIN", user_id)
        
        # start receiving messages
        await self.handle_messages()

    # receive messages and put them in the queue
    async def handle_messages(self):
        try:
            while True:
                message = await self.socket.recv()
                self.queue.put_nowait(("websocket", message))
        except Exception as e:
            print("disconnected from server:", e)
            terminate()

    # send a message to the server
    async def send_message(self, type, payload):
        message = json.dumps({"type": type, "payload": payload})
        if type != "HEARTBEAT":
            print("sending message to server ", message)

        try:
            await self.socket.send(message)
        except Exception as e:
            print("couldn't send message:", e)
    
    async def process_message(self, message_string, serial):
        message = json.loads(message_string)
        type = message.get("type")
        payload = message.get("payload")

        if type == "HEARTBEAT":
            # respond with a heartbeat
            await self.send_message("HEARTBEAT", None)
            return

        print("received message from server: ", message_string)

        if type == "SIGNUP_SUCCESS":
            save_id(payload)
        elif type == "ERROR":
            print("got error from server:", payload)
        
        elif type == "HEADPAT" or type == "WAVE":
            # forward to serial
            await serial.send_message(type, payload)

        elif type == "EXPRESSION":
            await serial.send_expression(payload)

        elif type == "IMAGE":
            await serial.send_image(payload)
            
        else:
            print("unknown message type:", type)
