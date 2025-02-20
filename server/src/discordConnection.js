import { clients, userIdToClientId, discordIdToUserId } from "./connection.js";

export const discord = null;

export class DiscordHandler {
  constructor(ws) {
    this.ws = ws;
  }

  handleMessage(message) {
    switch (message.type) {
      case MessageTypes.DISCORD_LOGIN:
        this.discordLogin(message.payload);
        break;

      case MessageTypes.PAIR:
        this.pair(message.payload);
        break;
      case MessageTypes.UNPAIR:
        this.unpair(message.payload);
        break;

      default:
        this.reportError(`unknown message type ${message.type}`);
    }
  }

  discordLogin({ userId, discordId }) {
    const client = clients[userIdToClientId[userId]];
    if (!client) {
      this.reportError("the user id is not correct");
      return;
    }
    // set this client's discord id
    client.discordId = discordId;
    // set the persisted ids
    discordIdToUserId[discordId] = userId;

    // send success message
    this.sendMessage({ type: MessageTypes.DISCORD_LOGIN_SUCCESS });
  }

  pair({ requesterId, requestedId }) {
    // find clients that are being connected
    const requester = clients[userIdToClientId[discordIdToUserId[requesterId]]];
    const requested = clients[userIdToClientId[discordIdToUserId[requestedId]]];
    if (!requester || !requested) {
      this.reportError("either you or your pair is not logged in");
      return;
    }

    requester.pair(requester.id);

    // send success message
    this.sendMessage({ type: MessageTypes.PAIR_SUCCESS });
  }

  unpair(requesterId) {
    // find client being unpaired
    const requester = clients[userIdToClientId[discordIdToUserId[requesterId]]];
    if (!requester) {
      this.reportError("you are not logged in");
      return;
    }
    if (!requester.pairId) {
      this.reportError("you are not paired");
      return;
    }

    // unpair
    requester.unpair();

    // send success message
    this.sendMessage({ type: MessageTypes.UNPAIR_SUCCESS });
  }

  sendMessage(message) {
    const messageString = JSON.stringify(message);
    console.log(`sending message to discord: ${messageString}`);
    if (this.ws.readyState === this.ws.OPEN) {
      this.ws.send(messageString);
    }
  }

  reportError(errorMessage) {
    this.sendMessage({ type: MessageTypes.ERROR, payload: errorMessage });
  }
}
