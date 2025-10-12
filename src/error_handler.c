#include "../include/error_handler.h"
#include <stdlib.h>
#include <string.h>

void fatal_error(const char *message) {
    fprintf(stderr, "ERROR: %s\n", message);
    exit(EXIT_FAILURE);
}

void print_warning(const char *message) {
    fprintf(stderr, "WARNING: %s\n", message);
}

void print_info(const char *message) {
    printf("INFO: %s\n", message);
}
