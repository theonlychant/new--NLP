FROM python:3.12-slim
WORKDIR /app

# Copy repository into image so product can import local `python_module`
COPY . /workspace
ENV PYTHONPATH=/workspace

RUN pip install --no-cache-dir -r /workspace/product/requirements.txt

EXPOSE 8000
CMD ["uvicorn","product.app.main:app","--host","0.0.0.0","--port","8000"]
