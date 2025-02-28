import asyncio

from websocket_handler import WebsocketHandler
from serial_handler import SerialHandler


socket = WebsocketHandler()
serial = SerialHandler()


async def process_queue(queue):
    while True:
        # i put messages in the queue as a tuple (source, message)
        source, message = await queue.get()

        if source == "serial":
            await serial.process_message(message, socket)

        elif source == "websocket":
            await socket.process_message(message, serial)

        queue.task_done()


async def main():
    queue = asyncio.Queue()
    loop = asyncio.get_running_loop()
    
    # producers get messages from socket/serial and add them to queue
    websocket_producer = socket.connect(queue)
    serial_producer = serial.connect(queue, loop)
    # process_queue is the consumer

    # handle messages from both serial and websocket at the same time
    await asyncio.gather(websocket_producer, serial_producer, process_queue(queue))


asyncio.run(main())