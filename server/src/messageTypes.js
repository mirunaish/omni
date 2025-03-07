export const MessageTypes = {
  // btwn server and client and server and discord, in both directions
  HEARTBEAT: "HEARTBEAT",

  // client to server
  SIGNUP: "SIGNUP",
  LOGIN: "LOGIN",

  // server to client
  SIGNUP_SUCCESS: "SIGNUP_SUCCESS",
  ERROR: "ERROR",

  // discord bot to server
  I_AM_DISCORD: "I_AM_DISCORD",
  DISCORD_LOGIN: "DISCORD_LOGIN",
  PAIR: "PAIR",
  UNPAIR: "UNPAIR",

  // server to discord bot
  SUCCESS: "SUCCESS",
  ERROR: "ERROR", // yea this appears in server to client too

  // forward to arduino
  HEADPAT: "HEADPAT",
  WAVE: "WAVE",
  EXPRESSION: "EXPRESSION", // from discord bot
  IMAGE: "IMAGE", // from discord bot
};
