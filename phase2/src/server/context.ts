import { Socket } from "node:net";

export function parse(request: string, socket: Socket): HTTPContext {
    const [start, ...rest] = request.split("\r\n");
    const [method, raw_path] = start.split(" ");

    const spliter = rest.findIndex((line) => line === "");

    const entries = rest.slice(0, spliter).map((header) => {
        const [key, value] = header.split(": ");
        return [key.toLowerCase(), value];
    });

    const headers = entries.reduce((acc, [key, value]) => {
        if (acc[key]) {
            acc[key].push(value);
        } else {
            acc[key] = [value];
        }
        return acc;
    }, {} as Record<string, string[]>);

    const body = rest.slice(spliter + 1).join("\r\n");

    const path = new URL(raw_path, "http://" + (headers["host"][0] || "localhost"));

    return new HTTPContext(method.toUpperCase(), path, headers, body, socket);
}

export class HTTPContext {
    constructor(public method: string, public path: URL, public headers: Record<string, string[]>, public body: string, public socket: Socket) {}

    public write_headers(status_code: number, headers: [string, string][] = []) {
        this.socket.write(`HTTP/1.1 ${status_code}\r\n`);
        this.socket.write("Date: " + new Date().toUTCString() + "\r\n");
        this.socket.write("Connection: Keep-Alive\r\n");
        this.socket.write("Keep-Alive: timeout=5\r\n");
        this.socket.write("Transfer-Encoding: chunked\r\n");
        this.socket.write("Server: Node.js Only\r\n");
        headers.forEach(([key, value]) => {
            this.socket.write(`${key}: ${value}\r\n`);
        });
        this.socket.write("\r\n");
    }

    public write_body(data: string | Buffer) {
        const chunk = Buffer.from(data);
        this.socket.write(chunk.length.toString(16) + "\r\n");
        this.socket.write(chunk);
        this.socket.write("\r\n");
    }

    public end_body() {
        this.socket.write("0\r\n");
        this.socket.write("\r\n");
    }

    public redirect(url: string, headers: [string, string][] = []) {
        this.write_headers(302, [["Location", url], ...headers]);
        this.end_body();
    }

    public not_found() {
        this.write_headers(404, [["Content-Type", "text/plain"]]);
        this.write_body("Not Found");
        this.end_body();
    }

    public get cookies(): Record<string, string> {
        const cookie_header = this.headers["cookie"]?.[0];
        if (!cookie_header) {
            return {};
        }
        return cookie_header.split("; ").reduce((acc, cookie) => {
            const [key, value] = cookie.split("=");
            acc[key] = value;
            return acc;
        }, {} as Record<string, string>);
    }

    public get form(): Record<string, string> {
        return this.body.split("&").reduce((acc, kv) => {
            const [key, value] = kv.split("=");
            acc[key] = value;
            return acc;
        }, {} as Record<string, string>);
    }
}
