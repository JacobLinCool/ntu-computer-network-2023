import { Socket, createServer } from "node:net";
import { createServer as createSecuredServer } from "node:tls";
import { handle } from "./handle";
import fs from "node:fs";

const key_fp = "server.key";
const cert_fp = "server.crt";

const use_tls = fs.existsSync(key_fp) && fs.existsSync(cert_fp);
if (use_tls) {
    console.log("TLS enabled");
}

const create = use_tls
    ? (listener?: (socket: Socket) => void) =>
          createSecuredServer(
              {
                  key: fs.readFileSync(key_fp),
                  cert: fs.readFileSync(cert_fp),
              },
              listener
          )
    : createServer;

export const server = create((socket) => {
    socket.on("data", async (data) => {
        socket.setMaxListeners(0);
        socket.on("error", error_handler);
        socket.once("close", () => {
            socket.off("error", error_handler);
        });

        const request = data.toString();
        await handle(socket, request);
    });
});

function error_handler(err: Error) {
    // ignore ECONNRESET or EPIPE
    if (err.message !== "read ECONNRESET" && err.message !== "write EPIPE") {
        console.error(err);
    }
}
