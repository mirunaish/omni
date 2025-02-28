import asyncio
import websockets
import json

from config import SERVER_URL
from user_id_cache import load_id, save_id

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
                print("received message from server: ", message)
                self.queue.put_nowait(("websocket", message))
        except Exception as e:
            print("disconnected from server:", e)

    # send a message to the server
    async def send_message(self, type, payload):
        message = json.dumps({"type": type, "payload": payload})
        print("sending message ", message)
        try:
            await self.socket.send(message)
        except Exception as e:
            print("couldn't send message:", e)
    
    async def process_message(self, message, serial):
        message = json.loads(message)
        type, payload = message["type"], message["payload"]

        if type == "SIGNUP_SUCCESS":
            save_id(payload)
        elif type == "ERROR":
            print("got error from server:", payload)
        
        elif type == "HEADPAT":
            # forward to serial
            await serial.send_message(message)
        elif type == "WAVE":
            # forward to serial
            await serial.send_message(message)

        else:
            print("unknown message type:", type)
