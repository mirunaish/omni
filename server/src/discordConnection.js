import { clients, userIdToClientId } from "./connection.js";
import { MessageTypes } from "./messageTypes.js";
import { fetchUser, fetchUserByDiscordId, updateDiscordId } from "./redis.js";

export const discord = { handler: null };

// add discord handlers
export function addDiscord(ws) {
  discord.handler = new DiscordHandler(ws); // add discord

  ws.on("error", (error) => console.error(`discord error: ${error}`));

  // when i receive a message
  ws.on("message", async (rawMessage) => {
    try {
      // parse message into object
      const message = JSON.parse(rawMessage.toString());
      console.log(
        `server received message from discord: ${JSON.stringify(message)}`
      );

      await discord.handler.handleMessage(message);
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

  async handleMessage(message) {
    switch (message.type) {
      case MessageTypes.DISCORD_LOGIN:
        await this.discordLogin(message.payload, message.requestId);
        break;

      case MessageTypes.PAIR:
        await this.pair(message.payload, message.requestId);
        break;
      case MessageTypes.UNPAIR:
        await this.unpair(message.payload, message.requestId);
        break;

      default:
        this.reportError(
          `unknown message type ${message.type}`,
          message.requestId
        );
    }
  }

  async discordLogin({ userId, discordId }, requestId) {
    const client = clients[userIdToClientId[userId]];
    if (!client) {
      this.reportError("the user id is not correct", requestId);
      return;
    }
    // set this client's discord id
    client.discordId = discordId;
    // set the persisted ids
    await updateDiscordId(userId, discordId);

    // send success message
    this.sendMessage({
      type: MessageTypes.SUCCESS,
      payload: `<@${discordId}> and robot ${userId} successfully connected`,
      requestId,
    });
  }

  async pair({ requesterId, requestedId }, requestId) {
    // find clients that are being connected
    const requesterUserId = (await fetchUserByDiscordId(requesterId)).userId;
    const requestedUserId = (await fetchUserByDiscordId(requestedId)).userId;
    if (!requesterUserId && !requestedUserId) {
      this.reportError("neither user is connected to discord", requestId);
      return;
    } else if (!requesterUserId) {
      this.reportError("you are not connected to discord", requestId);
      return;
    } else if (!requestedUserId) {
      this.reportError("your pair is not connected to discord", requestId);
      return;
    }

    const requester = clients[userIdToClientId[requesterUserId]];
    const requested = clients[userIdToClientId[requestedUserId]];

    if (!requester && !requested) {
      this.reportError("neither user is currently logged in", requestId);
      return;
    } else if (!requester) {
      this.reportError("you are not currently logged in", requestId);
      return;
    } else if (!requested) {
      this.reportError("your pair is not currently logged in", requestId);
      return;
    }

    // if the other user is already paired, report error
    if (requested.pairId) {
      this.reportError("the other user is already paired", requestId);
      return;
    }

    const oldPairId = await requester.pair(requested);

    // let old pair know they've been unpaired
    if (oldPairId) {
      const oldPairDiscordId = (await fetchUser(oldPairId))?.discordId;

      this.sendMessage({
        type: MessageTypes.SUCCESS,
        payload: `<@${requester.discordId}> and <@${oldPairDiscordId}> are no longer paired`,
        requestId,
      });
    }

    this.sendMessage({
      type: MessageTypes.SUCCESS,
      payload: `<@${requester.discordId}> is now paired with <@${requested.discordId}>`,
      requestId,
    });
  }

  async unpair(requesterId, requestId) {
    // find client being unpaired
    const requesterUserId = (await fetchUserByDiscordId(requesterId)).userId;
    if (!requesterUserId) {
      this.reportError("you are not connected to discord", requestId);
      return;
    }
    const requester = clients[userIdToClientId[requesterUserId]];
    if (!requester) {
      this.reportError("you are not currently logged in", requestId);
      return;
    }
    if (!requester.pairId) {
      this.reportError("you are not paired", requestId);
      return;
    }

    // find old pair, to let them know they've been unpaired
    // need to find in db, they might not be online
    const oldPair = await fetchUser(requester.pairId);
    const oldPairDiscordId = oldPair?.discordId ?? "";

    // unpair
    await requester.unpair();

    // send success message
    this.sendMessage({
      type: MessageTypes.SUCCESS,
      payload: `<@${requester.discordId}> and <@${oldPairDiscordId}> are no longer paired`,
      requestId,
    });
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
