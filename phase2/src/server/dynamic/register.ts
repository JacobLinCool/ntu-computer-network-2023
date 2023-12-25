import { HTTPContext } from "../context";
import { Router } from "../router";
import { user } from "../data";
import { webcrypto } from "node:crypto";

export const register = (router: Router) => async (ctx: HTTPContext) => {
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

        if (username.length < 3 || username.length > 32) {
            return ctx.redirect("/register?msg=Username must be 3 ~ 32 characters");
        }

        if (password.length < 8 || password.length > 64) {
            return ctx.redirect("/register?msg=Password must be 8 ~ 64 characters");
        }

        if (username === "GPT") {
            return ctx.redirect("/register?msg=You cannot register as GPT");
        }

        const u = user.get(username);
        if (u) {
            return ctx.redirect("/login?msg=User already exists");
        }

        const hash = await webcrypto.subtle.digest("SHA-256", Buffer.from(password));
        const hex = Buffer.from(hash).toString("hex");
        user.set(username, { id: username, password: hex });

        return ctx.redirect("/login");
    }

    return ctx.not_found();
};
