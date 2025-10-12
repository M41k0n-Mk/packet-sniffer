# Packet Sniffer

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)

## Detailed Explanation
Packet Sniffer is a tool designed to monitor and analyze network traffic. It captures packets that traverse a network interface and provides insights into protocol usage, data transfer, and network performance.

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

## Usage Instructions
1. Clone the repository:
   ```
   git clone https://github.com/M41k0n-Mk/packet-sniffer.git
   ```
2. Compile the source code:
   ```
   make
   ```
3. Run the application:
   ```
   ./packet_sniffer
   ```

## Roadmap

For detailed information about planned features and development phases, see [ROADMAP.md](ROADMAP.md).

Summary:
- **Phase 1**: Initial release with basic packet capture functionality.
- **Phase 2**: Add support for more protocols.
- **Phase 3**: Implement advanced analysis features.

## Theory/Practice References
- [TCP/IP Protocol Suite](https://www.oreilly.com/library/view/tcpip-illustrated/0201633612/)
- [Wireshark](https://www.wireshark.org/)