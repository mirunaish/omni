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

Each client is assigned an ID upon connecting. A client can then sign up or log
in to receive a user ID.
