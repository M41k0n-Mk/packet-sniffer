#include "../include/packet_parser.h"
#include "../include/error_handler.h"
#include <stdio.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include <string.h>

/* Protocol numbers */
#define PROTOCOL_ICMP 1
#define PROTOCOL_TCP 6
#define PROTOCOL_UDP 17

/* Ethernet types */
#define ETHER_TYPE_IP 0x0800
#define ETHER_TYPE_ARP 0x0806
#define ETHER_TYPE_IPV6 0x86DD

static int packet_counter = 0;

void process_packet(u_char *user, const struct pcap_pkthdr *packet_header, 
                   const u_char *packet_data) {
    (void)user; /* Unused parameter */
    packet_counter++;
    
    printf("\n========================================\n");
    printf("Packet #%d\n", packet_counter);
    printf("Capture Length: %d bytes\n", packet_header->caplen);
    printf("Total Length: %d bytes\n", packet_header->len);
    printf("========================================\n");
    
    /* Parse Ethernet header */
    parse_ethernet_header(packet_data);
    
    /* Get Ethernet type to determine next layer protocol */
    struct ethernet_header *eth_header = (struct ethernet_header *)packet_data;
    uint16_t ether_type = ntohs(eth_header->ether_type);
    
    /* Parse IP layer if it's an IP packet */
    if (ether_type == ETHER_TYPE_IP) {
        const u_char *ip_packet = packet_data + sizeof(struct ethernet_header);
        uint8_t protocol = parse_ipv4_header(ip_packet);
        
        /* Get IP header length */
        struct ipv4_header *ip_header = (struct ipv4_header *)ip_packet;
        uint8_t ip_header_length = (ip_header->version_ihl & 0x0F) * 4;
        
        /* Parse transport layer */
        const u_char *transport_packet = ip_packet + ip_header_length;
        
        if (protocol == PROTOCOL_TCP) {
            parse_tcp_header(transport_packet);
        } else if (protocol == PROTOCOL_UDP) {
            parse_udp_header(transport_packet);
        } else if (protocol == PROTOCOL_ICMP) {
            printf("\nTransport Layer: ICMP\n");
        }
    } else if (ether_type == ETHER_TYPE_ARP) {
        printf("\nNetwork Layer: ARP\n");
    } else if (ether_type == ETHER_TYPE_IPV6) {
        printf("\nNetwork Layer: IPv6 (parsing not implemented)\n");
    }
}

void parse_ethernet_header(const u_char *packet) {
    struct ethernet_header *eth_header = (struct ethernet_header *)packet;
    
    printf("\n--- Ethernet Header ---\n");
    printf("Source MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
           eth_header->src_mac[0], eth_header->src_mac[1],
           eth_header->src_mac[2], eth_header->src_mac[3],
           eth_header->src_mac[4], eth_header->src_mac[5]);
    printf("Dest MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
           eth_header->dest_mac[0], eth_header->dest_mac[1],
           eth_header->dest_mac[2], eth_header->dest_mac[3],
           eth_header->dest_mac[4], eth_header->dest_mac[5]);
    
    uint16_t ether_type = ntohs(eth_header->ether_type);
    printf("EtherType: 0x%04x", ether_type);
    
    if (ether_type == ETHER_TYPE_IP) {
        printf(" (IPv4)\n");
    } else if (ether_type == ETHER_TYPE_ARP) {
        printf(" (ARP)\n");
    } else if (ether_type == ETHER_TYPE_IPV6) {
        printf(" (IPv6)\n");
    } else {
        printf(" (Unknown)\n");
    }
}

uint8_t parse_ipv4_header(const u_char *packet) {
    struct ipv4_header *ip_header = (struct ipv4_header *)packet;
    
    printf("\n--- IPv4 Header ---\n");
    printf("Version: %d\n", (ip_header->version_ihl >> 4) & 0x0F);
    printf("Header Length: %d bytes\n", (ip_header->version_ihl & 0x0F) * 4);
    printf("TTL: %d\n", ip_header->ttl);
    printf("Protocol: %d", ip_header->protocol);
    
    if (ip_header->protocol == PROTOCOL_TCP) {
        printf(" (TCP)\n");
    } else if (ip_header->protocol == PROTOCOL_UDP) {
        printf(" (UDP)\n");
    } else if (ip_header->protocol == PROTOCOL_ICMP) {
        printf(" (ICMP)\n");
    } else {
        printf(" (Unknown)\n");
    }
    
    printf("Source IP: %d.%d.%d.%d\n",
           ip_header->src_ip[0], ip_header->src_ip[1],
           ip_header->src_ip[2], ip_header->src_ip[3]);
    printf("Dest IP: %d.%d.%d.%d\n",
           ip_header->dest_ip[0], ip_header->dest_ip[1],
           ip_header->dest_ip[2], ip_header->dest_ip[3]);
    
    return ip_header->protocol;
}

void parse_tcp_header(const u_char *packet) {
    struct tcp_header *tcp_hdr = (struct tcp_header *)packet;
    
    printf("\n--- TCP Header ---\n");
    printf("Source Port: %d\n", ntohs(tcp_hdr->src_port));
    printf("Dest Port: %d\n", ntohs(tcp_hdr->dest_port));
    printf("Sequence Number: %u\n", ntohl(tcp_hdr->seq_number));
    printf("Acknowledgment Number: %u\n", ntohl(tcp_hdr->ack_number));
    
    /* Parse TCP flags */
    uint8_t flags = tcp_hdr->flags;
    printf("Flags: ");
    if (flags & 0x01) printf("FIN ");
    if (flags & 0x02) printf("SYN ");
    if (flags & 0x04) printf("RST ");
    if (flags & 0x08) printf("PSH ");
    if (flags & 0x10) printf("ACK ");
    if (flags & 0x20) printf("URG ");
    printf("\n");
}

void parse_udp_header(const u_char *packet) {
    struct udp_header *udp_hdr = (struct udp_header *)packet;
    
    printf("\n--- UDP Header ---\n");
    printf("Source Port: %d\n", ntohs(udp_hdr->src_port));
    printf("Dest Port: %d\n", ntohs(udp_hdr->dest_port));
    printf("Length: %d bytes\n", ntohs(udp_hdr->length));
    printf("Checksum: 0x%04x\n", ntohs(udp_hdr->checksum));
}
