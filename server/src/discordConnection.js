import { clients, userIdToClientId } from "./connection.js";
import { UserError } from "./errors.js";
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
    try {
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

        case MessageTypes.EXPRESSION:
          await this.forwardToPair(message.type, message.payload);
          break;
        case MessageTypes.IMAGE:
          await this.forwardToPair(message.type, message.payload);
          break;

        default:
          this.reportError(
            `unknown message type ${message.type}`,
            message.requestId
          );
      }
    } catch (e) {
      if (e instanceof UserError) {
        console.error(`user error: ${e.message}`);
        if (message.requestId) this.reportError(e.message, message.requestId);
      } else throw e;
    }
  }

  async getUserId(discordId) {
    const userId = (await fetchUserByDiscordId(discordId))?.userId;
    if (!userId) throw new UserError("you are not connected to discord");
    return userId;
  }
  async getClient(discordId) {
    const client = clients[userIdToClientId[await this.getUserId(discordId)]];
    if (!client) throw new UserError("you are not currently logged in");
    return client;
  }

  async discordLogin({ userId, discordId }, requestId) {
    const client = clients[userIdToClientId[userId]];
    if (!client) throw new UserError("the user id is not correct");

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
    const requester = await this.getClient(requesterId);

    const requestedUserId = await this.getUserId(requestedId);
    if (!requestedUserId)
      throw new UserError("your pair is not connected to discord");

    const requested = clients[userIdToClientId[requestedUserId]];
    if (!requested) throw new UserError("your pair is not currently logged in");

    // if the other user is already paired, report error
    if (requested.pairId)
      throw new UserError("the other user is already paired");

    const oldPairId = await requester.pair(requested);

    // let old pair know they've been unpaired
    if (oldPairId) {
      const oldPairDiscordId = (await fetchUser(oldPairId)).discordId;

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
    const requester = this.getClient(requesterId);

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

  async forwardToPair(type, payloadWithId) {
    let { discordId, ...payload } = payloadWithId;

    const user = await this.getClient(discordId);

    if (Object.values(payload).length == 1) payload = Object.values(payload)[0];
    user.sendToPairedUser({ type, payload });
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
