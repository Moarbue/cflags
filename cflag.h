#ifndef _CFLAG_H
#define _CFLAG_H

#include <stdbool.h>

/// \brief Creates a new boolean flag.
/// \param name  the name of the flag without dash
/// \param desc  a short description of the flag
/// \param def   the default value of the flag
/// \returns a pointer to the value of the flag; be sure to first call cflag_parse()
bool * cflag_bool(const char *name, const char *desc, bool def);

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
#include <stdint.h>
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

static char * cflag__shift_args(int *argc, char ***argv);


struct cflag_flag *cflag_new(enum cflag_type type, const char *name, const char *desc)
{
    assert(cflag_count < CFLAG_MAX && "To many flags! Change size of CFLAG_MAX!");

    struct cflag_flag *flag = &cflag_flags[cflag_count++];
    memset(flag, 0, sizeof(*flag));

    flag->type  = type;
    flag->name = (char*) name;
    flag->desc = (char*) desc;

    return flag;
}

bool * cflag_bool(const char *name, const char *desc, bool def)
{
    struct cflag_flag *flag = cflag_new(CFLAG_BOOL, name, desc);

    flag->def.boolean = def;
    flag->val.boolean = def;

    return &flag->val.boolean;
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

// shift flags by one position
static char * cflag__shift_args(int *argc, char ***argv)
{
    assert(*argc > 0);

    char *res = **argv;

    *argc -= 1;
    *argv += 1;

    return res;
}

#endif //CFLAG_IMPLEMENTATION