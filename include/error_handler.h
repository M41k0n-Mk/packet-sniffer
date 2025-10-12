#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <stdio.h>

/**
 * Print an error message to stderr and exit the program
 * 
 * @param message Error message to display
 */
void fatal_error(const char *message);

/**
 * Print a warning message to stderr
 * 
 * @param message Warning message to display
 */
void print_warning(const char *message);

/**
 * Print an informational message to stdout
 * 
 * @param message Information message to display
 */
void print_info(const char *message);

#endif /* ERROR_HANDLER_H */
