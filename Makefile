CC = gcc
CFLAGS = -Wall -Wextra -O2 -IC:/npcap/Include
LDFLAGS = -LC:/npcap/Lib -lwpcap -lws2_32
TARGET = packet_sniffer.exe

SOURCES := $(wildcard src/*.c)
OBJECTS := $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo Cleaning build artifacts...
	@if exist "src\*.o" del /Q src\*.o
	@if exist "$(TARGET).exe" del /Q $(TARGET).exe
	@if exist "$(TARGET)" del /Q $(TARGET)
	@echo Clean complete.