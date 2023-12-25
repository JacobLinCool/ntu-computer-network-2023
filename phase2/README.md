# Phase 2

Use only Node.js built-in modules to implement a web server.

## Features

- [x] HTTP/HTTPS Server
  - [x] HTTP parser (see [context.ts](./src/server/context.ts))
  - [x] Persistent Connection (HTTP Keep-Alive) (see [router.ts](./src/server/router.ts))
  - [x] Streaming (Transfer-Encoding: chunked) (see [router.ts](./src/server/router.ts))
  - [x] Static File Server (see [static](./src/server/static)) (see [router.ts](./src/server/router.ts))
  - [x] Self-signed TLS Certificate (see [server](./src/server/index.ts))
- [x] Multi-Proccessing (Cluster) (see [index.ts](./src/index.ts))
  - The Node.js' event loop is good enough for IO-bound tasks.
  - The multi-proccessing is used for CPU-bound tasks. (see [compute-sqrt.ts](./src/server/dynamic/compute-sqrt.ts))
- [x] Account System
  - [x] Cookie-based Session Token (see [login.ts](./src/server/dynamic/login.ts) ...)
- [x] Chatroom
  - [x] GPT-3.5 Turbo Integration (see [gpt.ts](./src/server/utils/gpt.ts))
  - [x] Long Polling based auto-update (see [server](./src/server/dynamic/chat-status.ts) and [client](./site/static/chat.html))
- [x] File based Database (see [data](./src/server/data))
  - [x] User Account (see [user.ts](./src/server/data/user.ts))
  - [x] Chat History (see [chat.ts](./src/server/data/chat.ts))
- [x] Socket Chat Server (see [chat](./src/chat) and [client](./src/client)) _[Phase 1 refactoring]_

## Usage

> If `pnpm` is not installed, use `npm` or `yarn` instead.

### Generate TLS Certificates

_Optional_

```sh
pnpm cert
```

### Install TypeScript

```sh
pnpm i
```

### Run

```sh
pnpm start
```

> If `server.key` and `server.crt` are not found, the server will run in HTTP mode.

## GPT Integration

It has been integrated with GPT-3.5 Turbo.

It will be automatically enabled if `OPENAI_API_KEY` environment variable is set.

Add `@GPT` or `@gpt` to the message front to chat with GPT-3.5 Turbo.
