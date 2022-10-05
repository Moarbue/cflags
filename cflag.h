#ifndef _CFLAG_H
#define _CFLAG_H

#include <stdbool.h>
#include <stdint.h>

/// \brief Creates a new boolean flag.
/// \param name  the name of the flag without dash
/// \param desc  a short description of the flag
/// \param def   the default value of the flag
/// \returns a pointer to the value of the flag; be sure to call cflag_parse()
bool * cflag_bool(const char *name, const char *desc, bool def);

/// \brief Creates a new integer flag.
/// \param name  the name of the flag without dash
/// \param desc  a short description of the flag
/// \param def   the default value of the flag
/// \returns a pointer to the value of the flag; be sure to call cflag_parse()
int * cflag_int(const char *name, const char* desc, int def);

/// \brief Creates a new uint64 flag.
/// \param name  the name of the flag without dash
/// \param desc  a short description of the flag
/// \param def   the default value of the flag
/// \returns a pointer to the value of the flag; be sure to call cflag_parse()
uint64_t * cflag_uint64(const char *name, const char *desc, uint64_t def);

/// \brief Creates a new floating-point-number flag.
/// \param name  the name of the flag without dash
/// \param desc  a short description of the flag
/// \param def   the default value of the flag
/// \returns a pointer to the value of the flag; be sure to call cflag_parse()
float * cflag_float(const char *name, const char* desc, float def);

/// \brief Creates a new floating-point-number flag.
/// \param name  the name of the flag without dash
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

#endif // _CFLAG_H

#define CFLAG_IMPLEMENTATION
#ifdef CFLAG_IMPLEMENTATION

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

enum cflag_type {
    CFLAG_BOOL,
    CFLAG_INT,
    CFLAG_UINT64,
    CFLAG_FLOAT,
    CFLAG_STRING,

    CFLAG_TYPE_COUNT,
};
static_assert(CFLAG_TYPE_COUNT == 5, "Exhaustive cflag_type definition!");

union cflag_value {
    bool     boolean;
    int      integer;
    uint64_t uint64;
    float    floating;
    char *   string;
};

struct cflag_flag {
    enum cflag_type type; //value of the enum cflag_type
    char *name;      // name without dash
    char *desc;      // short description
    union cflag_value def; // default value
    union cflag_value val; // current value
};

#ifndef CFLAG_MAX
#define CFLAG_MAX 128
#endif // CFLAG_MAX

struct cflag_flag cflag_flags[CFLAG_MAX];
uint32_t cflag_count = 0;


// forward declaration of helper functions

static struct cflag_flag *cflag__new(enum cflag_type type, const char *name, const char *desc);
static char * cflag__shift_args(int *argc, char ***argv);
static int cflag__str2int(int *out, char *s, int min, int max);
static int cflag__str2uint64(uint64_t *out, char *s, uint64_t min, uint64_t max);
static int cflag__str2float(float *out, char *s, float min, float max);


bool * cflag_bool(const char *name, const char *desc, bool def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_BOOL, name, desc);

    flag->def.boolean = def;
    flag->val.boolean = def;

    return &flag->val.boolean;
}

int * cflag_int(const char *name, const char* desc, int def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_INT, name, desc);

    flag->def.integer = def;
    flag->val.integer = def;

    return &flag->val.integer;
}

uint64_t * cflag_uint64(const char *name, const char *desc, uint64_t def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_UINT64, name, desc);

    flag->def.uint64 = def;
    flag->val.uint64 = def;

    return &flag->val.uint64;
}

float * cflag_float(const char *name, const char* desc, float def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_FLOAT, name, desc);

    flag->def.floating = def;
    flag->val.floating = def;

    return &flag->val.floating;
}

char ** cflag_string(const char *name, const char* desc, const char *def)
{
    struct cflag_flag *flag = cflag__new(CFLAG_STRING, name, desc);

    flag->def.string = (char*) def;
    flag->val.string = (char*) def;

    return &flag->val.string;
}

