# new-NLP Product (Codespace-ready)

This lightweight FastAPI service demonstrates using the `python_module` in this repository.

Quick start (local):

1. Build and run with Docker:

```bash
docker build -t new-nlp-product -f product/Dockerfile .
docker run --rm -p 8000:8000 new-nlp-product
```

2. Call the API:

```bash
curl -X POST http://localhost:8000/embed -H 'Content-Type: application/json' -d '{"text":"hello world"}'
```

Opening this repo in GitHub Codespaces will use `.devcontainer` to preinstall Python, Go and Rust tooling.
