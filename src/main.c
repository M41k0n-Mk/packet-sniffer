#define _GNU_SOURCE  /* For strdup */
#include "../include/packet_capture.h"
#include "../include/error_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ncurses.h>

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
 * Interactive setup for device and filter
 */
void interactive_setup(char **device, char **filter_expression, int *auto_detect_device, int packet_count) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    while (1) {
        // Get list of devices
        char errbuf[PCAP_ERRBUF_SIZE];
        pcap_if_t *all_devices;
        if (pcap_findalldevs(&all_devices, errbuf) == -1) {
            endwin();
            fprintf(stderr, "Error finding devices: %s\n", errbuf);
            exit(EXIT_FAILURE);
        }
        
        // Count devices
        int num_devices = 0;
        for (pcap_if_t *dev = all_devices; dev != NULL; dev = dev->next) {
            num_devices++;
        }
        
        if (num_devices == 0) {
            endwin();
            fprintf(stderr, "No devices found\n");
            pcap_freealldevs(all_devices);
            exit(EXIT_FAILURE);
        }
        
        // Create array of device names
        char **device_names = malloc(num_devices * sizeof(char*));
        char **device_descs = malloc(num_devices * sizeof(char*));
        int i = 0;
        for (pcap_if_t *dev = all_devices; dev != NULL; dev = dev->next) {
            device_names[i] = strdup(dev->name);
            device_descs[i] = dev->description ? strdup(dev->description) : NULL;
            i++;
        }
        pcap_freealldevs(all_devices);
        
        // Menu loop
        int current = 0;
        int ch;
        while (1) {
            clear();
            mvprintw(0, 0, "Select network interface (use arrow keys, Enter to select, q to quit):");
            for (int j = 0; j < num_devices; j++) {
                if (j == current) {
                    attron(A_REVERSE);
                }
                mvprintw(j + 2, 0, "%d. %s", j + 1, device_names[j]);
                if (device_descs[j]) {
                    printw(" (%s)", device_descs[j]);
                }
                if (j == current) {
                    attroff(A_REVERSE);
                }
            }
            refresh();
            
            ch = getch();
            if (ch == KEY_UP) {
                current = (current - 1 + num_devices) % num_devices;
            } else if (ch == KEY_DOWN) {
                current = (current + 1) % num_devices;
            } else if (ch == '\n' || ch == KEY_ENTER) {
                break;
            } else if (ch == 'q' || ch == 'Q') {
                // Free memory
                for (int j = 0; j < num_devices; j++) {
                    free(device_names[j]);
                    if (device_descs[j]) free(device_descs[j]);
                }
                free(device_names);
                free(device_descs);
                endwin();
                exit(0);
            }
        }
        
        // Set device
        free(*device);
        *device = strdup(device_names[current]);
        *auto_detect_device = 0;
        
        // Free memory
        for (int j = 0; j < num_devices; j++) {
            free(device_names[j]);
            if (device_descs[j]) free(device_descs[j]);
        }
        free(device_names);
        free(device_descs);
        
        // Ask for filter
        endwin();
        printf("Selected interface: %s\n", *device);
        printf("Enter BPF filter expression (or press Enter for none): ");
        char input[256];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            exit(EXIT_FAILURE);
        }
        input[strcspn(input, "\n")] = 0;
        free(*filter_expression);
        if (strlen(input) > 0) {
            *filter_expression = strdup(input);
        } else {
            *filter_expression = NULL;
        }
        
        // Start capture
        printf("Starting capture on %s", *device);
        if (*filter_expression) {
            printf(" with filter '%s'", *filter_expression);
        }
        printf("...\n");
        printf("Press Ctrl+C to stop and return to interface selection.\n");
        
        /* Initialize packet capture */
        pcap_t *handle = initialize_capture(*device, *filter_expression);
        if (handle == NULL) {
            fatal_error("Failed to initialize packet capture");
        }
        /* Store handle globally for signal handler */
        global_handle = handle;
        /* Start capturing packets */
        start_capture(handle, packet_count);
        /* Cleanup */
        cleanup_capture(handle);
        global_handle = NULL;
        
        // Loop back to select interface again
    }
    
    endwin();
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
    printf("  -I              Interactive mode: select interface and filter\n");
    printf("  -h              Display this help message\n\n");
    printf("Examples:\n");
    printf("  %s                          # Capture on default interface\n", program_name);
    printf("  %s -i eth0 -c 100           # Capture 100 packets on eth0\n", program_name);
    printf("  %s -f 'tcp port 443'        # Capture only HTTPS traffic\n", program_name);
    printf("  %s -i wlan0 -f 'udp'        # Capture UDP packets on wlan0\n", program_name);
    printf("  %s -I                       # Interactive mode\n\n", program_name);
    printf("Note: This program requires root privileges (run with sudo).\n");
}

int main(int argc, char *argv[]) {
    char *device = NULL;
    char *filter_expression = NULL;
    int packet_count = -1; /* -1 means infinite */
    int auto_detect_device = 1;
    int interactive = 0;
    
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
        } else if (strcmp(argv[i], "-I") == 0) {
            interactive = 1;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }
    
    /* Interactive setup if requested */
    if (interactive) {
        interactive_setup(&device, &filter_expression, &auto_detect_device, packet_count);
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
