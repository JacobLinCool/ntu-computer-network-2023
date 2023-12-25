import { HTTPContext } from "../context";

export async function reflect(ctx: HTTPContext) {
    ctx.write_headers(200, [["Content-Type", "application/json"]]);
    ctx.write_body(JSON.stringify(ctx, null, 2));
    ctx.end_body();
}
