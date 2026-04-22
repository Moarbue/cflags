#ifndef CARGS_H
#define CARGS_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define CARGS_STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#elif defined(__cplusplus) && __cplusplus >= 201103L
#define CARGS_STATIC_ASSERT(cond, msg) static_assert(cond, msg)
#else
#define CARGS_STATIC_ASSERT(cond, msg) typedef char static_assertion_##__LINE__[(cond) ? 1 : -1]
#endif



/// \brief Error codes returned by the library during parsing.
enum cargs_errors {
    CARGS_ERROR_NONE,           ///< No error occurred.
    CARGS_ERROR_UNKNOWN,        ///< An unknown flag was provided.
    CARGS_ERROR_NO_VALUE,       ///< A flag requiring a value was provided without one.
    CARGS_ERROR_INVALID_NUMBER, ///< The provided value could not be parsed as a number.
    CARGS_ERROR_OVERFLOW,       ///< The provided number exceeds the maximum representable value.
    CARGS_ERROR_UNDERFLOW,      ///< The provided number is below the minimum representable value.
    CARGS_ERROR_OUT_OF_BOUNDS,  ///< The value is outside the specified range (unused in current version).
    CARGS_ERROR_MISSING_POSITIONAL, ///< A mandatory positional argument was not provided.
 
    CARGS_ERROR_COUNT,

};
CARGS_STATIC_ASSERT(CARGS_ERROR_COUNT == 8, "Exhaustive cargs_error definition!");

/// \brief Contains detailed information about a parsing error.
typedef struct {
    enum cargs_errors error; ///< The type of error that occurred.
    char *flag;              ///< The name of the flag that caused the error.
    char *value;             ///< The value that caused the error (if applicable).
} cargs_error;


