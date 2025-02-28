import { MessageTypes } from "./messageTypes.js";
import { uuid } from "./utils.js";
import { storeUser, updatePair, fetchUser } from "./redis.js";

/** dictionary of all clients */
export const clients = {};
export const userIdToClientId = {}; // map user id to client id
export const discordIdToUserId = {}; // only for logged in users

export function addClient(ws) {
  // assign this client an id
  const id = uuid();
  // create handler for this client
  clients[id] = new ClientHandler(ws, id);

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
    const otherUser = clients[id].pairedUser();
    if (otherUser) otherUser.pairId = null;

    // remove client
    delete userIdToClientId[clients[id].userId];
    delete discordIdToUserId[clients[id].discordId];
    delete clients[id];
  });

  console.log(`client ${id} connected`);
}

/** handle a connection */
export class ClientHandler {
  constructor(ws, id) {
    this.ws = ws;
    this.id = id;

    this.userId = null;
    this.pairId = null; // the client id of the paired client
    this.discordId = null; // this user's discord id
  }

  handleMessage(message) {
    if (message.type === MessageTypes.SIGNUP) {
      this.signup();
      return;
    } else if (message.type === MessageTypes.LOGIN) {
      this.login(message.payload);
      return;
    }
    // if the message is any other type, the user must be logged in and paired
    else if (!this.userId || !this.discordId || !this.pairId) {
      this.reportError("you must be logged in and paired to do that");
      return;
    }

    switch (message.type) {
      default:
        console.error(`unknown message type ${message.type}`);
    }
  }

  pairedUser() {
    if (!this.pairId) return null;

    return clients[this.pairId];
  }

  /** assign this client a new user id */
  signup() {
    this.userId = uuid();
    userIdToClientId[this.userId] = this.id;

    // send the user id to the client
    this.sendMessage({
      type: MessageTypes.SIGNUP_SUCCESS,
      payload: this.userId,
    });

    // store the new user in the database (null discordId and paidId)
    storeUser(this.userId, null, null);
  }

  login(userId) {
    // get user from database
    const user = fetchUser(userId);
    if (!user) {
      // report error to user
      this.reportError("user not found");
      return;
    }

    const { discordId, pairId } = user;

    this.userId = userId;
    userIdToClientId[userId] = this.id;

    if (discordId) {
      this.discordId = discordId;
      discordIdToUserId[discordId] = userId;
    }

    // attempt to connect to pair, if my pair is online
    if (pairId) {
      const pair = clients[userIdToClientId[pairId]];
      if (!pair) return;

      this.pairId = pair.id;
      pair.pairId = this.id;
    }
  }

  /** given another client's userId, pair with them */
  pair(userId, requestId) {
    // unpair from my current pair
    unpair();

    // find the requested user
    const otherUser = clients[userIdToClientId[userId]];

    // if the other user is already paired, report error
    if (otherUser.pairId) {
      discord.reportError("the other user is already paired", requestId);
      return;
    }

    // set the pair id
    this.pairId = otherUser.id;
    // tell other user i paired with them
    otherUser.pairId = this.id;

    // save this pairing to the db, in both directions
    updatePair(this.userId, otherUser.userId);
    updatePair(otherUser.userId, this.userId);

    discord.sendMessage({ type: MessageTypes.SUCCESS, requestId });
  }

  unpair() {
    // clear the persisted pairing in the database
    deletePairing(this.userId);

    this.pairId = null;

    const otherUser = this.pairedUser();
    if (!otherUser) return;

    otherUser.pairId = null;
  }

  /** send a message to this client */
  sendMessage(message) {
    const messageString = JSON.stringify(message);
    console.log(`sending message to client ${this.id}: ${messageString}`);
    if (this.ws.readyState === this.ws.OPEN) {
      this.ws.send(messageString);
    }
  }

  reportError(errorMessage) {
    this.sendMessage({
      type: MessageTypes.ERROR,
      payload: errorMessage,
    });
  }
}
