import fs from "node:fs";

export interface ChatData {
    user: string;
    message: string;
    timestamp: number;
}

const fp = "data/chat.json";

export function get_chat(from?: number, to?: number): ChatData[] {
    return retrieve().slice(from, to);
}

export function add_chat(chat: ChatData) {
    const data = retrieve();
    data.push(chat);
    persist(data);
}

export function changed(): Promise<boolean> {
    const size = retrieve().length;

    return new Promise((resolve) => {
        // if (!fs.existsSync(fp)) {
        //     setTimeout(() => {
        //         resolve(false);
        //     }, 5_000);
        //     return;
        // }

        // const watcher = fs.watch(fp, () => resolve(true));
        // setTimeout(() => {
        //     resolve(false);
        //     watcher.close();
        // }, 15_000);

        const interval = setInterval(() => {
            if (size !== retrieve().length) {
                resolve(true);
            }
        }, 1000);
        setTimeout(() => {
            resolve(size !== retrieve().length);
            clearInterval(interval);
        }, 15_000);
    });
}

function retrieve(): ChatData[] {
    if (fs.existsSync(fp)) {
        const raw = fs.readFileSync(fp);
        return JSON.parse(raw.toString());
    } else {
        return [];
    }
}

function persist(data: ChatData[]) {
    if (!fs.existsSync("data")) {
        fs.mkdirSync("data");
    }
    fs.writeFileSync(fp, JSON.stringify(data));
}
