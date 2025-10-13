# Makefile for Packet Sniffer

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
LDFLAGS = -lpcap

# Target binary
TARGET = packet_sniffer

# Find all .c files in src/ and subdirectories
SOURCES := $(shell find src -name '*.c')
OBJECTS := $(SOURCES:.c=.o)

# Default target
.PHONY: all
all:
	@echo "=================================================="
	@echo "Packet Sniffer - Build"
	@echo "=================================================="
	@if [ -z "$(SOURCES)" ]; then \
		echo "Note: No source files are currently present in the repository."; \
		echo "This is a placeholder Makefile for CI/CD workflow compatibility."; \
		echo ""; \
		echo "Once source files are added, this Makefile will compile them into"; \
		echo "the '$(TARGET)' binary using libpcap."; \
		echo ""; \
		echo "Build completed successfully (placeholder mode)."; \
		echo "=================================================="; \
	else \
		$(MAKE) $(TARGET); \
	fi

# Build the target (when sources are available)
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete."

# Install target (placeholder)
.PHONY: install
install:
	@echo "Install target not yet implemented."
	@echo "This will be configured once the project has source code."

# Run tests (placeholder)
.PHONY: test
test:
	@echo "Running tests..."
	@echo "No tests are currently configured."
	@echo "Tests will be added as the project develops."

# Help target
.PHONY: help
help:
	@echo "Packet Sniffer Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  all      - Build the packet sniffer (default target)"
	@echo "  clean    - Remove build artifacts"
	@echo "  install  - Install the binary (not yet implemented)"
	@echo "  test     - Run tests (not yet implemented)"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Current status: Ready for source files in src/"