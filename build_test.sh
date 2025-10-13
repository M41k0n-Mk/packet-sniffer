#!/bin/bash
# Test build script for Linux-only packet sniffer

echo "==============================================="
echo "Packet Sniffer - Linux Build Test"
echo "==============================================="

# Check if libpcap is installed
echo "Checking libpcap installation..."
if ! ldconfig -p | grep -q libpcap; then
    echo "WARNING: libpcap may not be installed"
    echo "Install with: sudo apt-get install libpcap-dev"
fi

# Clean previous build
echo "Cleaning previous build..."
make clean

# Build the project
echo "Building packet sniffer..."
if make; then
    echo "✅ Build successful!"
    echo "Binary created: ./packet_sniffer"
    echo ""
    echo "Usage:"
    echo "  sudo ./packet_sniffer          # Basic usage"
    echo "  sudo ./packet_sniffer -h       # Show help"
    echo "  sudo ./packet_sniffer -i eth0  # Specify interface"
    echo ""
    echo "Note: Requires root privileges to capture packets"
else
    echo "❌ Build failed!"
    exit 1
fi