SERVER_URL = "wss://omni-7wge.onrender.com"
SERIAL_PORT = "COM6"
SERIAL_BAUD_RATE = 1000000
SCREEN_SIZE = 240
SCREEN_SCALE = 5

# will send CHUNK_SIZE*CHUNK_SIZE pixels at a time
# this number should always be a divisor of SCREEN_SIZE
# if it's too small, it loses a few pixels every chunk (? i'd look into it but i don't have time)
# if it's too big, it loses whole chunks more often + losing a chunk loses more data
# good values are 12 (more stable) or 16 (slightly less stable)
CHUNK_SIZE = 12