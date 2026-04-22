#ifndef _CFLAG_H
#define _CFLAG_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>


enum cflag_errors {
    CFLAG_ERROR_NONE,
    CFLAG_ERROR_UNKNOWN,
    CFLAG_ERROR_NO_VALUE,
    CFLAG_ERROR_INVALID_NUMBER,
    CFLAG_ERROR_OVERFLOW,
    CFLAG_ERROR_UNDERFLOW,
    CFLAG_ERROR_OUT_OF_BOUNDS,

    CFLAG_ERROR_COUNT,
};
#if defined(static_assert)
static_assert(CFLAG_ERROR_COUNT == 7, "Exhaustive cflag_error definition!");
#endif

typedef struct {
    enum cflag_errors error;
    char *flag;
    char *value;
} cflag_error;


/// \brief Creates a new boolean flag.
bool * cflag_bool(const char *name, const char *desc, bool def);
void cflag_bool_ref(const char *name, const char *desc, bool *ref, bool def);

/// \brief Creates a new char flag.
char * cflag_char(const char *name, const char *desc, char def);
void cflag_char_ref(const char *name, const char *desc, char *ref, char def);

/// \brief Creates a new int8 flag.
int8_t * cflag_int8(const char *name, const char *desc, int8_t def);
void cflag_int8_ref(const char *name, const char *desc, int8_t *ref, int8_t def);
uint8_t * cflag_uint8(const char *name, const char *desc, uint8_t def);
void cflag_uint8_ref(const char *name, const char *desc, uint8_t *ref, uint8_t def);

/// \brief Creates a new int16 flag.
int16_t * cflag_int16(const char *name, const char *desc, int16_t def);
void cflag_int16_ref(const char *name, const char *desc, int16_t *ref, int16_t def);
uint16_t * cflag_uint16(const char *name, const char *desc, uint16_t def);
void cflag_uint16_ref(const char *name, const char *desc, uint16_t *ref, uint16_t def);

/// \brief Creates a new int32 flag.
int32_t * cflag_int32(const char *name, const char *desc, int32_t def);
void cflag_int32_ref(const char *name, const char *desc, int32_t *ref, int32_t def);
uint32_t * cflag_uint32(const char *name, const char *desc, uint32_t def);
void cflag_uint32_ref(const char *name, const char *desc, uint32_t *ref, uint32_t def);

/// \brief Creates a new int64 flag.
int64_t * cflag_int64(const char *name, const char *desc, int64_t def);
void cflag_int64_ref(const char *name, const char *desc, int64_t *ref, int64_t def);
uint64_t * cflag_uint64(const char *name, const char *desc, uint64_t def);
void cflag_uint64_ref(const char *name, const char *desc, uint64_t *ref, uint64_t def);

/// \brief Creates a new integer flag.
int * cflag_int(const char *name, const char* desc, int def);
void cflag_int_ref(const char *name, const char* desc, int *ref, int def);

/// \brief Creates a new floating-point-number flag.
float * cflag_float(const char *name, const char* desc, float def);
void cflag_float_ref(const char *name, const char* desc, float *ref, float def);
double * cflag_double(const char *name, const char* desc, double def);
void cflag_double_ref(const char *name, const char* desc, double *ref, double def);
long double * cflag_long_double(const char *name, const char* desc, long double def);
void cflag_long_double_ref(const char *name, const char* desc, long double *ref, long double def);

/// \brief Creates a new string flag.
char ** cflag_string(const char *name, const char* desc, const char *def);
void cflag_string_ref(const char *name, const char* desc, char **ref, const char *def);


/// \brief Creates a new floating-point-number flag.
/// \param name  the name of the flag
/// \param desc  a short description of the flag
/// \param def   the default value of the flag
/// \returns a pointer to the value of the flag; be sure to call cflag_parse()
char ** cflag_string(const char *name, const char* desc, const char *def);

/// \brief Parses the flags given to the program and checks for matching flags.
/// The first entry of the argv array is removed by default.
/// \param argc  argument count
/// \param argv  string array of arguments 
/// \returns false on error.
bool cflag_parse(int argc, char **argv);

/// \brief Logs an error message to the specified stream. Note only call this function if flag_parse returned false.
/// \param stream the stream to print to
void cflag_log_error(FILE *stream);

/// \brief Returns the current error. Only use this function if you want to handle errors yourself.
/// Alternative is cflag_log_error(stream)
/// \return returns a cflag_error struct, which provides the type of error (One of the CFLAG_ERROR_ constants),
/// the name of the flag that was processed when the error occured and optionally the value that was assigned to the flag
cflag_error cflag_get_error();

