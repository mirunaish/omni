# O.M.N.I. server

## Setup instructions

1. Copy the contents of `.env.template` into a new file `.env`. Fill out the
   required values.
2. `cd server`
3. `npm install`
4. `npm run dev`

## Connecting to server

Connect to the server using the websocket protocol: URLs starting with `ws://`
or `wss://`

Messages (both sent and received) should be in a JSON format:

```
{
  "type": "<see message types below>"
  "payload": <anything here. a string, an array, or an object>
}
```

Each client is assigned an ID upon connecting. A client can then sign up or log
in to receive a user ID.

## Message Types

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
