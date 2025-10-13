#include "../include/packet_capture.h"
#include "../include/error_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

/* Cross-platform network includes */
#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <pcap.h>

/* Global handle for signal handling */
static pcap_t *global_handle = NULL;

/* List all capture devices */
void list_capture_devices()
{
    pcap_if_t *alldevs, *d;
    char errbuf[PCAP_ERRBUF_SIZE];
    int i = 0;

    if (pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
        return;
    }

    printf("Available capture devices:\n");
    for (d = alldevs; d; d = d->next)
    {
        printf("%d. %s", ++i, d->name);
        if (d->description)
            printf(" (%s)\n", d->description);
        else
            printf(" (No description available)\n");
    }

    pcap_freealldevs(alldevs);

    if (i == 0)
    {
        printf("No capture devices found.\n");
    }
}

/**
 * Signal handler for graceful shutdown
 */
void signal_handler(int signum)
{
    (void)signum; /* Unused parameter */
    printf("\n");
    print_info("Interrupt signal received. Shutting down...");

    if (global_handle != NULL)
    {
        pcap_breakloop(global_handle);
    }
}

/**
 * Display usage information
 */
void print_usage(const char *program_name)
{
    printf("Packet Sniffer - Real-time Network Packet Capture Tool\n\n");
    printf("Usage: %s [OPTIONS]\n\n", program_name);
    printf("Options:\n");
    printf("  -L, --list      List available network interfaces for capture\n");
    printf("  -i <device>     Network interface to capture from (default: auto-detect)\n");
    printf("  -c <count>      Number of packets to capture (default: infinite)\n");
    printf("  -f <filter>     BPF filter expression (e.g., 'tcp port 80')\n");
    printf("  -h              Display this help message\n\n");
    printf("Examples:\n");
    printf("  %s --list                    # List all capture devices\n", program_name);
    printf("  %s                          # Capture on default interface\n", program_name);
    printf("  %s -i <device> -c 100        # Capture 100 packets on selected device\n", program_name);
    printf("  %s -f 'tcp port 443'         # Capture only HTTPS traffic\n", program_name);
    printf("Note: This program requires administrator privileges (run as administrator).\n");
}

int main(int argc, char *argv[])
{
    char *device = NULL;
    char *filter_expression = NULL;
    int packet_count = -1; /* -1 means infinite */
    int auto_detect_device = 1;
    int show_list = 0;

#ifdef _WIN32
    /* Initialize Winsock */
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        fprintf(stderr, "WSAStartup failed\n");
        return EXIT_FAILURE;
    }
#endif

    /* Parse command line arguments */
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            print_usage(argv[0]);
#ifdef _WIN32
            WSACleanup();
#endif
            return EXIT_SUCCESS;
        }
        else if (strcmp(argv[i], "-L") == 0 || strcmp(argv[i], "--list") == 0)
        {
            show_list = 1;
        }
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc)
        {
            device = argv[++i];
            auto_detect_device = 0;
        }
        else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc)
        {
            packet_count = atoi(argv[++i]);
            if (packet_count <= 0)
            {
                fprintf(stderr, "Invalid packet count: %s\n", argv[i]);
#ifdef _WIN32
                WSACleanup();
#endif
                return EXIT_FAILURE;
            }
        }
        else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc)
        {
            filter_expression = argv[++i];
        }
        else
        {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
#ifdef _WIN32
            WSACleanup();
#endif
            return EXIT_FAILURE;
        }
    }

    /* List interfaces and exit if requested */
    if (show_list)
    {
        list_capture_devices();
#ifdef _WIN32
        WSACleanup();
#endif
        return EXIT_SUCCESS;
    }

    /* Set up signal handler for graceful shutdown */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* Get default device if not specified */
    if (auto_detect_device)
    {
        device = get_default_device();
        if (device == NULL)
        {
            fatal_error("Could not find a suitable network device");
        }
        printf("Using default device: %s\n", device);
    }

    /* Initialize packet capture */
    pcap_t *handle = initialize_capture(device, filter_expression);
    if (handle == NULL)
    {
        if (auto_detect_device)
        {
            free(device);
        }
#ifdef _WIN32
        WSACleanup();
#endif
        fatal_error("Failed to initialize packet capture");
    }

    /* Store handle globally for signal handler */
    global_handle = handle;

    /* Start capturing packets */
    start_capture(handle, packet_count);

    /* Cleanup */
    cleanup_capture(handle);

    if (auto_detect_device)
    {
        free(device);
    }

#ifdef _WIN32
    WSACleanup();
#endif

    return EXIT_SUCCESS;
}