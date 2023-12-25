import { HTTPContext } from "../context";
import { user, chat } from "../data";

export async function chat_status(ctx: HTTPContext) {
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
        chat.changed()
            .then((changed) => {
                ctx.write_headers(changed ? 200 : 304);
                ctx.end_body();
            })
            .catch((e) => {
                console.error(e);
                ctx.write_headers(500);
                ctx.end_body();
            });
        return;
    }

    ctx.not_found();
}
