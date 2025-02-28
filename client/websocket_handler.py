import asyncio
import websockets

from config import SERVER_URL

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
    async def send_message(self, message):
        print("sending message ", message)
        try:
            await self.socket.send(message)
        except Exception as e:
            print("couldn't send message:", e)
