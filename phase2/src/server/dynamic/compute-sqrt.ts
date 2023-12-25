import { HTTPContext } from "../context";

export async function compute_sqrt(ctx: HTTPContext) {
    ctx.write_headers(200, [["Content-Type", "text/plain"]]);

    // a compute intensive task
    let x = 0;
    for (let i = 0; i < 100_000_000; i++) {
        x += Math.sqrt(i);
    }

    ctx.write_body(`Sum of square roots from 0 to 100,000,000 is ${x}\n`);
    ctx.end_body();
}
