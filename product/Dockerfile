FROM python:3.12-slim
WORKDIR /app

# Only copy needed parts of the repo to keep build context small
COPY product/ /workspace/product/
COPY python_module/ /workspace/python_module/
COPY go_module/model.bin /workspace/go_module/model.bin
ENV PYTHONPATH=/workspace

RUN pip install --no-cache-dir -r /workspace/product/requirements.txt

EXPOSE 8000
CMD ["uvicorn","product.app.main:app","--host","0.0.0.0","--port","8000"]
