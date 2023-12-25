import net from "node:net";
import readline from "node:readline";

const MAX_MESSAGE_SIZE = 1024;
const MAX_NAME_LENGTH = 50;

function rename(socket: net.Socket, username: string) {
    socket.write(`/rename ${username}`);
}

function main(server_ip: string, server_port: number) {
    const socket = new net.Socket();

    socket.connect({ host: server_ip, port: server_port }, () => {
        console.log("Connected to server.");

        const rl = readline.createInterface({
            input: process.stdin,
            output: process.stdout,
        });

        rl.question("Enter your username: ", (username) => {
            username = username.trim();
            if (username.length > MAX_NAME_LENGTH) {
                console.error(`Username must be less than ${MAX_NAME_LENGTH} characters.`);
                rl.close();
                socket.end();
                return;
            }

            rename(socket, username);

            rl.on("line", (input) => {
                const message = input.trim();
                if (message.length > MAX_MESSAGE_SIZE) {
                    console.error(`Message must be less than ${MAX_MESSAGE_SIZE} characters.`);
                    return;
                }
                socket.write(message);
            });

            socket.on("data", (data) => {
                console.log(data.toString().trim());
            });

            socket.on("close", () => {
                console.log("Connection closed.");
                rl.close();
                process.exit(0);
            });
        });
    });

    socket.on("error", (err) => {
        console.error(`Connection failed: ${err.message}`);
        process.exit(1);
    });
}

if (process.argv.length < 4) {
    console.log("Usage: tsx src/client <server_ip> <server_port>");
    process.exit(1);
} else {
    const server_ip = process.argv[2];
    const server_port = parseInt(process.argv[3]);
    main(server_ip, server_port);
}
