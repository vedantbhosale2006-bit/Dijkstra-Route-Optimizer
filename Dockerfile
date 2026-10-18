FROM debian:bookworm-slim
WORKDIR /app
COPY router /app/router
RUN chmod +x /app/router
CMD ["./router"]