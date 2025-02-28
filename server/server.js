import { WebSocketServer } from "ws";
import dotenv from "dotenv";
import { addClient } from "./src/connection.js";
import { addDiscord, discord } from "./src/discordConnection.js";
dotenv.config();

const server = new WebSocketServer({ port: process.env.PORT });

// server assumes that first client to connect is discord
server.on("connection", (ws) => {
  // if discord hasn't been connected, connect discord
  if (discord.handler === null) addDiscord(ws);
  // otherwise connect a new client
  else addClient(ws);
});

console.log("socket server running on port " + process.env.PORT);
