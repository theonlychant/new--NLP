FROM mcr.microsoft.com/devcontainers/base:ubuntu

USER root

# Install common dev tools: python3, pip, node, npm, go, build tools
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
    build-essential curl ca-certificates git python3 python3-pip python3-venv \
    nodejs npm golang-go pkg-config libssl-dev \
 && rm -rf /var/lib/apt/lists/*

# Install rustup (non-interactive)
RUN curl https://sh.rustup.rs -sSf | sh -s -- -y || true

USER vscode

ENV PATH="/home/vscode/.cargo/bin:${PATH}"
