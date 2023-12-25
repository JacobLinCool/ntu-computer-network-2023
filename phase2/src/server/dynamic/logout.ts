import { HTTPContext } from "../context";
import { Router } from "../router";
import { user } from "../data";
import { webcrypto } from "node:crypto";

export const logout = (router: Router) => async (ctx: HTTPContext) => {
    const cookies = ctx.cookies;
    const uid = cookies.user;
    const token = cookies.token;
    const u = user.get(uid);

    if (u && token === u.token) {
        user.set(uid, { ...u, token: undefined });
    }

    return ctx.redirect("/login", [
        ["Set-Cookie", `user=; Path=/; HttpOnly`],
        ["Set-Cookie", `token=; Path=/; HttpOnly`],
    ]);
};
