import { clients, userIdToClientId, discordIdToUserId } from "./connection.js";
import { MessageTypes } from "./messageTypes.js";
import { updateDiscordId } from "./redis.js";

export const discord = { handler: null };

// add discord handlers
export function addDiscord(ws) {
  discord.handler = new DiscordHandler(ws); // add discord

  ws.on("error", (error) => console.error(`discord error: ${error}`));

  // when i receive a message
  ws.on("message", (rawMessage) => {
    try {
      // parse message into object
      const message = JSON.parse(rawMessage.toString());
      console.log(
        `server received message from discord: ${JSON.stringify(message)}`
      );

      discord.handler.handleMessage(message);
    } catch (e) {
      console.error(`discord handler encountered an error: ${e}`);
      console.error(e.stack);
    }
  });

  ws.on("close", () => {
    console.log("discord disconnected");
    discord.handler = null;
  });

  console.log("discord connected");
}

export class DiscordHandler {
  constructor(ws) {
    this.ws = ws;
  }

  handleMessage(message) {
    switch (message.type) {
      case MessageTypes.DISCORD_LOGIN:
        this.discordLogin(message.payload, message.requestId);
        break;

      case MessageTypes.PAIR:
        this.pair(message.payload, message.requestId);
        break;
      case MessageTypes.UNPAIR:
        this.unpair(message.payload, message.requestId);
        break;

      default:
        this.reportError(`unknown message type ${message.type}`);
    }
  }

  discordLogin({ userId, discordId }, requestId) {
    const client = clients[userIdToClientId[userId]];
    if (!client) {
      this.reportError("the user id is not correct", requestId);
      return;
    }
    // set this client's discord id
    client.discordId = discordId;
    // set the persisted ids
    discordIdToUserId[discordId] = userId;
    updateDiscordId(userId, discordId);

    // send success message
    this.sendMessage({ type: MessageTypes.SUCCESS, requestId });
  }

  pair({ requesterId, requestedId }, requestId) {
    // find clients that are being connected
    const requester = clients[userIdToClientId[discordIdToUserId[requesterId]]];
    const requested = clients[userIdToClientId[discordIdToUserId[requestedId]]];
    if (!requester || !requested) {
      this.reportError("either you or your pair is not logged in", requestId);
      return;
    }

    requester.pair(requester.id, requestId);
  }

  unpair(requesterId, requestId) {
    // find client being unpaired
    const requester = clients[userIdToClientId[discordIdToUserId[requesterId]]];
    if (!requester) {
      this.reportError("you are not logged in", requestId);
      return;
    }
    if (!requester.pairId) {
      this.reportError("you are not paired", requestId);
      return;
    }

    // unpair
    requester.unpair();

    // send success message
    this.sendMessage({ type: MessageTypes.SUCCESS, requestId });
  }

  sendMessage(message) {
    const messageString = JSON.stringify(message);
    console.log(`sending message to discord: ${messageString}`);
    if (this.ws.readyState === this.ws.OPEN) {
      this.ws.send(messageString);
    }
  }

  reportError(errorMessage, requestId) {
    this.sendMessage({
      type: MessageTypes.ERROR,
      payload: errorMessage,
      requestId,
    });
  }
}
