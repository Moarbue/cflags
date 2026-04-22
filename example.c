#include <stdio.h>

#define CFLAG_IMPLEMENTATION
#include "cflag.h"

int main(int argc, char **argv)
{
    // 1. Reference-based binding: variables are declared here, and the library updates them directly.
    bool verbose = false;
    cflag_bool_ref("-v", "Enable verbose output", &verbose, false);

    int max_lines = 100;
    cflag_int_ref("-l", "Maximum number of lines to analyze", &max_lines, 100);

    float threshold = 0.5f;
    cflag_float_ref("-t", "Error threshold for alerts", &threshold, 0.5f);

    // 2. Pointer-based binding: memory is managed by the library.
    char **input_file = cflag_string("-f", "Path to the log file to analyze", "access.log");
    char **log_level = cflag_string("-L", "Filter by log level (INFO, WARN, ERROR)", "INFO");

    // Parse flags and check for errors.
    if (!cflag_parse(argc, argv)) {
        cflag_log_error(stderr);
        printf("\nUsage: %s [OPTIONS]\n", argv[0]);
        printf("Options:\n");
        cflag_log_options(stdout, true);
        return 1;
    }

    // Print help if verbose is requested or as a separate flag (though here we use -v for demo)
    if (verbose) {
        printf("[INFO] Starting log analysis...\n");
        printf("[INFO] File: %s\n", *input_file);
        printf("[INFO] Level: %s\n", *log_level);
        printf("[INFO] Max Lines: %d\n", max_lines);
        printf("[INFO] Threshold: %.2f\n", threshold);
    }

    printf("Analyzing %s for level %s (Max: %d, Threshold: %.2f)...\n", 
           *input_file, *log_level, max_lines, threshold);

    // Mock analysis logic
    for (int i = 1; i <= max_lines; ++i) {
        if (verbose) printf("Processing line %d...\n", i);
        if (i == 42) {
            printf("  >> Alert: High error rate detected at line %d (above threshold %.2f)!\n", i, threshold);
        }
    }

    printf("Analysis complete.\n");

    return 0;
}
