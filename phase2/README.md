# Phase 2

Use only Node.js built-in modules.

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