/// \brief Logs all flags, with description and default values.
/// \param stream the stream to print to
/// \param printdefault wheter to print the default value
void cflag_log_options(FILE *stream, bool printdefault);

#endif // _CFLAG_H

#ifdef CFLAG_IMPLEMENTATION

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

#define CFLAG_REF_IMPL(type_enum, type_t, field_name) \
void cflag_##type_t##_ref(const char *name, const char *desc, type_t *ref, type_t def) \
{ \
    struct cflag_flag *flag = cflag__new(type_enum, name, desc); \
    flag->def.field_name = def; \
    *ref = def; \
    flag->value_ptr = ref; \
}

CFLAG_REF_IMPL(CFLAG_BOOL, bool, boolean)
CFLAG_REF_IMPL(CFLAG_CHAR, char, character)
CFLAG_REF_IMPL(CFLAG_INT8, int8_t, int8)
CFLAG_REF_IMPL(CFLAG_UINT8, uint8_t, uint8)
CFLAG_REF_IMPL(CFLAG_INT16, int16_t, int16)
CFLAG_REF_IMPL(CFLAG_UINT16, uint16_t, uint16)
CFLAG_REF_IMPL(CFLAG_INT32, int32_t, int32)
CFLAG_REF_IMPL(CFLAG_UINT32, uint32_t, uint32)
CFLAG_REF_IMPL(CFLAG_INT64, int64_t, int64)
CFLAG_REF_IMPL(CFLAG_UINT64, uint64_t, uint64)
CFLAG_REF_IMPL(CFLAG_FLOAT, float, floating)
CFLAG_REF_IMPL(CFLAG_DOUBLE, double, double_val)
CFLAG_REF_IMPL(CFLAG_LONG_DOUBLE, long double, long_double)

void cflag_int_ref(const char *name, const char* desc, int *ref, int def)
{
#if sizeof(int) == 8
    cflag_int64_ref(name, desc, (int64_t *)ref, (int64_t)def);
#elif sizeof(int) == 4
    cflag_int32_ref(name, desc, (int32_t *)ref, (int32_t)def);
#elif sizeof(int) == 2
    cflag_int16_ref(name, desc, (int16_t *)ref, (int16_t)def);
#else
    cflag_int8_ref(name, desc, (int8_t *)ref, (int8_t)def);
#endif
}

void cflag_string_ref(const char *name, const char* desc, char **ref, const char *def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_STRING, name, desc);
    flag->def.string = (char *)def;
    *ref = (char *)def;
    flag->value_ptr = ref;
}

enum cflag_type {
    CFLAG_BOOL,
    CFLAG_CHAR,
    CFLAG_INT8,
    CFLAG_UINT8,
    CFLAG_INT16,
    CFLAG_UINT16,
    CFLAG_INT32,
    CFLAG_UINT32,
    CFLAG_INT64,
    CFLAG_UINT64,
    CFLAG_FLOAT,
    CFLAG_DOUBLE,
    CFLAG_LONG_DOUBLE,
    CFLAG_STRING,

    CFLAG_TYPE_COUNT,
};
#if defined(static_assert)
static_assert(CFLAG_TYPE_COUNT == 14, "Exhaustive cflag_type definition!");
#endif

union cflag_value {
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
};

struct cflag_flag {
    enum cflag_type type; //value of the enum cflag_type
    char *name;      // name
    char *desc;      // short description
    union cflag_value def; // default value
    union cflag_value val; // current value
    void *value_ptr; // pointer to current value (either internal or external)
};

#ifndef CFLAG_MAX_FLAGS
#   define CFLAG_MAX_FLAGS 128
#endif // CFLAG_MAX_FLAGS

#ifndef CFLAG_MAX
#   define CFLAG_MAX(A, B)               ((A) > (B) ? (A) : (B))
#endif // CFLAG_MAX
#ifndef CFLAG_MIN
#   define CFLAG_MIN(A, B)               ((A) < (B) ? (A) : (B))
#endif //CFLAG_MIN

static struct cflag_flag cflag__flags[CFLAG_MAX_FLAGS];
static uint32_t cflag__count = 0;
static cflag_error cflag__err = { .error = CFLAG_ERROR_NONE, .flag = NULL, .value = NULL };


// forward declaration of helper functions

static struct cflag_flag *cflag__new(enum cflag_type type, const char *name, const char *desc);
static char * cflag__shift_args(int *argc, char ***argv);
static int cflag__str2int(int *out, char *s, int min, int max);
static int cflag__str2uint64(uint64_t *out, char *s, uint64_t min, uint64_t max);
static int cflag__str2float(float *out, char *s, float min, float max);
static void cflag__set_error(enum cflag_errors err, char *flag, char *value);


