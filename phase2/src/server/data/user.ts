import fs from "node:fs";

export interface UserData {
    id: string;
    password: string;
    token?: string;
}

const fp = "data/user.json";

export function get_user(id: string): UserData | undefined {
    return retrieve()[id];
}

export function set_user(id: string, user: UserData) {
    const data = retrieve();
    if (!data[id]) {
        data[id] = user;
    } else {
        Object.assign(data[id], user);
    }
    persist(data);
}

function retrieve(): Record<string, UserData> {
    if (fs.existsSync(fp)) {
        const raw = fs.readFileSync(fp);
        return JSON.parse(raw.toString());
    } else {
        return {};
    }
}

function persist(data: Record<string, UserData>) {
    if (!fs.existsSync("data")) {
        fs.mkdirSync("data");
    }
    fs.writeFileSync(fp, JSON.stringify(data));
}
