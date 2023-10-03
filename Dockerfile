FROM alpine:latest as builder

RUN apk add --no-cache musl-dev gcc make

WORKDIR /app
COPY . .
RUN make clean && make

FROM alpine:latest as runner

WORKDIR /app
COPY --from=builder /app/server .

CMD ["./server"]