#ifdef __cplusplus
extern "C" {
#endif

/// \brief Creates a new boolean flag.
/// \param name  the name of the flag (e.g., "-v")
/// \param desc  a short description of the flag for help output
/// \param def   the default value of the flag
/// \returns a pointer to the flag's value; call cargs_parse() to update it.
bool * cargs_bool(const char *name, const char *desc, bool def);
/// \brief Binds a boolean flag to an external variable.
/// \param name  the name of the flag
/// \param desc  a short description of the flag
/// \param ref   pointer to the external variable to update
/// \param def   the default value to initialize the variable with
void cargs_bool_ref(const char *name, const char *desc, bool *ref, bool def);

/// \brief Creates a new char flag.
/// \param name  the name of the flag
/// \param desc  a short description of the flag
/// \param def   the default value of the flag
/// \returns a pointer to the flag's value.
char * cargs_char(const char *name, const char *desc, char def);
/// \brief Binds a char flag to an external variable.
void cargs_char_ref(const char *name, const char *desc, char *ref, char def);

/// \brief Creates a new int8 flag.
/// \param name  the name of the flag
/// \param desc  a short description of the flag
/// \param def   the default value of the flag
/// \returns a pointer to the flag's value.
int8_t * cargs_int8(const char *name, const char *desc, int8_t def);
/// \brief Binds an int8 flag to an external variable.
void cargs_int8_ref(const char *name, const char *desc, int8_t *ref, int8_t def);
/// \brief Creates a new uint8 flag.
uint8_t * cargs_uint8(const char *name, const char *desc, uint8_t def);
/// \brief Binds a uint8 flag to an external variable.
void cargs_uint8_ref(const char *name, const char *desc, uint8_t *ref, uint8_t def);

/// \brief Creates a new int16 flag.
/// \param name  the name of the flag
/// \param desc  a short description of the flag
/// \param def   the default value of the flag
/// \returns a pointer to the flag's value.
int16_t * cargs_int16(const char *name, const char *desc, int16_t def);
/// \brief Binds an int16 flag to an external variable.
void cargs_int16_ref(const char *name, const char *desc, int16_t *ref, int16_t def);
/// \brief Creates a new uint16 flag.
uint16_t * cargs_uint16(const char *name, const char *desc, uint16_t def);
/// \brief Binds a uint16 flag to an external variable.
void cargs_uint16_ref(const char *name, const char *desc, uint16_t *ref, uint16_t def);

/// \brief Creates a new int32 flag.
/// \param name  the name of the flag
/// \param desc  a short description of the flag
/// \param def   the default value of the flag
/// \returns a pointer to the flag's value.
int32_t * cargs_int32(const char *name, const char *desc, int32_t def);
/// \brief Binds an int32 flag to an external variable.
void cargs_int32_ref(const char *name, const char *desc, int32_t *ref, int32_t def);
/// \brief Creates a new uint32 flag.
uint32_t * cargs_uint32(const char *name, const char *desc, uint32_t def);
/// \brief Binds a uint32 flag to an external variable.
void cargs_uint32_ref(const char *name, const char *desc, uint32_t *ref, uint32_t def);

/// \brief Creates a new int64 flag.
/// \param name  the name of the flag
/// \param desc  a short description of the flag
/// \param def   the default value of the flag
/// \returns a pointer to the flag's value.
int64_t * cargs_int64(const char *name, const char *desc, int64_t def);
/// \brief Binds an int64 flag to an external variable.
void cargs_int64_ref(const char *name, const char *desc, int64_t *ref, int64_t def);
/// \brief Creates a new uint64 flag.
uint64_t * cargs_uint64(const char *name, const char *desc, uint64_t def);
/// \brief Binds a uint64 flag to an external variable.
void cargs_uint64_ref(const char *name, const char *desc, uint64_t *ref, uint64_t def);

/// \brief Creates a new integer flag (width determined by platform).
/// \param name  the name of the flag
/// \param desc  a short description of the flag
/// \param def   the default value of the flag
/// \returns a pointer to the flag's value.
int * cargs_int(const char *name, const char* desc, int def);
/// \brief Binds an integer flag to an external variable.
void cargs_int_ref(const char *name, const char* desc, int *ref, int def);

/// \brief Creates a new size_t flag.
/// \param name  the name of the flag
/// \param desc  a short description of the flag
/// \param def   the default value of the flag
/// \returns a pointer to the flag's value.
size_t * cargs_size_t(const char *name, const char *desc, size_t def);
/// \brief Binds a size_t flag to an external variable.
void cargs_size_t_ref(const char *name, const char *desc, size_t *ref, size_t def);

/// \brief Creates a new floating-point flag.
/// \param name  the name of the flag
/// \param desc  a short description of the flag
/// \param def   the default value of the flag
/// \returns a pointer to the flag's value.
float * cargs_float(const char *name, const char* desc, float def);
/// \brief Binds a float flag to an external variable.
void cargs_float_ref(const char *name, const char* desc, float *ref, float def);
/// \brief Creates a new double flag.
double * cargs_double(const char *name, const char* desc, double def);
/// \brief Binds a double flag to an external variable.
void cargs_double_ref(const char *name, const char* desc, double *ref, double def);
/// \brief Creates a new long double flag.
long double * cargs_long_double(const char *name, const char* desc, long double def);
/// \brief Binds a long double flag to an external variable.
void cargs_long_double_ref(const char *name, const char* desc, long double *ref, long double def);

/// \brief Creates a new string flag.
/// \param name  the name of the flag
/// \param desc  a short description of the flag
/// \param def   the default value of the flag
/// \returns a pointer to the string value.
char ** cargs_string(const char *name, const char* desc, const char *def);
/// \brief Binds a string flag to an external variable.
void cargs_string_ref(const char *name, const char* desc, char **ref, const char *def);
 
/// \brief Creates a new positional argument.
/// \param name  the name of the argument (for help output)
/// \param desc  a short description of the argument
/// \param mandatory whether the argument is mandatory
/// \returns a pointer to the argument's value; call cargs_parse() to update it.
char ** cargs_positional(const char *name, const char *desc, bool mandatory);

/// \brief Parses the flags given to the program and checks for matching flags.
/// The first entry of the argv array (program name) is skipped.
/// \param argc  The number of arguments in argv.
/// \param argv  The array of argument strings.
/// \returns true if parsing was successful, false if an error occurred.
bool cargs_parse(int argc, char **argv);

/// \brief Logs the current parsing error to the specified stream.
/// Only call this function if cargs_parse() returned false.
/// \param stream The output stream (e.g., stderr).
void cargs_log_error(FILE *stream);

/// \brief Returns a structure containing details about the last parsing error.
/// \return A cargs_error struct containing the error type, flag name, and value.
cargs_error cargs_get_error();

/// \brief Prints the registered flags, their descriptions, and default values.
/// \param stream The output stream (e.g., stdout).
/// \param printdefault Whether to print the default values for each flag.
void cargs_log_options(FILE *stream, bool printdefault);
 
 
#ifdef __cplusplus

}
#endif

#endif // CARGS_H

#ifdef CARGS_IMPLEMENTATION

#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum cargs_type {
    CARGS_BOOL,
    CARGS_CHAR,
    CARGS_INT8,
    CARGS_UINT8,
    CARGS_INT16,
    CARGS_UINT16,
    CARGS_INT32,
    CARGS_UINT32,
    CARGS_INT64,
    CARGS_UINT64,
    CARGS_FLOAT,
    CARGS_DOUBLE,
    CARGS_LONG_DOUBLE,
    CARGS_STRING,
    CARGS_SIZE_T,
    CARGS_POSITIONAL,
 
    CARGS_TYPE_COUNT,

};
CARGS_STATIC_ASSERT(CARGS_TYPE_COUNT == 16, "Exhaustive cargs_type definition!");

