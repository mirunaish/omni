# O.M.N.I. (Offering Moments Needed Instantly)

Be in two places at once

## Local setup instructions

1. Start server
2. Start Discord bot
3. Start Arduino
4. Start client

## Server

Communicates with the client via sockets and with the discord bot via HTTP
requests.

See the [documentation](./server/README.md).

## Discord bot

Communicates with the server via HTTP requests.

See the [documentation](./discord_bot/README.md).

## Client

Communicates with the server via sockets and with the Arduino over Serial.

See the [documentation](./client/README.md).

## Arduino project

Communicates with the client over Serial.

See the [documentation](./arduino/README.md).

## Contributors

- Miruna Palaghean
- Michelle Garcia
