import { Socket } from "node:net";
import { parse } from "./context";
import { Router } from "./router";
import { reflect } from "./dynamic/reflect";
import { compute_sqrt } from "./dynamic/compute-sqrt";
import { register } from "./dynamic/register";
import { chatroom } from "./dynamic/chat";
import { login } from "./dynamic/login";
import { logout } from "./dynamic/logout";
import { chat_status } from "./dynamic/chat-status";

export async function handle(socket: Socket, request: string) {
    const ctx = parse(request, socket);
    console.log(ctx.method, ctx.path.pathname);

    const router = new Router()
        .serve("./site/static")
        .alias("/", "/index.html")
        .alias("/chat", "/chat.html")
        .alias("/register", "/register.html")
        .alias("/login", "/login.html")
        .override("/reflect", reflect)
        .override("/compute-sqrt-1e8-times", compute_sqrt);

    router.override("/register", register(router));
    router.override("/login", login(router));
    router.override("/logout", logout(router));
    router.override("/chat", chatroom(router));
    router.override("/chat-status", chat_status);

    await router.handle(ctx);
}
