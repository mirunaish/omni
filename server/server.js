import { WebSocketServer } from "ws";
import dotenv from "dotenv";
import { clients, userIdToClientId, ClientHandler } from "./src/connection.js";
import { uuid } from "./src/utils.js";
import { MessageTypes } from "./src/messageTypes.js";

dotenv.config();

const server = new WebSocketServer({ port: process.env.PORT });

server.on("connection", (ws) => {
  // assign this client an id
  const id = uuid();
  // create handler for this client
  clients[id] = new ClientHandler(ws, id);

  console.log(`client ${id} connected`);

  // handle errors...
  ws.on("error", (error) => {
    console.error(`client ${id} error: ${error}`);
  });

  // when i receive a message
  ws.on("message", (rawMessage) => {
    try {
      // parse message into object
      const message = JSON.parse(rawMessage.toString());
      console.log(
        `server received message from client ${id}: ${JSON.stringify(message)}`
      );

      // if i receive an I_AM_DISCORD, change the client into discord handler...
      if (message.type == MessageTypes.I_AM_DISCORD) {
        delete clients[id].userId; // delete it from client
        discord = new DiscordHandler(ws); // add discord

        // remove default handlers
        ws.removeAllListeners("error");
        ws.removeAllListeners("close");

        // add discord handlers
        ws.on("error", (error) => console.error(`discord error: ${error}`));
        ws.on("close", () => {
          console.log("discord disconnected");
          discord = null;
        });

        return;
      }

      clients[id].handleMessage(message);
    } catch (e) {
      console.error(`client ${id} handler encountered an error: ${e}`);
      console.error(e.stack);
    }
  });

  // when socket closes, do any clean up necessary
  ws.on("close", () => {
    console.log(`client ${id} disconnected`);

    // disconnect from connected client
    clients[id].unpair();

    // remove client
    delete userIdToClientId[clients[id].userId];
    delete clients[id];
  });
});

console.log("socket server running on port " + process.env.PORT);
