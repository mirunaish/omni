import { sendToServer } from "../bot.js";

export const Discord = {
  requests: {},
  id: "",

  handleDiscordMessage: (message) => {
    // split message up into parts
    let words = message.content.split(" ").slice(1); // remove bot mention
    const command = words[0];
    words = words.slice(1); // remove command

    if (command === "connect") Discord.connect(message, words);
    else if (command === "pair") Discord.pair(message, words);
    else if (command === "unpair") Discord.unpair(message, words);
    // none of those matched, reply with a confused message
    else message.reply("huh?");
  },

  connect: (message, words) => {
    if (words.length != 1) {
      message.reply("connect expects 1 argument: user id");
      return;
    }

    const discordId = message.author.id;
    const userId = words[0];
    Discord.makeServerRequest(message, {
      type: "DISCORD_LOGIN",
      payload: { discordId, userId },
    });
  },

  pair: (message, words) => {
    if (words.length != 1) {
      message.reply(
        "pair expects 1 argument: a discord mention of the other user"
      );
      return;
    }

    const requesterId = message.author.id;
    const requestedId = words[0];
    Discord.makeServerRequest(message, {
      type: "PAIR",
      payload: { requesterId, requestedId },
    });
  },

  unpair: (message, words) => {
    if (words.length != 0) {
      message.reply("unpair expects no arguments");
      return;
    }

    Discord.makeServerRequest(message, {
      type: "UNPAIR",
      payload: message.author.id,
    });
  },

  makeServerRequest: (discordMessage, serverMessage) => {
    // save request to requests object so i can reply to the correct message later
    // generate id
    const requestId = crypto.randomUUID();
    Discord.requests[requestId] = { discordMessage, ...serverMessage };

    // add the request id to the message
    serverMessage = { ...serverMessage, requestId };

    console.log(`making request to server with id ${requestId}`);

    // send message to server
    sendToServer(serverMessage);
  },

  handleServerMessage: (message) => {
    // find request this message is replying to
    const request = Discord.requests[message.requestId];
    if (!request) {
      console.error("no request found for message", message);
      return;
    }

    let messageText = "";
    if (request.type === "DISCORD_LOGIN" && message.type === "SUCCESS")
      messageText = `<@${request.payload.discordId}> and robot ${request.payload.userId} successfully connected`;
    if (request.type === "DISCORD_LOGIN" && message.type === "ERROR")
      messageText = `could not log in: ${message.payload}`;

    if (request.type === "PAIR" && message.type === "SUCCESS")
      messageText = `<@${request.payload.requesterId}> is now paired with <@${request.payload.requestedId}>`;
    if (request.type === "PAIR" && message.type === "ERROR")
      messageText = `could not pair: ${message.payload}`;

    if (request.type === "UNPAIR" && message.type === "SUCCESS")
      messageText = `<@${request.payload}> is no longer paired`;
    if (request.type === "UNPAIR" && message.type === "ERROR")
      messageText = `could not unpair: ${message.payload}`;

    if (messageText === "") {
      console.error("invalid message type", message.type);
      request.discordMessage.reply("something went wrong");
      return;
    }

    // respond
    request.discordMessage.reply(messageText);

    // remove request
    delete Discord.requests[message.requestId];
  },
};
