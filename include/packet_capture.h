#ifndef PACKET_CAPTURE_H
#define PACKET_CAPTURE_H

#include <pcap.h>

/**
 * Initialize packet capture session
 * 
 * @param device Network device name (NULL for default device)
 * @param filter_expression BPF filter expression (NULL for no filter)
 * @return pcap handle on success, NULL on failure
 */
pcap_t* initialize_capture(const char *device, const char *filter_expression);

/**
 * Start capturing packets
 * 
 * @param handle pcap handle from initialize_capture
 * @param packet_count Number of packets to capture (-1 for infinite)
 */
void start_capture(pcap_t *handle, int packet_count);

/**
 * Cleanup and close capture session
 * 
 * @param handle pcap handle to close
 */
void cleanup_capture(pcap_t *handle);

/**
 * Get the default network device
 * 
 * @return Device name on success, NULL on failure
 */
char* get_default_device(void);

#endif /* PACKET_CAPTURE_H */
