import { Client, GatewayIntentBits } from "discord.js";
import { WebSocket } from "ws";
import dotenv from "dotenv";

dotenv.config();

// set up websocket client (for sending and receiving messages to/from the server)
const socket = new WebSocket(process.env.SERVER_URL);

function sendToServer(message) {
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
  console.log("websocket connected");

  // send a message letting server know that i'm the discord bot
  sendToServer({ type: "I_AM_DISCORD" });
});

// set up discord bot
bot.once("ready", () => {
  console.log(`discord bot logged in as ${bot.user.tag}`);
});

bot.on("messageCreate", (message) => {
  if (message.author.bot) return; // don't handle messages from bots
  if (!message.content.startsWith(`<@!${bot.user.id}>`)) return; // don't handle messages not talking to me

  Discord.handleDiscordMessage(message);
});

// handle messages from server (mostly feedback for commands)
socket.on("message", (messageRaw) => {
  const message = JSON.parse(messageRaw);
  console.log("bot received message from server:", message);

  Discord.handleServerMessage(message);
});

bot.login(process.env.DISCORD_TOKEN);
