import { HTTPContext } from "../context";
import { Router } from "../router";
import { user } from "../data";
import { webcrypto } from "node:crypto";

export const login = (router: Router) => async (ctx: HTTPContext) => {
    const cookies = ctx.cookies;
    const uid = cookies.user;
    const u = user.get(uid);

    if (u) {
        return ctx.redirect("/chat");
    }

    if (ctx.method === "GET") {
        return router.serve_static(ctx.path.pathname, ctx);
    }

    if (ctx.method === "POST") {
        const form = ctx.form;
        const username = form.username;
        const password = form.password;

        const u = user.get(username);
        if (!u) {
            return ctx.redirect("/register?msg=User not found");
        }

        const hash = await webcrypto.subtle.digest("SHA-256", Buffer.from(password));
        const hex = Buffer.from(hash).toString("hex");
        if (u.password !== hex) {
            return ctx.redirect("/login?msg=Incorrect password");
        }

        const token = Buffer.from(webcrypto.randomUUID()).toString("hex");
        user.set(username, { ...u, token: token });

        return ctx.redirect("/chat", [
            ["Set-Cookie", `user=${username}; Path=/; HttpOnly`],
            ["Set-Cookie", `token=${token}; Path=/; HttpOnly`],
        ]);
    }

    return ctx.not_found();
};
