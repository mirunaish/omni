import { Client, GatewayIntentBits } from "discord.js";
import { WebSocket } from "ws";
import dotenv from "dotenv";
import { Discord } from "./src/discord.js";

dotenv.config();

// set up websocket client (for sending and receiving messages to/from the server)
const socket = new WebSocket(process.env.SERVER_URL);

export function sendToServer(message) {
  if (socket.readyState !== socket.OPEN) return;
  socket.send(JSON.stringify(message));
}

// set up discord bot
const bot = new Client({
  intents: [
    GatewayIntentBits.Guilds, // so it can be in servers
    GatewayIntentBits.GuildMessages, // so it can see messages in servers
    GatewayIntentBits.MessageContent, // so it can see message content
  ],
});

// set up message listeners
socket.on("open", () => {
  console.log("bot connected to server");

  // send a message letting server know that i'm the discord bot
  // sendToServer({ type: "I_AM_DISCORD" });
});

// set up discord bot
bot.once("ready", () => {
  console.log(`discord bot logged in as ${bot.user.tag}`);
  Discord.id = bot.user.id;
});

bot.on("messageCreate", (message) => {
  if (message.author.bot) return; // don't handle messages from bots
  const isCommand =
    message.content.startsWith(`<@${bot.user.id}>`) ||
    message.content.startsWith(`<@!${bot.user.id}>`);
  if (isCommand) Discord.handleDiscordCommand(message);
  else Discord.handleDiscordMessage(message);
});

// handle messages from server (mostly feedback for commands)
socket.on("message", (messageRaw) => {
  const message = JSON.parse(messageRaw);
  console.log("bot received message from server:", message);

  Discord.handleServerMessage(message);
});

bot.login(process.env.DISCORD_TOKEN);
