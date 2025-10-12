#include "../include/packet_capture.h"
#include "../include/packet_parser.h"
#include "../include/error_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Platform-specific includes */
#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <netinet/in.h>
#endif

#define SNAP_LEN 65535
#define PROMISCUOUS_MODE 1
#define TIMEOUT_MS 1000

char* get_default_device(void) {
    char errbuf[PCAP_ERRBUF_SIZE];
    char *device;
    
#ifdef _WIN32
    pcap_if_t *all_devices;
    pcap_if_t *d;
    
    /* Get list of available devices */
    if (pcap_findalldevs(&all_devices, errbuf) == -1) {
        fprintf(stderr, "Error finding devices: %s\n", errbuf);
        return NULL;
    }
    
    /* Use first available device */
    if (all_devices == NULL) {
        fprintf(stderr, "No devices found\n");
        return NULL;
    }
    
    device = strdup(all_devices->name);
    pcap_freealldevs(all_devices);
#else
    /* Use pcap_findalldevs for Unix-like systems (pcap_lookupdev is deprecated) */
    pcap_if_t *all_devices;
    
    if (pcap_findalldevs(&all_devices, errbuf) == -1) {
        fprintf(stderr, "Error finding devices: %s\n", errbuf);
        fprintf(stderr, "Try running with sudo/administrator privileges\n");
        return NULL;
    }
    
    if (all_devices == NULL) {
        fprintf(stderr, "No devices found\n");
        fprintf(stderr, "Try running with sudo/administrator privileges\n");
        return NULL;
    }
    
    device = strdup(all_devices->name);
    pcap_freealldevs(all_devices);
#endif
    
    return device;
}

pcap_t* initialize_capture(const char *device, const char *filter_expression) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    struct bpf_program filter;
    bpf_u_int32 net_mask;
    bpf_u_int32 net_ip;
    
    /* Get network address and mask */
    if (pcap_lookupnet(device, &net_ip, &net_mask, errbuf) == -1) {
        print_warning("Could not get netmask for device");
        net_ip = 0;
        net_mask = 0;
    }
    
    /* Open capture device */
    handle = pcap_open_live(device, SNAP_LEN, PROMISCUOUS_MODE, TIMEOUT_MS, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Error opening device %s: %s\n", device, errbuf);
        return NULL;
    }
    
    printf("Successfully opened device: %s\n", device);
    
    /* Compile and apply filter if provided */
    if (filter_expression != NULL) {
        if (pcap_compile(handle, &filter, filter_expression, 0, net_ip) == -1) {
            fprintf(stderr, "Error compiling filter: %s\n", pcap_geterr(handle));
            pcap_close(handle);
            return NULL;
        }
        
        if (pcap_setfilter(handle, &filter) == -1) {
            fprintf(stderr, "Error setting filter: %s\n", pcap_geterr(handle));
            pcap_freecode(&filter);
            pcap_close(handle);
            return NULL;
        }
        
        printf("Filter applied: %s\n", filter_expression);
        pcap_freecode(&filter);
    }
    
    return handle;
}

void start_capture(pcap_t *handle, int packet_count) {
    print_info("Starting packet capture...");
    print_info("Press Ctrl+C to stop");
    printf("\n");
    
    /* Start packet capture loop */
    int result = pcap_loop(handle, packet_count, process_packet, NULL);
    
    if (result == -1) {
        fprintf(stderr, "Error during packet capture: %s\n", pcap_geterr(handle));
    } else if (result == -2) {
        print_info("Packet capture stopped by user");
    } else {
        print_info("Packet capture completed");
    }
}

void cleanup_capture(pcap_t *handle) {
    if (handle != NULL) {
        pcap_close(handle);
        print_info("Capture session closed");
    }
}
