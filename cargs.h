#ifndef CARGS_H
#define CARGS_H

#ifndef CARGSDEF
#   ifdef CARGS_STATIC
#       define CARGSDEF static
#   else
#       define CARGSDEF extern
#   endif
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// validation function type, which is called when parsing the argument of a flag
typedef bool (validation_func)(const char *name, const char *value);

typedef enum {
    CARGS_OK,
    CARGS_INVALID_ARGUMENT,
    CARGS_MISSING_ARGUMENT,
    CARGS_UNKNOWN_FLAG,
    CARGS_UNKNOWN_SUBCOMMAND,
} cargs_error;


CARGSDEF void cargs_int(const char *long_name, const char short_name, int8_t *reference, const int8_t default_value, validation_func *validate, const char *description);
CARGSDEF void cargs_uint(const char *long_name, const char short_name, uint8_t *reference, const uint8_t default_value, validation_func *validate, const char *description);
CARGSDEF void cargs_bool(const char *long_name, const char short_name, bool *reference, const bool default_value, validation_func *validate, const char *description);
CARGSDEF void cargs_float(const char *long_name, const char short_name, float *reference, const float default_value, validation_func *validate, const char *description);
CARGSDEF void cargs_char(const char *long_name, const char short_name, char *reference, const char default_value, validation_func *validate, const char *description);
CARGSDEF void cargs_string(const char *long_name, const char short_name, char **reference, const char *default_value, validation_func *validate, const char *description);

CARGSDEF void cargs_subcommand_start(const char *name, const char *description);
CARGSDEF void cargs_subcommand_end(void);

CARGSDEF void cargs_set_error(cargs_error error, const char *message);
CARGSDEF cargs_error cargs_get_error(void);
CARGSDEF const char *cargs_get_error_message(void);
CARGSDEF void cargs_log_error(FILE *stream);

CARGSDEF int cargs_parse(int argc, char **argv);
CARGSDEF void cargs_reset(void);
CARGSDEF void cargs_print_help(FILE *stream);


#endif // CARGS_H

#ifdef CARGS_IMPLEMENTATION


#endif // CARGS_IMPLEMENTATION
