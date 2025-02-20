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

| Message Type | Payload                           | Description                                                                    |
| ------------ | --------------------------------- | ------------------------------------------------------------------------------ |
| SIGNUP       | none                              | the server assigns the client an ID and responds with a SIGNUP_SUCCESS message |
| LOGIN        | the ID returned by signup.        | send after connecting if a client has already been assigned an ID              |
| PAIR         | the ID of the client to pair with | the two clients are paired so their robots can communicate                     |
| UNPAIR       | none                              | the clients are unpaired                                                       |

These are messages the server sends the client:

| Message Type   | Payload         | Description                          |
| -------------- | --------------- | ------------------------------------ |
| SIGNUP_SUCCESS | the assigned ID | sent in response to a SIGNUP message |