union cargs_value {
    bool     boolean;
    char     character;
    int8_t   int8;
    uint8_t  uint8;
    int16_t  int16;
    uint16_t uint16;
    int32_t  int32;
    uint32_t uint32;
    int64_t  int64;
    uint64_t uint64;
    float    floating;
    double   double_val;
    long double long_double;
    char *   string;
    size_t   size;
};

struct cargs_flag {
    enum cargs_type type; //value of the enum cargs_type
    const char *name;      // name
    const char *desc;      // short description
    union cargs_value def; // default value
    union cargs_value val; // current value
    void *value_ptr; // pointer to current value (either internal or external)
    bool mandatory; // whether the argument is mandatory (only for positionals)
};

#ifndef CARGS_MAX_FLAGS
#   define CARGS_MAX_FLAGS 128
#endif // CARGS_MAX_FLAGS

static struct cargs_flag cargs__flags[CARGS_MAX_FLAGS];
static uint32_t cargs__count = 0;
static cargs_error cargs__err = { .error = CARGS_ERROR_NONE, .flag = NULL, .value = NULL };

// Forward declarations
static struct cargs_flag *cargs__new(enum cargs_type type, const char *name, const char *desc);
static char * cargs__shift_args(int *argc, char ***argv);
static void cargs__set_error(enum cargs_errors err, const char *flag, char *value);
static int cargs__find_next_positional();
static int cargs__is_flag(const char *arg);
static int cargs__str2int_generic(int64_t *out, char *s, int64_t min, int64_t max);
static int cargs__str2uint_generic(uint64_t *out, char *s, uint64_t min, uint64_t max);
static int cargs__str2float_generic(long double *out, char *s, long double min, long double max);

#define CARGS_REF_IMPL(type_enum, type_name, type_t, field_name) \
void cargs_##type_name##_ref(const char *name, const char *desc, type_t *ref, type_t def) \
{ \
    struct cargs_flag *flag = cargs__new(type_enum, name, desc); \
    flag->def.field_name = def; \
    *ref = def; \
    flag->value_ptr = ref; \
}

CARGS_REF_IMPL(CARGS_BOOL, bool, bool, boolean)
CARGS_REF_IMPL(CARGS_CHAR, char, char, character)
CARGS_REF_IMPL(CARGS_INT8, int8, int8_t, int8)
CARGS_REF_IMPL(CARGS_UINT8, uint8, uint8_t, uint8)
CARGS_REF_IMPL(CARGS_INT16, int16, int16_t, int16)
CARGS_REF_IMPL(CARGS_UINT16, uint16, uint16_t, uint16)
CARGS_REF_IMPL(CARGS_INT32, int32, int32_t, int32)
CARGS_REF_IMPL(CARGS_UINT32, uint32, uint32_t, uint32)
CARGS_REF_IMPL(CARGS_INT64, int64, int64_t, int64)
CARGS_REF_IMPL(CARGS_UINT64, uint64, uint64_t, uint64)
CARGS_REF_IMPL(CARGS_FLOAT, float, float, floating)
CARGS_REF_IMPL(CARGS_DOUBLE, double, double, double_val)

void cargs_long_double_ref(const char *name, const char *desc, long double *ref, long double def)
{
    struct cargs_flag *flag = cargs__new(CARGS_LONG_DOUBLE, name, desc);
    flag->def.long_double = def;
    *ref = def;
    flag->value_ptr = ref;
}

void cargs_int_ref(const char *name, const char* desc, int *ref, int def)
{
#if INT_MAX == INT64_MAX
    cargs_int64_ref(name, desc, (int64_t *)ref, (int64_t)def);
#elif INT_MAX == INT32_MAX
    cargs_int32_ref(name, desc, (int32_t *)ref, (int32_t)def);
#elif INT_MAX == INT16_MAX
    cargs_int16_ref(name, desc, (int16_t *)ref, (int16_t)def);
#elif INT_MAX == INT8_MAX
    cargs_int8_ref(name, desc, (int8_t *)ref, (int8_t)def);
#else
    #error "int size does not match 8/16/32/64 bits"
#endif
}

void cargs_size_t_ref(const char *name, const char *desc, size_t *ref, size_t def)
{
#if SIZE_MAX == UINT64_MAX
    cargs_uint64_ref(name, desc, (uint64_t *)ref, (uint64_t)def);
#elif SIZE_MAX == UINT32_MAX
    cargs_uint32_ref(name, desc, (uint32_t *)ref, (uint32_t)def);
#else
    #error "size_t size is neither 32 nor 64 bits"
#endif
}

