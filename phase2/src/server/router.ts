import fs from "node:fs";
import { HTTPContext } from "./context";
import path from "node:path";

export type RouteHandler = (ctx: HTTPContext) => Promise<void>;

export class Router {
    protected statics: string[] = [];
    protected aliases = new Map<string, string>();
    protected overrides = new Map<string, RouteHandler>();

    public serve(path: string): this {
        if (fs.existsSync(path) && fs.statSync(path).isDirectory()) {
            this.statics.push(path);
        } else {
            throw new Error(`Directory ${path} does not exist.`);
        }
        return this;
    }

    public alias(path: string, target: string): this {
        this.aliases.set(path, target);
        return this;
    }

    public override(path: string, handler: RouteHandler): this {
        this.overrides.set(path, handler);
        return this;
    }

    public handle(ctx: HTTPContext): Promise<void> {
        const handler = this.overrides.get(ctx.path.pathname);
        if (handler) {
            return handler(ctx);
        }
        return this.serve_static(ctx.path.pathname, ctx);
    }

    public render_static(p: string, vars: Record<string, string>): string | undefined {
        const pathname = this.aliases.get(p) ?? p;
        if (pathname.includes("..")) {
            return undefined;
        }

        for (const dir of this.statics) {
            const fp = path.join(dir, pathname);
            if (fs.existsSync(fp) && fs.statSync(fp).isFile() && path.extname(fp) === ".html") {
                const stream = fs.readFileSync(fp).toString();
                return stream.replace(/\$\{(\w+)\}/g, (_, name) => vars[name] ?? "");
            }
        }

        return undefined;
    }

    public async serve_static(p: string, ctx: HTTPContext): Promise<void> {
        const pathname = this.aliases.get(p) ?? p;
        // prevent directory traversal
        if (pathname.includes("..")) {
            ctx.write_headers(403, [["Content-Type", "text/plain"]]);
            ctx.write_body("Forbidden");
            ctx.end_body();
            return;
        }

        for (const dir of this.statics) {
            const fp = path.join(dir, pathname);
            if (fs.existsSync(fp)) {
                const stat = fs.statSync(fp);
                if (stat.isFile()) {
                    let content_type = "";
                    switch (path.extname(fp)) {
                        case ".html": {
                            content_type = "text/html";
                            break;
                        }
                        case ".css": {
                            content_type = "text/css";
                            break;
                        }
                        case ".js": {
                            content_type = "text/javascript";
                            break;
                        }
                        case ".png": {
                            content_type = "image/png";
                            break;
                        }
                        case ".jpg": {
                            content_type = "image/jpeg";
                            break;
                        }
                        case ".gif": {
                            content_type = "image/gif";
                            break;
                        }
                        case ".ico": {
                            content_type = "image/x-icon";
                            break;
                        }
                        case ".svg": {
                            content_type = "image/svg+xml";
                            break;
                        }
                        case ".json": {
                            content_type = "application/json";
                            break;
                        }
                        case ".mp4": {
                            content_type = "video/mp4";
                            break;
                        }
                        case ".mp3": {
                            content_type = "audio/mpeg";
                            break;
                        }
                    }

                    if (content_type) {
                        console.log(`Serving ${fp} as ${content_type}`);
                        ctx.write_headers(200, [["Content-Type", content_type]]);

                        const stream = fs.createReadStream(fp);
                        for await (const chunk of stream) {
                            ctx.write_body(chunk);
                        }
                        ctx.end_body();
                    } else {
                        ctx.write_headers(415, [["Content-Type", "text/plain"]]);
                        ctx.write_body("Unsupported Media Type");
                        ctx.end_body();
                    }

                    return;
                }
            }
        }

        ctx.not_found();
    }
}
