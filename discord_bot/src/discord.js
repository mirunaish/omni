export const Discord = {
  requests: {},

  handleDiscordMessage: (message) => {
    const formattedMessage = message.content.replace(`<@!${bot.user.id}> `, "");
    const words = formattedMessage.split(" ").slice(1); // array of words

    if (formattedMessage.startsWith("connect")) connect(message, words);
    else if (formattedMessage.startsWith("pair")) pair(message, words);
    else if (formattedMessage.startsWith("unpair")) unpair(message, words);
    // none of those matched, reply with a confused message
    else message.reply("huh?");
  },

  connect: (message, words) => {
    if (words.length > 1) {
      message.reply("too many arguments");
      return;
    }

    const discordId = message.author.id;
    const userId = words[0];
    makeServerRequest(message, {
      type: "DISCORD_LOGIN",
      payload: { discordId, userId },
    });
  },

  pair: (message, words) => {
    if (words.length > 1) {
      message.reply("too many arguments");
      return;
    }

    const requesterId = message.author.id;
    const requestedId = words[0];
    makeServerRequest(message, {
      type: "PAIR",
      payload: { requesterId, requestedId },
    });
  },

  unpair: (message, words) => {
    if (words.length > 0) {
      message.reply("too many arguments");
      return;
    }

    makeServerRequest(message, { type: "UNPAIR", payload: message.author.id });
  },

  makeServerRequest: (discordMessage, serverMessage) => {
    // save request to requests object so i can reply to the correct message later
    // generate id
    const requestId = crypto.randomUUID();
    Discord.requests[requestId] = { discordMessage, ...serverMessage };

    // add the request id to the message
    serverMessage = { ...serverMessage, requestId };

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
      request.channel.send("something went wrong");
      return;
    }

    // respond
    request.channel.send(messageText);

    // remove request
    delete Discord.requests[originalMessage.requestId];
  },
};