void cargs_string_ref(const char *name, const char* desc, char **ref, const char *def)
{
    struct cargs_flag *flag = cargs__new(CARGS_STRING, name, desc);
    flag->def.string = (char *)def;
    *ref = (char *)def;
    flag->value_ptr = ref;
}



// forward declaration of helper functions

static struct cargs_flag *cargs__new(enum cargs_type type, const char *name, const char *desc);
static char * cargs__shift_args(int *argc, char ***argv);
static void cargs__set_error(enum cargs_errors err, const char *flag, char *value);


bool * cargs_bool(const char *name, const char *desc, bool def)
{
    struct cargs_flag *flag = cargs__new(CARGS_BOOL, name, desc);

    flag->def.boolean = def;
    flag->val.boolean = def;

    return (bool *)flag->value_ptr;
}

char * cargs_char(const char *name, const char *desc, char def)
{
    struct cargs_flag *flag = cargs__new(CARGS_CHAR, name, desc);

    flag->def.character = def;
    flag->val.character = def;

    return (char *)flag->value_ptr;
}

int8_t * cargs_int8(const char *name, const char *desc, int8_t def)
{
    struct cargs_flag *flag = cargs__new(CARGS_INT8, name, desc);

    flag->def.int8 = def;
    flag->val.int8 = def;

    return (int8_t *)flag->value_ptr;
}

uint8_t * cargs_uint8(const char *name, const char *desc, uint8_t def)
{
    struct cargs_flag *flag = cargs__new(CARGS_UINT8, name, desc);

    flag->def.uint8 = def;
    flag->val.uint8 = def;

    return (uint8_t *)flag->value_ptr;
}

int16_t * cargs_int16(const char *name, const char *desc, int16_t def)
{
    struct cargs_flag *flag = cargs__new(CARGS_INT16, name, desc);

    flag->def.int16 = def;
    flag->val.int16 = def;

    return (int16_t *)flag->value_ptr;
}

uint16_t * cargs_uint16(const char *name, const char *desc, uint16_t def)
{
    struct cargs_flag *flag = cargs__new(CARGS_UINT16, name, desc);

    flag->def.uint16 = def;
    flag->val.uint16 = def;

    return (uint16_t *)flag->value_ptr;
}

int32_t * cargs_int32(const char *name, const char *desc, int32_t def)
{
    struct cargs_flag *flag = cargs__new(CARGS_INT32, name, desc);

    flag->def.int32 = def;
    flag->val.int32 = def;

    return (int32_t *)flag->value_ptr;
}

uint32_t * cargs_uint32(const char *name, const char *desc, uint32_t def)
{
    struct cargs_flag *flag = cargs__new(CARGS_UINT32, name, desc);

    flag->def.uint32 = def;
    flag->val.uint32 = def;

    return (uint32_t *)flag->value_ptr;
}

int64_t * cargs_int64(const char *name, const char *desc, int64_t def)
{
    struct cargs_flag *flag = cargs__new(CARGS_INT64, name, desc);

    flag->def.int64 = def;
    flag->val.int64 = def;

    return (int64_t *)flag->value_ptr;
}

uint64_t * cargs_uint64(const char *name, const char *desc, uint64_t def)
{
    struct cargs_flag *flag = cargs__new(CARGS_UINT64, name, desc);

    flag->def.uint64 = def;
    flag->val.uint64 = def;

    return (uint64_t *)flag->value_ptr;
}

int * cargs_int(const char *name, const char* desc, int def)
{
#if INT_MAX == INT64_MAX
    return (int *)cargs_int64(name, desc, (int64_t)def);
#elif INT_MAX == INT32_MAX
    return (int *)cargs_int32(name, desc, (int32_t)def);
#elif INT_MAX == INT16_MAX
    return (int *)cargs_int16(name, desc, (int16_t)def);
#elif INT_MAX == INT8_MAX
    return (int *)cargs_int8(name, desc, (int8_t)def);
#else
    #error "int size does not match 8/16/32/64 bits"
#endif
}

size_t * cargs_size_t(const char *name, const char *desc, size_t def)
{
#if SIZE_MAX == UINT64_MAX
    return (size_t *)cargs_uint64(name, desc, (uint64_t)def);
#elif SIZE_MAX == UINT32_MAX
    return (size_t *)cargs_uint32(name, desc, (uint32_t)def);
#else
    #error "size_t size is neither 32 nor 64 bits"
#endif
}

float * cargs_float(const char *name, const char* desc, float def)
{
    struct cargs_flag *flag = cargs__new(CARGS_FLOAT, name, desc);

    flag->def.floating = def;
    flag->val.floating = def;

    return (float *)flag->value_ptr;
}

double * cargs_double(const char *name, const char* desc, double def)
{
    struct cargs_flag *flag = cargs__new(CARGS_DOUBLE, name, desc);

    flag->def.double_val = def;
    flag->val.double_val = def;

    return (double *)flag->value_ptr;
}

