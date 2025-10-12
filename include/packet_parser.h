#ifndef PACKET_PARSER_H
#define PACKET_PARSER_H

#include <pcap.h>
#include <stdint.h>

/* Ethernet header structure */
struct ethernet_header {
    uint8_t dest_mac[6];
    uint8_t src_mac[6];
    uint16_t ether_type;
};

/* IPv4 header structure */
struct ipv4_header {
    uint8_t version_ihl;
    uint8_t tos;
    uint16_t total_length;
    uint16_t id;
    uint16_t flags_fragment_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint8_t src_ip[4];
    uint8_t dest_ip[4];
};

/* TCP header structure */
struct tcp_header {
    uint16_t src_port;
    uint16_t dest_port;
    uint32_t seq_number;
    uint32_t ack_number;
    uint8_t data_offset_reserved;
    uint8_t flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgent_pointer;
};

/* UDP header structure */
struct udp_header {
    uint16_t src_port;
    uint16_t dest_port;
    uint16_t length;
    uint16_t checksum;
};

/**
 * Callback function for processing captured packets
 * 
 * @param user User-provided data
 * @param packet_header Packet metadata from pcap
 * @param packet_data Raw packet data
 */
void process_packet(u_char *user, const struct pcap_pkthdr *packet_header, 
                   const u_char *packet_data);

/**
 * Parse and display Ethernet header
 * 
 * @param packet Pointer to packet data
 */
void parse_ethernet_header(const u_char *packet);

/**
 * Parse and display IPv4 header
 * 
 * @param packet Pointer to IP header in packet
 * @return Protocol number
 */
uint8_t parse_ipv4_header(const u_char *packet);

/**
 * Parse and display TCP header
 * 
 * @param packet Pointer to TCP header in packet
 */
void parse_tcp_header(const u_char *packet);

/**
 * Parse and display UDP header
 * 
 * @param packet Pointer to UDP header in packet
 */
void parse_udp_header(const u_char *packet);

#endif /* PACKET_PARSER_H */
