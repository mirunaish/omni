import { MessageTypes } from "./messageTypes.js";
import { uuid } from "./utils.js";

/** dictionary of all clients */
export const clients = {};
export const userIdToClientId = {}; // map user id to client id
export const discordIdToUserId = {}; // persisted after disconnect

/**
 * map client user id to paired client user id.
 * this is persisted after a user disconnects, so when they log back in they
 * are automatically paired with the same user (if they are also connected)
 */
export const pairedUserIds = {};

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
    if (message.type === MessageTypes.SIGNUP) this.signup();
    if (message.type === MessageTypes.LOGIN) this.login(message.payload);

    // if the message is any other type, the user must be logged in and paired
    if (!this.userId || !this.discordId || !this.pairId) {
      this.sendMessage({
        type: MessageTypes.ERROR,
        payload: "you must be logged in and paired to do that",
      });
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

  /** if this user was previously paired with another user, pair them again */
  attemptToAutoPair() {
    const pairedClient = clients[userIdToClientId[pairedUserIds[this.userId]]];
    if (!pairedClient) return;

    this.pairId = pairedClient.id;
    pairedClient.pairId = this.id;
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

    this.attemptToAutoPair();
  }

  login(userId) {
    // set my user id
    this.userId = userId;

    this.attemptToAutoPair();
  }

  /** given another client's userId, pair with them */
  pair(userId) {
    // unpair from my current pair
    unpair();

    // find the requested user
    const otherUser = clients[userIdToClientId[userId]];

    // set the pair id
    this.pairId = otherUser.id;
    // tell other user i paired with them
    other.pairId = this.id;
  }

  unpair() {
    const otherUser = this.pairedUser();
    if (!otherUser) return;

    // clear the persisted pairing
    delete pairedUserIds[this.userId];
    delete pairedUserIds[otherUser.userId];

    // clear both pair ids
    this.pairId = null;
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
}
