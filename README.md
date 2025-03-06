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

## Message Types

### Websocket

All messages over websocket should be in a JSON format:

```
{
  "type": "<see message types below>"
  "payload": <anything here. a string, an array, or an object>
}
```

These are messages that the server receives from the client:

| Message type | Payload                    | Description                                                                    |
| ------------ | -------------------------- | ------------------------------------------------------------------------------ |
| SIGNUP       | none                       | the server assigns the client an ID and responds with a SIGNUP_SUCCESS message |
| LOGIN        | the ID returned by signup. | send after connecting if a client has already been assigned an ID              |

These are messages the server sends the client:

| Message type   | Payload         | Description                          |
| -------------- | --------------- | ------------------------------------ |
| SIGNUP_SUCCESS | the assigned ID | sent in response to a SIGNUP message |
| ERROR          | message         | on any request error                 |

These are messages the server receives from the discord bot:

| Message type  | Payload                      | Description                                         |
| ------------- | ---------------------------- | --------------------------------------------------- |
| I_AM_DISCORD  | none                         | so the server knows which client is the discord bot |
| DISCORD_LOGIN | { userId, discordId }        | connects the user id to the discord id              |
| PAIR          | { requesterId, requestedId } | pairs the two discord ids together                  |
| UNPAIR        | requesterId                  | unpairs the requester discord id                    |

These are messages the server sends the discord bot:

| Message type | Payload | Description            |
| ------------ | ------- | ---------------------- |
| SUCCESS      | message | on any request success |
| ERROR        | message | on any request error   |

Additionally, client and server forward messages between Arduinos, outlined below.

### Serial

Serial messages are space-separated strings:

```
<message type> <payload argument 1> <payload argument 2> ...
```

These are messages that the Arduino sends:

| Message type | Payload    | Description                              |
| ------------ | ---------- | ---------------------------------------- |
| HEADPAT      | none       | forwarded to pair                        |
| WAVE         | name value | name is LEFT or RIGHT. forwarded to pair |
| LOG          | message    | printed in client console for debugging  |
| ERROR        | message    | on any error                             |

These are messages that the Arduino receives:

| Message type | Payload           | Description                                          |
| ------------ | ----------------- | ---------------------------------------------------- |
| HEADPAT      | none              |                                                      |
| WAVE         | name value        |                                                      |
| RESET_SCREEN | none              | resets screen back to black                          |
| PIXELS       | x y size [colors] | a chunk with corner x, y. colors are size\*size ints |

## Contributors

- Miruna Palaghean
- Michelle Garcia
