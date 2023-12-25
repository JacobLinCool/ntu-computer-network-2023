import { server } from "./server";
import cluster from "node:cluster";
import { availableParallelism } from "node:os";
import process from "node:process";

const PORT = Number(process.env.PORT) || 8080;

if (cluster.isPrimary) {
    console.log(`Primary node (pid ${process.pid}) is running`);

    const childs = Math.min(availableParallelism(), Number(process.env.MAX_WORKERS) || 4);
    console.log(`Will spawn ${childs} worker nodes`);

    for (let i = 0; i < childs; i++) {
        cluster.fork();
    }

    console.log(`Using port ${PORT}`);

    let leaving = false;

    cluster.on("exit", (worker, code, signal) => {
        console.log(`Worker node (pid ${worker.process.pid}) exited with code ${code} and signal ${signal}`);
        if (!leaving) {
            cluster.fork();
        }
    });

    process.on("SIGINT", () => {
        if (leaving) {
            console.log("Force quitting");
            process.exit(1);
        } else {
            console.log("Gracefully shutting down");
            leaving = true;
            cluster.disconnect();
        }
    });
} else {
    server.listen(PORT);
    console.log(`Worker node (pid ${process.pid}) is running `);
}