bool * cflag_bool(const char *name, const char *desc, bool def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_BOOL, name, desc);

    flag->def.boolean = def;
    flag->val.boolean = def;

    return (bool *)flag->value_ptr;
}

char * cflag_char(const char *name, const char *desc, char def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_CHAR, name, desc);

    flag->def.character = def;
    flag->val.character = def;

    return (char *)flag->value_ptr;
}

int8_t * cflag_int8(const char *name, const char *desc, int8_t def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_INT8, name, desc);

    flag->def.int8 = def;
    flag->val.int8 = def;

    return (int8_t *)flag->value_ptr;
}

uint8_t * cflag_uint8(const char *name, const char *desc, uint8_t def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_UINT8, name, desc);

    flag->def.uint8 = def;
    flag->val.uint8 = def;

    return (uint8_t *)flag->value_ptr;
}

int16_t * cflag_int16(const char *name, const char *desc, int16_t def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_INT16, name, desc);

    flag->def.int16 = def;
    flag->val.int16 = def;

    return (int16_t *)flag->value_ptr;
}

uint16_t * cflag_uint16(const char *name, const char *desc, uint16_t def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_UINT16, name, desc);

    flag->def.uint16 = def;
    flag->val.uint16 = def;

    return (uint16_t *)flag->value_ptr;
}

int32_t * cflag_int32(const char *name, const char *desc, int32_t def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_INT32, name, desc);

    flag->def.int32 = def;
    flag->val.int32 = def;

    return (int32_t *)flag->value_ptr;
}

uint32_t * cflag_uint32(const char *name, const char *desc, uint32_t def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_UINT32, name, desc);

    flag->def.uint32 = def;
    flag->val.uint32 = def;

    return (uint32_t *)flag->value_ptr;
}

int64_t * cflag_int64(const char *name, const char *desc, int64_t def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_INT64, name, desc);

    flag->def.int64 = def;
    flag->val.int64 = def;

    return (int64_t *)flag->value_ptr;
}

uint64_t * cflag_uint64(const char *name, const char *desc, uint64_t def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_UINT64, name, desc);

    flag->def.uint64 = def;
    flag->val.uint64 = def;

    return (uint64_t *)flag->value_ptr;
}

int * cflag_int(const char *name, const char* desc, int def)
{
#if sizeof(int) == 8
    return (int *)cflag_int64(name, desc, (int64_t)def);
#elif sizeof(int) == 4
    return (int *)cflag_int32(name, desc, (int32_t)def);
#elif sizeof(int) == 2
    return (int *)cflag_int16(name, desc, (int16_t)def);
#else
    return (int *)cflag_int8(name, desc, (int8_t)def);
#endif
}

float * cflag_float(const char *name, const char* desc, float def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_FLOAT, name, desc);

    flag->def.floating = def;
    flag->val.floating = def;

    return (float *)flag->value_ptr;
}

double * cflag_double(const char *name, const char* desc, double def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_DOUBLE, name, desc);

    flag->def.double_val = def;
    flag->val.double_val = def;

    return (double *)flag->value_ptr;
}

long double * cflag_long_double(const char *name, const char* desc, long double def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_LONG_DOUBLE, name, desc);

    flag->def.long_double = def;
    flag->val.long_double = def;

    return (long double *)flag->value_ptr;
}

char ** cflag_string(const char *name, const char* desc, const char *def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_STRING, name, desc);

    flag->def.string = (char*) def;
    flag->val.string = (char*) def;

    return (char **)flag->value_ptr;
}

