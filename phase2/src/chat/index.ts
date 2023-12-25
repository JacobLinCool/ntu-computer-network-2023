import net from "node:net";

interface ChatClient {
    socket: net.Socket;
    name?: string;
}

export class ChatServer {
    private clients: ChatClient[] = [];
    private server: net.Server;
    private client_count = 0;

    constructor(private config: ChatServerConfig) {
        this.server = net.createServer((socket) => this.welcome(socket));
    }

    start() {
        this.server.listen(this.config.server_port, () => {
            console.log(`Socket chat server listening on port ${this.config.server_port}`);
        });
    }

    stop() {
        this.server.close();
        this.clients.forEach((client) => client.socket.destroy());
        this.clients = [];
    }

    private welcome(socket: net.Socket) {
        const client: ChatClient = { socket };
        this.clients.push(client);

        socket.on("data", (data) => this.receive(client, data));
        socket.on("end", () => this.remove(client));
    }

    private receive(client: ChatClient, data: Buffer) {
        const message = data.toString().trim();
        if (message.startsWith("/rename ")) {
            const name = message.substring(8).trim();
            if (name.length > this.config.max_name_length) {
                client.socket.write("Server: Name too long.\n");
                return;
            }
            client.name = name;
            client.socket.write("Server: Name changed.\n");
        } else if (!client.name) {
            client.socket.write("Server: Please set your name first with /rename <your-name>.\n");
        } else {
            const broadcast = `${client.name}: ${message}`.trim();
            console.error(`Broadcasting: ${broadcast}`);
            this.clients.forEach((c) => c.socket.write(broadcast + "\n"));
        }
    }

    private remove(client: ChatClient) {
        this.clients = this.clients.filter((c) => c !== client);
        client.socket.destroy();
    }
}

export interface ChatServerConfig {
    max_name_length: number;
    server_port: number;
}

const config: ChatServerConfig = {
    max_name_length: parseInt(process.env.CHAT_SERVER_MAX_NAME_LENGTH || "50"),
    server_port: parseInt(process.env.CHAT_SERVER_PORT || "3000"),
};

const chatServer = new ChatServer(config);
chatServer.start();
