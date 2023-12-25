import { set_user, get_user } from "./user";
import { add_chat, changed, get_chat } from "./chat";

export const user = {
    get: get_user,
    set: set_user,
};

export const chat = {
    get: get_chat,
    add: add_chat,
    changed: changed,
};
