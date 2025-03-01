from PIL import Image

from config import SCREEN_SIZE, CHUNK_SIZE, SCREEN_SCALE

def color565(r, g, b):
    # Ensure the values are within the valid range (0-255)
    r = max(0, min(255, r))
    g = max(0, min(255, g))
    b = max(0, min(255, b))

    # Convert RGB values (8-bit each) to 16-bit color format
    r_5bit = (r >> 3) & 0x1F  # Take the 5 most significant bits of red (0-31)
    g_6bit = (g >> 2) & 0x3F  # Take the 6 most significant bits of green (0-63)
    b_5bit = (b >> 3) & 0x1F  # Take the 5 most significant bits of blue (0-31)

    # Combine them into one 16-bit value
    return (r_5bit << 11) | (g_6bit << 5) | b_5bit

def get_pixels(image_url):
    pixels = []
    size = int(SCREEN_SIZE / SCREEN_SCALE)
    with Image.open(image_url) as image:
        image.thumbnail((size, size))

        for y in range(size):
            for x in range(size):
                pixel = image.getpixel((x, y))
                pixels.extend([x, y, color565(pixel[0], pixel[1], pixel[2])])
    return pixels

def get_pixel_chunks(image_url):
    chunks = []
    with Image.open(image_url) as image:
        image.thumbnail((SCREEN_SIZE, SCREEN_SIZE))

        # split the image into squares of CHUNK_SIZE*CHUNK_SIZE pixels
        for y in range(0, SCREEN_SIZE, CHUNK_SIZE):
            for x in range(0, SCREEN_SIZE, CHUNK_SIZE):
                chunk = { "x": x, "y": y, "size": CHUNK_SIZE, "pixels": [] }
                for j in range(CHUNK_SIZE):
                    for i in range(CHUNK_SIZE):
                        pixel = image.getpixel((x+i, y+j))
                        chunk["pixels"].append(color565(pixel[0], pixel[1], pixel[2]))
                chunks.append(chunk)
    return chunks
    