long double * cargs_long_double(const char *name, const char* desc, long double def)
{
    struct cargs_flag *flag = cargs__new(CARGS_LONG_DOUBLE, name, desc);

    flag->def.long_double = def;
    flag->val.long_double = def;

    return (long double *)flag->value_ptr;
}

char ** cargs_string(const char *name, const char* desc, const char *def)
{
    struct cargs_flag *flag = cargs__new(CARGS_STRING, name, desc);

    flag->def.string = (char*) def;
    flag->val.string = (char*) def;

    return (char **)flag->value_ptr;
}

char ** cargs_positional(const char *name, const char *desc, bool mandatory)
{
    struct cargs_flag *flag = cargs__new(CARGS_POSITIONAL, name, desc);
     
    flag->mandatory = mandatory;
    flag->val.string = NULL;
    flag->value_ptr = &flag->val.string;

    return (char **)flag->value_ptr;
}
 
bool cargs_parse(int argc, char **argv)
{
    // Remove first entry which is the program's name
    cargs__shift_args(&argc, &argv);

    while (argc > 0) {
        char *flag_name = cargs__shift_args(&argc, &argv);

        uint32_t i;
        for (i = 0; i < cargs__count; ++i) {
            // check if flag_name matches any of the declared flags
            if (strcmp(cargs__flags[i].name, flag_name) == 0) {
                // check type of flag and parse accordingly
                switch (cargs__flags[i].type) {
                    case CARGS_BOOL: {
                        *(bool *)(cargs__flags[i].value_ptr) = true;
                    }
                    break;

                    case CARGS_CHAR: {
                        if (argc == 0 || cargs__is_flag(*argv)) {
                            cargs__set_error(CARGS_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        char *arg = cargs__shift_args(&argc, &argv);
                        *(char *)(cargs__flags[i].value_ptr) = arg[0];
                    }
                    break;

                    case CARGS_INT8: {
                        if (argc == 0 || cargs__is_flag(*argv)) {
                            cargs__set_error(CARGS_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        char *arg = cargs__shift_args(&argc, &argv);
                        int64_t val;
                        int res = cargs__str2int_generic(&val, arg, INT8_MIN, INT8_MAX);
                        if (res != CARGS_ERROR_NONE) {
                            cargs__set_error(res, flag_name, arg);
                            return false;
                        }
                        *(int8_t *)(cargs__flags[i].value_ptr) = (int8_t)val;
                    }
                    break;

                    case CARGS_INT16: {
                        if (argc == 0 || cargs__is_flag(*argv)) {
                            cargs__set_error(CARGS_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        char *arg = cargs__shift_args(&argc, &argv);
                        int64_t val;
                        int res = cargs__str2int_generic(&val, arg, INT16_MIN, INT16_MAX);
                        if (res != CARGS_ERROR_NONE) {
                            cargs__set_error(res, flag_name, arg);
                            return false;
                        }
                        *(int16_t *)(cargs__flags[i].value_ptr) = (int16_t)val;
                    }
                    break;

                    case CARGS_INT32: {
                        if (argc == 0 || cargs__is_flag(*argv)) {
                            cargs__set_error(CARGS_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        char *arg = cargs__shift_args(&argc, &argv);
                        int64_t val;
                        int res = cargs__str2int_generic(&val, arg, INT32_MIN, INT32_MAX);
                        if (res != CARGS_ERROR_NONE) {
                            cargs__set_error(res, flag_name, arg);
                            return false;
                        }
                        *(int32_t *)(cargs__flags[i].value_ptr) = (int32_t)val;
                    }
                    break;

                    case CARGS_INT64: {
                        if (argc == 0 || cargs__is_flag(*argv)) {
                            cargs__set_error(CARGS_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        char *arg = cargs__shift_args(&argc, &argv);
                        int64_t val;
                        int res = cargs__str2int_generic(&val, arg, INT64_MIN, INT64_MAX);
                        if (res != CARGS_ERROR_NONE) {
                            cargs__set_error(res, flag_name, arg);
                            return false;
                        }
                        *(int64_t *)(cargs__flags[i].value_ptr) = val;
                    }
                    break;

                    case CARGS_UINT8: {
                        if (argc == 0 || cargs__is_flag(*argv)) {
                            cargs__set_error(CARGS_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        char *arg = cargs__shift_args(&argc, &argv);
                        uint64_t val;
                        int res = cargs__str2uint_generic(&val, arg, 0, UINT8_MAX);
                        if (res != CARGS_ERROR_NONE) {
                            cargs__set_error(res, flag_name, arg);
                            return false;
                        }
                        *(uint8_t *)(cargs__flags[i].value_ptr) = (uint8_t)val;
                    }
                    break;

                    case CARGS_UINT16: {
                        if (argc == 0 || cargs__is_flag(*argv)) {
                            cargs__set_error(CARGS_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        char *arg = cargs__shift_args(&argc, &argv);
                        uint64_t val;
                        int res = cargs__str2uint_generic(&val, arg, 0, UINT16_MAX);
                        if (res != CARGS_ERROR_NONE) {
                            cargs__set_error(res, flag_name, arg);
                            return false;
                        }
                        *(uint16_t *)(cargs__flags[i].value_ptr) = (uint16_t)val;
                    }
                    break;

                    case CARGS_UINT32: {
                        if (argc == 0 || cargs__is_flag(*argv)) {
                            cargs__set_error(CARGS_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        char *arg = cargs__shift_args(&argc, &argv);
                        uint64_t val;
                        int res = cargs__str2uint_generic(&val, arg, 0, UINT32_MAX);
                        if (res != CARGS_ERROR_NONE) {
                            cargs__set_error(res, flag_name, arg);
                            return false;
                        }
                        *(uint32_t *)(cargs__flags[i].value_ptr) = (uint32_t)val;
                    }
                    break;

                    case CARGS_UINT64: {
                        if (argc == 0 || cargs__is_flag(*argv)) {
                            cargs__set_error(CARGS_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        char *arg = cargs__shift_args(&argc, &argv);
                        uint64_t val;
                        int res = cargs__str2uint_generic(&val, arg, 0, UINT64_MAX);
                        if (res != CARGS_ERROR_NONE) {
                            cargs__set_error(res, flag_name, arg);
                            return false;
                        }
                        *(uint64_t *)(cargs__flags[i].value_ptr) = val;
                    }
                    break;

                    case CARGS_SIZE_T: {
                        if (argc == 0 || cargs__is_flag(*argv)) {
                            cargs__set_error(CARGS_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        char *arg = cargs__shift_args(&argc, &argv);
                        uint64_t val;
                        int res = cargs__str2uint_generic(&val, arg, 0, SIZE_MAX);
                        if (res != CARGS_ERROR_NONE) {
                            cargs__set_error(res, flag_name, arg);
                            return false;
                        }
                        *(size_t *)(cargs__flags[i].value_ptr) = (size_t)val;
                    }
                    break;

                    case CARGS_FLOAT: {
                        if (argc == 0 || cargs__is_flag(*argv)) {
                            cargs__set_error(CARGS_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        char *arg = cargs__shift_args(&argc, &argv);
                        long double val;
                        int res = cargs__str2float_generic(&val, arg, -FLT_MAX, FLT_MAX);
                        if (res != CARGS_ERROR_NONE) {
                            cargs__set_error(res, flag_name, arg);
                            return false;
                        }
                        *(float *)(cargs__flags[i].value_ptr) = (float)val;
                    }
                    break;

                    case CARGS_DOUBLE: {
                        if (argc == 0 || cargs__is_flag(*argv)) {
                            cargs__set_error(CARGS_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        char *arg = cargs__shift_args(&argc, &argv);
                        long double val;
                        int res = cargs__str2float_generic(&val, arg, -DBL_MAX, DBL_MAX);
                        if (res != CARGS_ERROR_NONE) {
                            cargs__set_error(res, flag_name, arg);
                            return false;
                        }
                        *(double *)(cargs__flags[i].value_ptr) = (double)val;
                    }
                    break;

                    case CARGS_LONG_DOUBLE: {
                        if (argc == 0 || cargs__is_flag(*argv)) {
                            cargs__set_error(CARGS_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        char *arg = cargs__shift_args(&argc, &argv);
                        long double val;
                        int res = cargs__str2float_generic(&val, arg, -LDBL_MAX, LDBL_MAX);
                        if (res != CARGS_ERROR_NONE) {
                            cargs__set_error(res, flag_name, arg);
                            return false;
                        }
                        *(long double *)(cargs__flags[i].value_ptr) = val;
                    }
                    break;

                    case CARGS_STRING: {
                        if (argc == 0 || cargs__is_flag(*argv)) {
                            cargs__set_error(CARGS_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        char *arg = cargs__shift_args(&argc, &argv);
                        *(char **)(cargs__flags[i].value_ptr) = arg;
                    }
                    break;

                    case CARGS_TYPE_COUNT:
                    default: {
                        assert(0 && "Unreachable, Unknown Type");
                    }
                }
                // exit loop if we successfully parsed the flag
                break;
            }

        }
        
        // check if we parsed the flag
        if (i == cargs__count) {
            // If the unknown argument looks like a flag, report error
            if (cargs__is_flag(flag_name)) {
                cargs__set_error(CARGS_ERROR_UNKNOWN, flag_name, NULL);
                return false;
            }
            // Try to assign to a positional argument
            int pos_idx = cargs__find_next_positional();
            if (pos_idx != -1) {
                cargs__flags[pos_idx].val.string = flag_name;
                *(char **)(cargs__flags[pos_idx].value_ptr) = flag_name;
            } else {
                cargs__set_error(CARGS_ERROR_UNKNOWN, flag_name, NULL);
                return false;
            }
        }
    }
    
    // Validate mandatory positionals
    for (uint32_t i = 0; i < cargs__count; ++i) {
        if (cargs__flags[i].type == CARGS_POSITIONAL && cargs__flags[i].mandatory) {
            if (cargs__flags[i].val.string == NULL) {
                cargs__set_error(CARGS_ERROR_MISSING_POSITIONAL, cargs__flags[i].name, NULL);
                return false;
            }
        }
    }
    
    return true;
}

void cargs_log_error(FILE *stream)
{
    char cargs__type[] = "command";
    if (cargs__err.flag != NULL && *cargs__err.flag == '-') strcpy(cargs__type, "flag");
    
    switch (cargs__err.error) {
        case CARGS_ERROR_NONE:
            fprintf(stream, "No Error. Please only call cargs_log_error if flag_parse returned false!\n");
        break;

        case CARGS_ERROR_UNKNOWN:
            fprintf(stream, "ERROR: UNKNOWN %s \"%s\"\n", cargs__type, cargs__err.flag);
        break;

        case CARGS_ERROR_NO_VALUE:
            fprintf(stream, "ERROR: NO VALUE provided for %s \"%s\"\n", cargs__type, cargs__err.flag);
        break;

        case CARGS_ERROR_INVALID_NUMBER:
            fprintf(stream, "ERROR: INVALID VALUE for %s \"%s\". Provided value was \"%s\"\n", cargs__type, cargs__err.flag, cargs__err.value);
        break;

        case CARGS_ERROR_OVERFLOW:
            fprintf(stream, "ERROR: OVERFLOW while parsing %s \"%s\". Provided value was \"%s\"\n", cargs__type, cargs__err.flag, cargs__err.value);
        break;

        case CARGS_ERROR_UNDERFLOW:
            fprintf(stream, "ERROR: UNDERFLOW while parsing %s \"%s\". Provided value was \"%s\"\n", cargs__type, cargs__err.flag, cargs__err.value);
        break;

    case CARGS_ERROR_OUT_OF_BOUNDS:
        fprintf(stream, "ERROR: Value OUT OF BOUNDS for %s \"%s\". Provided value was \"%s\"\n", cargs__type, cargs__err.flag, cargs__err.value);
        break;
    case CARGS_ERROR_MISSING_POSITIONAL:
        fprintf(stream, "ERROR: MISSING mandatory argument \"%s\"\n", cargs__err.flag);
        break;
        
    case CARGS_ERROR_COUNT:

        default:
            assert(0 && "Unreachable, Unknown Error");
    }
}

cargs_error cargs_get_error() {
    return cargs__err;
}

void cargs_log_options(FILE *stream, bool printdefault)
{
    for (uint32_t i = 0; i < cargs__count; ++i) {
		
		fprintf(stream, "    %s\n", cargs__flags[i].name);
		fprintf(stream, "          %s\n", cargs__flags[i].desc);

        if (!printdefault) continue;

	switch(cargs__flags[i].type) {
		case CARGS_BOOL:
			fprintf(stream, "          Default: %s\n", cargs__flags[i].def.boolean ? "true" : "false");
			break;
		case CARGS_POSITIONAL:
			fprintf(stream, "          %s\n", cargs__flags[i].mandatory ? "Mandatory" : "Optional");
			break;
	    case CARGS_CHAR:
			fprintf(stream, "          Default: %c\n", cargs__flags[i].def.character);
			break;
 
		case CARGS_INT8:
			fprintf(stream, "          Default: %d\n", cargs__flags[i].def.int8);
			break;

		case CARGS_UINT8:
			fprintf(stream, "          Default: %u\n", cargs__flags[i].def.uint8);
			break;

		case CARGS_INT16:
			fprintf(stream, "          Default: %d\n", cargs__flags[i].def.int16);
			break;

		case CARGS_UINT16:
			fprintf(stream, "          Default: %u\n", cargs__flags[i].def.uint16);
			break;

		case CARGS_INT32:
			fprintf(stream, "          Default: %d\n", cargs__flags[i].def.int32);
			break;

		case CARGS_UINT32:
			fprintf(stream, "          Default: %u\n", cargs__flags[i].def.uint32);
			break;

		case CARGS_INT64:
			fprintf(stream, "          Default: %" PRId64 "\n", cargs__flags[i].def.int64);
			break;

		case CARGS_UINT64:
			fprintf(stream, "          Default: %" PRIu64 "\n", cargs__flags[i].def.uint64);
			break;

		case CARGS_FLOAT:
			fprintf(stream, "          Default: %f\n", cargs__flags[i].def.floating);
			break;

		case CARGS_DOUBLE:
			fprintf(stream, "          Default: %f\n", cargs__flags[i].def.double_val);
			break;

		case CARGS_LONG_DOUBLE:
			fprintf(stream, "          Default: %Lf\n", cargs__flags[i].def.long_double);
			break;

		case CARGS_STRING:
			fprintf(stream, "          Default: %s\n", cargs__flags[i].def.string);
			break;

		case CARGS_SIZE_T:
			fprintf(stream, "          Default: %zu\n", cargs__flags[i].def.size);
			break;

		case CARGS_TYPE_COUNT:
		default:
			assert(0 && "Unreachable, Unknown Type");
			exit(69);
			break;
		}

	}
}


// helper functions

// allocate a new flag on the local stack with provided type, name and description
static struct cargs_flag *cargs__new(enum cargs_type type, const char *name, const char *desc)
{
    assert(cargs__count < CARGS_MAX_FLAGS && "To many flags! Define #CARGS_MAX_FLAGS to be a bigger number!");

    struct cargs_flag *flag = &cargs__flags[cargs__count++];
    memset(flag, 0, sizeof(*flag));

    flag->type  = type;
    flag->name = (char*) name;
    flag->desc = (char*) desc;
    flag->value_ptr = &flag->val;

    return flag;
}

// shift flags by one position
static char * cargs__shift_args(int *argc, char ***argv)
{
    assert(*argc > 0);

    char *res = **argv;

    *argc -= 1;
    *argv += 1;

    return res;
}

// converts a string to a signed integer of specified width and checks if it's within range
static int cargs__str2int_generic(int64_t *out, char *s, int64_t min, int64_t max) {
    char *end;
    if (s[0] == '\0' || isspace(s[0]))
        return CARGS_ERROR_INVALID_NUMBER;
    errno = 0;
    long long l = strtoll(s, &end, 10);
    if (errno == ERANGE) {
        if (l == LLONG_MAX) return CARGS_ERROR_OVERFLOW;
        if (l == LLONG_MIN) return CARGS_ERROR_UNDERFLOW;
    }
    if (*end != '\0')
        return CARGS_ERROR_INVALID_NUMBER;
    if (min > l || l > max)
        return CARGS_ERROR_OUT_OF_BOUNDS;
    *out = l;
    return CARGS_ERROR_NONE;
}

// converts a string to an unsigned integer of specified width and checks if it's within range
static int cargs__str2uint_generic(uint64_t *out, char *s, uint64_t min, uint64_t max) {
    char *end;
    if (s[0] == '\0' || isspace(s[0]))
        return CARGS_ERROR_INVALID_NUMBER;
    errno = 0;
    unsigned long long u = strtoull(s, &end, 10);
    if (errno == ERANGE)
        return CARGS_ERROR_OVERFLOW;
    if (*end != '\0')
        return CARGS_ERROR_INVALID_NUMBER;
    if (min > u || u > max)
        return CARGS_ERROR_OUT_OF_BOUNDS;
    *out = u;
    return CARGS_ERROR_NONE;
}

// converts a string to a float and checks if it's within range
static int cargs__str2float_generic(long double *out, char *s, long double min, long double max) {
    char *end;
    if (s[0] == '\0' || isspace(s[0]))
        return CARGS_ERROR_INVALID_NUMBER;
    errno = 0;
    long double res = strtold(s, &end);
    if (errno == ERANGE) {
        if (res == HUGE_VALL) return CARGS_ERROR_OVERFLOW;
        if (res == -HUGE_VALL) return CARGS_ERROR_UNDERFLOW;
    }
    if (*end != '\0')
        return CARGS_ERROR_INVALID_NUMBER;
    if (min > res || res > max)
        return CARGS_ERROR_OUT_OF_BOUNDS;
    *out = res;
    return CARGS_ERROR_NONE;
}

static void cargs__set_error(enum cargs_errors err, const char *flag, char *value)
{
    cargs__err.error = err;
    cargs__err.flag = (char *)flag;
    cargs__err.value = value;
}

static int cargs__find_next_positional()
{
    for (uint32_t i = 0; i < cargs__count; ++i) {
        if (cargs__flags[i].type == CARGS_POSITIONAL && cargs__flags[i].val.string == NULL) {
            return i;
        }
    }
    return -1;
}

static int cargs__is_flag(const char *arg)
{
    return arg != NULL && arg[0] == '-';
}

#endif //CARGS_IMPLEMENTATION