#include "../include/packet_capture.h"
#include "../include/error_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

/* Global handle for signal handling */
static pcap_t *global_handle = NULL;

/**
 * Signal handler for graceful shutdown
 */
void signal_handler(int signum) {
    (void)signum; /* Unused parameter */
    printf("\n");
    print_info("Interrupt signal received. Shutting down...");
    
    if (global_handle != NULL) {
        pcap_breakloop(global_handle);
    }
}

/**
 * Display usage information
 */
void print_usage(const char *program_name) {
    printf("Packet Sniffer - Real-time Network Packet Capture Tool\n\n");
    printf("Usage: %s [OPTIONS]\n\n", program_name);
    printf("Options:\n");
    printf("  -i <device>     Network interface to capture from (default: auto-detect)\n");
    printf("  -c <count>      Number of packets to capture (default: infinite)\n");
    printf("  -f <filter>     BPF filter expression (e.g., 'tcp port 80')\n");
    printf("  -h              Display this help message\n\n");
    printf("Examples:\n");
    printf("  %s                          # Capture on default interface\n", program_name);
    printf("  %s -i eth0 -c 100           # Capture 100 packets on eth0\n", program_name);
    printf("  %s -f 'tcp port 443'        # Capture only HTTPS traffic\n", program_name);
    printf("  %s -i wlan0 -f 'udp'        # Capture UDP packets on wlan0\n\n", program_name);
    printf("Note: This program requires root privileges (run with sudo).\n");
}

int main(int argc, char *argv[]) {
    char *device = NULL;
    char *filter_expression = NULL;
    int packet_count = -1; /* -1 means infinite */
    int auto_detect_device = 1;
    
    /* Parse command line arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            device = argv[++i];
            auto_detect_device = 0;
        } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            packet_count = atoi(argv[++i]);
            if (packet_count <= 0) {
                fprintf(stderr, "Invalid packet count: %s\n", argv[i]);
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            filter_expression = argv[++i];
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }
    
    /* Set up signal handler for graceful shutdown */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    /* Get default device if not specified */
    if (auto_detect_device) {
        device = get_default_device();
        if (device == NULL) {
            fatal_error("Could not find a suitable network device");
        }
        printf("Using default device: %s\n", device);
    }
    
    /* Initialize packet capture */
    pcap_t *handle = initialize_capture(device, filter_expression);
    if (handle == NULL) {
        if (auto_detect_device) {
            free(device);
        }
        fatal_error("Failed to initialize packet capture");
    }
    
    /* Store handle globally for signal handler */
    global_handle = handle;
    
    /* Start capturing packets */
    start_capture(handle, packet_count);
    
    /* Cleanup */
    cleanup_capture(handle);
    
    if (auto_detect_device) {
        free(device);
    }
    
    return EXIT_SUCCESS;
}
