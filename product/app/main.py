from fastapi import FastAPI
from pydantic import BaseModel
import python_module


class TextIn(BaseModel):
    text: str


app = FastAPI(title="new-NLP Product API")


@app.post("/embed")
def embed_endpoint(inp: TextIn):
    # returns a small embedding vector (fallback/simple or from rust)
    vec = python_module.embed(inp.text)
    return {"embedding": vec}


@app.post("/analyze")
def analyze_endpoint(inp: TextIn):
    return {"analysis": python_module.analyze(inp.text)}


@app.get("/health")
def health():
    return {"status": "ok"}
