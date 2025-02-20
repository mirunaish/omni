import { WebSocketServer } from "ws";

const server = new WebSocketServer({ port: 8080 });

server.on("connection", (ws) => {
  console.log("client connected");

  // handle errors...
  ws.on("error", (error) => {
    console.error("websocket error:", error);
  });

  // when i receive a message
  ws.on("message", (message) => {
    console.log("received message ", message.toString());

    // echo message back to sender
    ws.send(message);
  });

  // when socket closes, do any clean up necessary
  ws.on("close", () => {
    console.log("client disconnected");
  });
});

console.log("socket server running on port 8080");
