#include <stdio.h>

#define CFLAG_IMPLEMENTATION
#include "cflag.h"

int main(int argc, char **argv)
{
    // define some flags

    //                     name  description                   default 
    bool *help = cflag_bool("-h", "Prints this help menu", false);
    int *iter = cflag_int("-i", "Print all integers up to i", 0);
    uint64_t *number = cflag_uint64("-n", "A uint64_t number which is printed before exiting the program", 0);
    float *number2 = cflag_float("-n2", "A floating point number which is printed before exiting the program", 0.f);
    char **printme = cflag_string("-s", "A string which is printed to stdout stream", NULL);

    // parse all flags and check for errors
    if (!cflag_parse(argc, argv)) {
        cflag_log_error(stdout);
        printf("Usage: %s [OPTIONS]\n", *argv);
		printf("Options:\n");
        cflag_log_options(stdout, true);
        return 1;
    }

    // the pointers now store the values that were passed to the program

    // e.g. print a help screen if the -h flag was passed
    // flags with name, description and optionally default values are formated
    // and printed automatically in the cflag_log_options() function
    if (*help) {
        printf("Usage: %s [OPTIONS]\n", *argv);
		printf("Options:\n");
        cflag_log_options(stdout, true);
        return 0;
    }

    // process other values...

    if (*iter > 0) {
        for (int i = 0; i < *iter + 1; ++i) {
            printf("%d\n", i);
        }
    } else if (*iter < 0) {
        for (int i = 0; i > *iter - 1; --i) {
            printf("%d\n", i);
        }
    }

    if (*number != 0)     printf("n  = %lu\n", *number);
    if (*number2 != 0.f)  printf("n2 = %.*f\n", 38, *number2);
    if (*printme != NULL) printf("s  = %s\n", *printme);

    return 0;
}