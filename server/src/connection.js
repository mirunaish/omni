import { MessageTypes } from "./messageTypes.js";
import { uuid } from "./utils.js";

/** dictionary of all clients */
export const clients = {};
export const userIdToClientId = {}; // map user id to client id

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
  }

  handleMessage(message) {
    switch (message.type) {
      case MessageTypes.SIGNUP:
        this.signup();
        break;
      case MessageTypes.LOGIN:
        this.login(message.payload);
        break;
      case MessageTypes.PAIR:
        this.pair(message.payload);
        break;
      case MessageTypes.UNPAIR:
        this.unpair();
        break;
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
    // find the user
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
