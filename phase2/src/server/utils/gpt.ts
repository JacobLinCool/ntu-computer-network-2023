import { chat } from "../data";

export async function gpt_reply() {
    if (process.env.OPENAI_API_KEY === undefined) {
        return;
    }

    const chats = chat.get(-10);
    if (chats.length === 0) {
        return;
    }

    const messages = chats.map((c) => ({
        role: c.user === "GPT" ? "assistant" : "user",
        content: c.user === "GPT" ? c.message : `[${c.user}] ${c.message}`,
    }));

    messages.unshift({
        role: "system",
        content:
            "You are a chatbot in an online chatroom of Computer Network class. You are a humorous and curious bot named ProjectGPT. You are created by Jacob Lin, who is a NTNU student attending the Computer Network course.",
    });

    const res = await fetch("https://api.openai.com/v1/chat/completions", {
        method: "POST",
        headers: {
            Authorization: `Bearer ${process.env.OPENAI_API_KEY}`,
            "Content-Type": "application/json",
        },
        body: JSON.stringify({
            model: "gpt-3.5-turbo-1106",
            max_tokens: 512,
            messages,
        }),
    });

    const json = await res.json();
    console.log(json.usage);

    const reply = json.choices[0].message.content;
    console.log(reply);

    chat.add({ user: "GPT", message: reply, timestamp: Date.now() });
}