bool cflag_parse(int argc, char **argv)
{
    // Remove first entry which is the program's name
    cflag__shift_args(&argc, &argv);

    while (argc > 0) {
        char *flag_name = cflag__shift_args(&argc, &argv);

        // check if flag starts with a dash
        if (*flag_name != '-') return false;
        flag_name++;
        // remove possible second dash for compability
        if (*flag_name == '-') flag_name++;

        uint32_t i;
        for (i = 0; i < cflag_count; ++i) {
            // check if flag_name matches any of the declared flags
            if (strcmp(cflag_flags[i].name, flag_name) == 0) {
                // check type of flag and parse accordingly
                switch (cflag_flags[i].type) {
                    case CFLAG_BOOL: {
                        cflag_flags[i].val.boolean = true;
                    }
                    break;

                    case CFLAG_INT: {
                        // check if a value was provided
                        if (argc == 0) return false;
                        // provided value as string
                        char *arg = cflag__shift_args(&argc, &argv);

                        int val;
                        int res = cflag__str2int(&val, arg, INT_MIN, INT_MAX);

                        if (res != 0) return false;

                        cflag_flags[i].val.integer = val;
                    }
                    break;

                    case CFLAG_UINT64: {
                        // check if a value was provided
                        if (argc == 0) return false;
                        // provided value as string
                        char *arg = cflag__shift_args(&argc, &argv);
                        
                        uint64_t val;
                        int res = cflag__str2uint64(&val, arg, 0, UINT64_MAX);

                        if (res != 0) return false;

                        cflag_flags[i].val.uint64 = val;
                    }
                    break;

                    case CFLAG_FLOAT: {
                        // check if value was provided
                        if (argc == 0) return false;
                        // provied value as string
                        char *arg = cflag__shift_args(&argc, &argv);

                       float val;
                       int res = cflag__str2float(&val, arg, -HUGE_VALF, HUGE_VALF); 

                       if (res != 0) return false;

                       cflag_flags[i].val.floating = val;
                    }
                    break;

                    case CFLAG_STRING: {
                        // check if value was provided
                        if (argc == 0) return false;
                        // provided value as string
                        char *arg = cflag__shift_args(&argc, &argv);

                        cflag_flags[i].val.string = arg;
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
        if (i == cflag_count) return false;
    }
    return true;
}


// helper functions

// allocate a new flag on the local stack with provided type, name and description
static struct cflag_flag *cflag__new(enum cflag_type type, const char *name, const char *desc)
{
    assert(cflag_count < CFLAG_MAX && "To many flags! Change size of CFLAG_MAX!");

    struct cflag_flag *flag = &cflag_flags[cflag_count++];
    memset(flag, 0, sizeof(*flag));

    flag->type  = type;
    flag->name = (char*) name;
    flag->desc = (char*) desc;

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
        return 0;
    errno = 1;
    long l = strtol(s, &end, 10);
    /* Both checks are needed because INT_MAX == LONG_MAX is possible. */
    if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
        return 2;
    if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
        return 3;
    if (*end != '\0')
        return 1;
    if (min > l || l > max)
        return 4;
    *out = l;
    return 0;
}

// converts a string to an uint64 and checks if the uint64 is between the specified range
static int cflag__str2uint64(uint64_t *out, char *s, uint64_t min, uint64_t max)
{
    static_assert(sizeof(unsigned long long int) == sizeof(uint64_t), "Please adjust to your needs.");
    char *end;
    if (s[0] == '\0' || isspace(s[0]))
        return 0;
    errno = 1;
    unsigned long long int res = strtoull(s, &end, 10);
    if (res == ULLONG_MAX && errno == ERANGE)
        return 2;
    if (*end != '\0')
        return 1;
    if (min > res || res > max)
        return 3;
    *out = res;
    return 0;
}

// converts a string to a float and checks if the float is between the specified range
static int cflag__str2float(float *out, char *s, float min, float max)
{
    char *end;
    if (s[0] == '\0' || isspace(s[0]))
        return 0;
    errno = 1;
    float res = strtof(s, &end);
    if (res == HUGE_VALF && errno == ERANGE)
        return 2;
    if (res == -HUGE_VALF && errno == ERANGE)
        return 2;
    if (res < FLT_MIN && errno == ERANGE)
        return 3;
    if (res < -FLT_MIN && errno == ERANGE)
        return 3;
    if (*end != '\0')
        return 1;
    if (min > res || res > max)
        return 4;
    *out = res;
    return 0;
}

#endif //CFLAG_IMPLEMENTATION