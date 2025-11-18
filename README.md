# Packet Sniffer

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)

## Detailed Explanation
Packet Sniffer is a real-time network packet capture and analysis tool written in C. It captures packets that traverse a network interface and provides insights into protocol usage, data transfer, and network performance. The tool supports basic protocol decoding for Ethernet, IPv4, TCP, and UDP packets.

## Features
- Real-time packet capture using libpcap
- Protocol decoding (Ethernet, IPv4, TCP, UDP)
- Linux support
- BPF filter support for targeted packet capture
- Clean, modular C code following best practices
- Configurable packet count and network interface selection

## Project Structure

The repository is organized as follows:

```
packet-sniffer/
├── src/              # Source code (.c files)
├── include/          # Header files (.h files)
├── docs/             # Documentation and guides
├── samples/          # Sample packet capture files
├── README.md         # This file
├── ROADMAP.md        # Project roadmap and planned features
├── CONTRIBUTING.md   # Contribution guidelines
└── LICENSE           # MIT License
```

Each directory contains a README.md with specific information about its purpose and usage.

## Dependencies

- **libpcap-dev**: Packet capture library
  ```bash
  # Ubuntu/Debian
  sudo apt-get install libpcap-dev
  
  # Fedora/RHEL
  sudo dnf install libpcap-devel
  
  # Arch Linux
  sudo pacman -S libpcap
  ```
- **GCC**: C compiler (usually pre-installed)
- **Make**: Build automation tool

## Building from Source

```bash
# Clone the repository
git clone https://github.com/M41k0n-Mk/packet-sniffer.git
cd packet-sniffer

# Build the project
make

chmod +x build_test.sh
./build_test.sh

# The binary will be created as 'packet_sniffer'
```

## Usage

**Important:** This program requires root privileges to capture packets.

### Basic Usage
```bash
# Run with sudo privileges
sudo ./packet_sniffer
```

### Command Line Options
```
Usage: packet_sniffer [OPTIONS]

Options:
  -i <device>     Network interface to capture from (default: auto-detect)
  -c <count>      Number of packets to capture (default: infinite)
  -f <filter>     BPF filter expression (e.g., 'tcp port 80')
  -h              Display help message
```

### Examples
```bash
# Capture packets on default interface (press Ctrl+C to stop)
sudo ./packet_sniffer

# Capture 100 packets on eth0
sudo ./packet_sniffer -i eth0 -c 100

# Capture only HTTPS traffic
sudo ./packet_sniffer -f 'tcp port 443'

# Capture UDP packets on wlan0
sudo ./packet_sniffer -i wlan0 -f 'udp'

# Capture HTTP traffic on specific interface
sudo ./packet_sniffer -i eth0 -f 'tcp port 80'
```

### BPF Filter Examples
BPF (Berkeley Packet Filter) allows you to filter packets based on various criteria:

- `tcp` - Capture only TCP packets
- `udp` - Capture only UDP packets
- `icmp` - Capture only ICMP packets
- `port 80` - Capture traffic on port 80
- `host 192.168.1.1` - Capture traffic to/from specific IP
- `net 192.168.1.0/24` - Capture traffic on specific subnet
- `tcp and port 443` - Capture TCP traffic on port 443
- `not arp` - Exclude ARP packets

## Troubleshooting

### Permission Denied
If you get a permission error, make sure to run the program with root privileges:
```bash
sudo ./packet_sniffer

# Or add capabilities to avoid sudo
sudo setcap cap_net_raw,cap_net_admin=eip ./packet_sniffer
./packet_sniffer
```

### No Devices Found
- Check if you have the necessary permissions and network interfaces are up
- Try listing available interfaces manually:
  ```bash
  ip link show
  ifconfig -a
  ```

## Roadmap

For detailed information about planned features and development phases, see [ROADMAP.md](ROADMAP.md).

Summary:
- **Phase 1**: Initial release with basic packet capture functionality.
- **Phase 2**: Add support for more protocols.
- **Phase 3**: Implement advanced analysis features.

## Code Structure

The project follows clean code principles with modular design:

```
src/
├── main.c              # Entry point and command-line parsing
├── packet_capture.c    # Packet capture initialization and control
├── packet_parser.c     # Protocol parsing and display
└── error_handler.c     # Error handling and logging utilities

include/
├── packet_capture.h    # Packet capture API
├── packet_parser.h     # Protocol parsing API
└── error_handler.h     # Error handling API
```

### Clean Code Practices Used
- **Single Responsibility**: Each module has a focused purpose
- **Meaningful Names**: Clear, descriptive function and variable names
- **Short Functions**: Functions are concise and focused
- **Error Handling**: All system calls and library functions are checked for errors
- **Documentation**: Functions are documented with clear comments
- **Modularity**: Code is separated into logical modules with clear interfaces
- **Platform Independence**: Preprocessor directives handle platform-specific code

## Theory/Practice References
- [TCP/IP Protocol Suite](https://www.oreilly.com/library/view/tcpip-illustrated/0201633612/)
- [Wireshark](https://www.wireshark.org/)
- [libpcap Documentation](https://www.tcpdump.org/manpages/pcap.3pcap.html)
- [BPF Filter Syntax](https://www.tcpdump.org/manpages/pcap-filter.7.html)
  
## Disclaimer

This project is for educational and personal experimentation only.

- Run all tools in isolated lab environments (VMs or private networks) and never capture or inspect traffic from systems you do not own or have explicit written permission to test.
- Defaults bind to `127.0.0.1` and use toy/insecure parameters — do not use in production.
- Do not commit or publish real private keys, credentials, or personal data.
- Found an issue, improvement or vulnerability? Please fork this repository and submit a pull request (PR) with your fix, or open an issue describing the problem and reproduction steps; maintainers will review and respond.

