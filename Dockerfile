FROM gcc:latest AS builder
WORKDIR /app
COPY MiniProjectPBLII.cpp .
RUN g++ -O3 -o router MiniProjectPBLII.cpp

FROM debian:bookworm-slim
WORKDIR /app
COPY --from=builder /app/router /app/router

CMD ["./router"]