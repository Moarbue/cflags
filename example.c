#include <stdio.h>
 
#define CARGS_IMPLEMENTATION
#include "cargs.h"
 
int main(int argc, char **argv)
{
    // 1. Reference-based binding: variables are declared here, and the library updates them directly.
    bool verbose = false;
    cargs_bool_ref("-v", "Enable verbose output", &verbose, false);
 
    int max_lines = 100;
    cargs_int_ref("-l", "Maximum number of lines to analyze", &max_lines, 100);
 
    float threshold = 0.5f;
    cargs_float_ref("-t", "Error threshold for alerts", &threshold, 0.5f);
 
    // 2. Positional arguments
    char **input_file = cargs_positional("input", "Path to the log file to analyze", true);
    char **log_level = cargs_positional("level", "Filter by log level (INFO, WARN, ERROR)", false);
 
    // 3. Pointer-based binding: memory is managed by the library.
    char **config_file = cargs_string("-c", "Path to config file", "config.ini");
 
    // Parse flags and check for errors.
    if (!cargs_parse(argc, argv)) {
        cargs_log_error(stderr);
        printf("\nUsage: %s [OPTIONS] <input> [level]\n", argv[0]);
        printf("Options:\n");
        cargs_log_options(stdout, true);
        return 1;
    }
 
    // Print help if verbose is requested
    if (verbose) {
        printf("[INFO] Starting log analysis...\n");
        printf("[INFO] File: %s\n", *input_file);
        printf("[INFO] Level: %s\n", *log_level ? *log_level : "INFO");
        printf("[INFO] Max Lines: %d\n", max_lines);
        printf("[INFO] Threshold: %.2f\n", threshold);
        printf("[INFO] Config: %s\n", *config_file);
    }
 
    printf("Analyzing %s for level %s (Max: %d, Threshold: %.2f)...\n", 
           *input_file, *log_level ? *log_level : "INFO", max_lines, threshold);
 
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