bool cflag_parse(int argc, char **argv)
{
    // Remove first entry which is the program's name
    cflag__shift_args(&argc, &argv);

    while (argc > 0) {
        char *flag_name = cflag__shift_args(&argc, &argv);

        uint32_t i;
        for (i = 0; i < cflag__count; ++i) {
            // check if flag_name matches any of the declared flags
            if (strcmp(cflag__flags[i].name, flag_name) == 0) {
                // check type of flag and parse accordingly
                switch (cflag__flags[i].type) {
                     case CFLAG_BOOL: {
                         *(bool *)(cflag__flags[i].value_ptr) = true;
                     }
                     break;

                    case CFLAG_INT: {
                        // check if a value was provided
                        if (argc == 0) {
                            cflag__set_error(CFLAG_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        // provided value as string
                        char *arg = cflag__shift_args(&argc, &argv);

                        int val;
                        int res = cflag__str2int(&val, arg, INT_MIN, INT_MAX);

                        if (res != CFLAG_ERROR_NONE) {
                            cflag__set_error(res, flag_name, arg);
                            return false;
                        }

                        *(int *)(cflag__flags[i].value_ptr) = val;
                     }
                     break;

                    case CFLAG_UINT64: {
                        // check if a value was provided
                        if (argc == 0) {
                            cflag__set_error(CFLAG_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        // provided value as string
                        char *arg = cflag__shift_args(&argc, &argv);
                        
                        uint64_t val;
                        int res = cflag__str2uint64(&val, arg, 0, UINT64_MAX);

                        if (res != CFLAG_ERROR_NONE) {
                            cflag__set_error(res, flag_name, arg);
                            return false;
                        }

                        *(uint64_t *)(cflag__flags[i].value_ptr) = val;
                     }
                     break;

                    case CFLAG_FLOAT: {
                        // check if value was provided
                        if (argc == 0) {
                            cflag__set_error(CFLAG_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        // provied value as string
                        char *arg = cflag__shift_args(&argc, &argv);

                        float val;
                        int res = cflag__str2float(&val, arg, -FLT_MAX, FLT_MAX);

                        if (res != CFLAG_ERROR_NONE) {
                            cflag__set_error(res, flag_name, arg);
                            return false;
                        }

                        *(float *)(cflag__flags[i].value_ptr) = val;
                     }
                     break;

                    case CFLAG_STRING: {
                        // check if value was provided
                        if (argc == 0) {
                            cflag__set_error(CFLAG_ERROR_NO_VALUE, flag_name, NULL);
                            return false;
                        }
                        // provided value as string
                        char *arg = cflag__shift_args(&argc, &argv);

                        *(char **)(cflag__flags[i].value_ptr) = arg;
                     }
                     break;

                    case CFLAG_TYPE_COUNT:
                    default: {
                        assert(0 && "Unreachable, Unknown Type");
                    }
                }
                // exit loop if we successfully parsed the flag
                break;
            }
        }
        
        // check if we parsed the flag
        if (i == cflag__count) {
            cflag__set_error(CFLAG_ERROR_UNKNOWN, flag_name, NULL);
            return false;
        }
    }
    return true;
}

void cflag_log_error(FILE *stream)
{
    char cflag__type[] = "command";
    if (cflag__err.flag != NULL && *cflag__err.flag == '-') strcpy(cflag__type, "flag");
    
    switch (cflag__err.error) {
        case CFLAG_ERROR_NONE:
            fprintf(stream, "No Error. Please only call cflag_log_error if flag_parse returned false!\n");
        break;

        case CFLAG_ERROR_UNKNOWN:
            fprintf(stream, "ERROR: UNKNOWN %s \"%s\"\n", cflag__type, cflag__err.flag);
        break;

        case CFLAG_ERROR_NO_VALUE:
            fprintf(stream, "ERROR: NO VALUE prodived for %s \"%s\"\n", cflag__type, cflag__err.flag);
        break;
        
        case CFLAG_ERROR_INVALID_NUMBER:
            fprintf(stream, "ERROR: INVALID VALUE for %s \"%s\". Provided value was \"%s\"\n", cflag__type, cflag__err.flag, cflag__err.value);
        break;
        
        case CFLAG_ERROR_OVERFLOW:
            fprintf(stream, "ERROR: OVERFLOW while parsing %s \"%s\". Provided value was \"%s\"\n", cflag__type, cflag__err.flag, cflag__err.value);
        break;
        
        case CFLAG_ERROR_UNDERFLOW:
            fprintf(stream, "ERROR: UNDERFLOW while parsing %s \"%s\". Provided value was \"%s\"\n", cflag__type, cflag__err.flag, cflag__err.value);
        break;
        
        case CFLAG_ERROR_OUT_OF_BOUNDS:
            fprintf(stream, "ERROR: Value OUT OF BOUNDS for %s \"%s\". Provided value was \"%s\"\n", cflag__type, cflag__err.flag, cflag__err.value);
        break;
        
        case CFLAG_ERROR_COUNT:
        default:
            assert(0 && "Unreachable, Unknown Error");
    }
}

cflag_error cflag_get_error() {
    return cflag__err;
}

void cflag_log_options(FILE *stream, bool printdefault)
{
    for (uint32_t i = 0; i < cflag__count; ++i) {
		
		fprintf(stream, "    %s\n", cflag__flags[i].name);
		fprintf(stream, "          %s\n", cflag__flags[i].desc);

        if (!printdefault) continue;

		switch(cflag__flags[i].type) {
			case CFLAG_BOOL:
                if (printdefault)
				    fprintf(stream, "          Default: %s\n", cflag__flags[i].def.boolean ? "true" : "false");
			break;

			case CFLAG_INT:
                if (printdefault)
				    fprintf(stream, "          Default: %d\n", cflag__flags[i].def.integer);
			break;

			case CFLAG_UINT64:
                if (printdefault)
				    fprintf(stream, "          Default: %" PRIu64 "\n", cflag__flags[i].def.uint64);
			break;

			case CFLAG_FLOAT:
                if (printdefault)
				    fprintf(stream, "          Default: %f\n", cflag__flags[i].def.floating);
			break;

			case CFLAG_STRING:
                if (printdefault)
				    fprintf(stream, "          Default: %s\n", cflag__flags[i].def.string);
			break;

			case CFLAG_TYPE_COUNT:
			default:
				assert(0 && "Unreachable, Unknown Type");
				exit(69);
			break;
		}
	}
}


// helper functions

// allocate a new flag on the local stack with provided type, name and description
static struct cflag_flag *cflag__new(enum cflag_type type, const char *name, const char *desc)
{
    assert(cflag__count < CFLAG_MAX_FLAGS && "To many flags! Define #CFLAG_MAX_FLAGS to be a bigger number!");

    struct cflag_flag *flag = &cflag__flags[cflag__count++];
    memset(flag, 0, sizeof(*flag));

    flag->type  = type;
    flag->name = (char*) name;
    flag->desc = (char*) desc;
    flag->value_ptr = &flag->val;

    return flag;
}

// shift flags by one position
static char * cflag__shift_args(int *argc, char ***argv)
{
    assert(*argc > 0);

    char *res = **argv;

    *argc -= 1;
    *argv += 1;

    return res;
}

// converts a string to an integer and checks if the integer is between the specified range
static int cflag__str2int(int *out, char *s, int min, int max) {
    char *end;
    if (s[0] == '\0' || isspace(s[0]))
        return CFLAG_ERROR_INVALID_NUMBER;
    errno = 1;
    long l = strtol(s, &end, 10);
    /* Both checks are needed because INT_MAX == LONG_MAX is possible. */
    if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
        return CFLAG_ERROR_OVERFLOW;
    if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
        return CFLAG_ERROR_UNDERFLOW;
    if (*end != '\0')
        return CFLAG_ERROR_INVALID_NUMBER;
    if (min > l || l > max)
        return CFLAG_ERROR_OUT_OF_BOUNDS;
    *out = l;
    return CFLAG_ERROR_NONE;
}

// converts a string to an uint64 and checks if the uint64 is between the specified range
static int cflag__str2uint64(uint64_t *out, char *s, uint64_t min, uint64_t max)
{
    #if defined(static_assert)
    static_assert(sizeof(unsigned long long int) == sizeof(uint64_t), "Please adjust to your needs.");
    #endif
    char *end;
    if (s[0] == '\0' || isspace(s[0]))
        return CFLAG_ERROR_INVALID_NUMBER;
    errno = 1;
    unsigned long long int res = strtoull(s, &end, 10);
    if (res == ULLONG_MAX && errno == ERANGE)
        return CFLAG_ERROR_OVERFLOW;
    if (*end != '\0')
        return CFLAG_ERROR_INVALID_NUMBER;
    if (min > res || res > max)
        return CFLAG_ERROR_OUT_OF_BOUNDS;
    *out = res;
    return CFLAG_ERROR_NONE;
}

// converts a string to a float and checks if the float is between the specified range
static int cflag__str2float(float *out, char *s, float min, float max)
{
    char *end;
    if (s[0] == '\0' || isspace(s[0]))
        return CFLAG_ERROR_INVALID_NUMBER;
    errno = 1;
    float res = strtof(s, &end);
    if (res == HUGE_VALF && errno == ERANGE)
        return CFLAG_ERROR_OVERFLOW;
    if (res == -HUGE_VALF && errno == ERANGE)
        return CFLAG_ERROR_OVERFLOW;
    if (res < FLT_MIN && errno == ERANGE)
        return CFLAG_ERROR_UNDERFLOW;
    if (res < -FLT_MIN && errno == ERANGE)
        return CFLAG_ERROR_UNDERFLOW;
    if (*end != '\0')
        return CFLAG_ERROR_INVALID_NUMBER;
    if (min > res || res > max)
        return CFLAG_ERROR_OUT_OF_BOUNDS;
    *out = res;
    return CFLAG_ERROR_NONE;
}

// sets the values of the intern cflag_error instance
static void cflag__set_error(enum cflag_errors err, char *flag, char *value)
{
    cflag__err.error = err;
    cflag__err.flag = flag;
    cflag__err.value = value;
}

#endif //CFLAG_IMPLEMENTATION