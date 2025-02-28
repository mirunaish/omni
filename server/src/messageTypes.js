export const MessageTypes = {
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

  // forward
  HEADPAT: "HEADPAT",
};
