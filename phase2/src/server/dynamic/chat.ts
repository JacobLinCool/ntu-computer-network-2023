import { HTTPContext } from "../context";
import { Router } from "../router";
import { user, chat } from "../data";
import { gpt_reply } from "../utils/gpt";

export const chatroom = (router: Router) => async (ctx: HTTPContext) => {
    const cookies = ctx.cookies;
    const uid = cookies.user;
    const u = user.get(uid);

    if (!u) {
        return ctx.redirect("/register");
    }

    if (cookies.token !== u.token) {
        return ctx.redirect("/login", [
            ["Set-Cookie", `user=; Path=/; HttpOnly`],
            ["Set-Cookie", `token=; Path=/; HttpOnly`],
        ]);
    }

    if (ctx.method === "GET") {
        const chats = chat.get();
        const html = router.render_static(ctx.path.pathname, {
            chats: chats
                .map((c) => `<div><b>${c.user}</b>: ${c.message.replace(/</g, "&lt;").replace(/>/g, "&gt;")} (${new Date(c.timestamp).toLocaleString()})</div>`)
                .join("\n"),
        });

        if (html) {
            ctx.write_headers(200, [["Content-Type", "text/html"]]);
            ctx.write_body(html);
            ctx.end_body();
        } else {
            ctx.not_found();
        }
    }

    if (ctx.method === "POST") {
        const form = ctx.form;
        const message = decodeURIComponent(form.message.replace(/\+/g, " ")).slice(0, 1024);

        chat.add({
            user: u.id,
            message: message,
            timestamp: Date.now(),
        });

        if (message.toLowerCase().startsWith("@gpt")) {
            setTimeout(gpt_reply, 500);
        }

        return ctx.redirect("/chat");
    }
};
