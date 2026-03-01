SHELL := /bin/bash
CC ?= gcc
INCLUDE_DIR := include
TARGET_DIR := target
CSRC := c_stub.c

.PHONY: all build-cstub build-rust install-cstub test clean

all: build-cstub

build-cstub: $(CSRC)
	@mkdir -p $(TARGET_DIR)
	$(CC) -shared -fPIC $(CSRC) -I$(INCLUDE_DIR) -o $(TARGET_DIR)/librustlib.so
	@echo "Built C stub shared lib: $(TARGET_DIR)/librustlib.so"

build-rust:
	@if command -v cargo >/dev/null 2>&1; then \
		cargo build --release; \
		cp target/release/librustlib.* $(TARGET_DIR)/ 2>/dev/null || true; \
		echo "Rust build complete (copied to $(TARGET_DIR) if present)."; \
	else \
		echo "cargo not found; skip Rust build."; \
	fi

install-cstub: build-cstub
	@echo "C stub installed to $(TARGET_DIR)/librustlib.so"

test: 
	@echo "Running Rust tests if cargo is available..."
	@if command -v cargo >/dev/null 2>&1; then \
		cargo test --verbose; \
	else \
		echo "cargo not found; Skipping Rust tests."; \
	fi

clean:
	rm -rf $(TARGET_DIR)
