#ifndef CARGS_H
#define CARGS_H

#ifndef CARGS_MAX_FLAGS
#   error "CARGS_MAX_FLAGS not defined"
#endif

#ifndef CARGS_MAX_SUBCOMMANDS
#   error "CARGS_MAX_SUBCOMMANDS not defined"
#endif

#ifndef CARGS_MAX_ERROR_LEN
#   define CARGS_MAX_ERROR_LEN 512
#endif

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
typedef bool (validation_func_t)(const char *name, const char *value);

typedef enum {
    CARGS_OK,
    CARGS_INVALID_ARGUMENT,
    CARGS_MISSING_ARGUMENT,
    CARGS_UNKNOWN_FLAG,
    CARGS_UNKNOWN_SUBCOMMAND,
} cargs_error;


CARGSDEF void cargs_int(const char *long_name, const char short_name, int *reference, const int default_value, validation_func_t *validtion_func, const char *description);
CARGSDEF void cargs_uint(const char *long_name, const char short_name, unsigned int *reference, const unsigned int default_value, validation_func_t *validtion_func, const char *description);
CARGSDEF void cargs_bool(const char *long_name, const char short_name, bool *reference, const bool default_value, validation_func_t *validtion_func, const char *description);
CARGSDEF void cargs_float(const char *long_name, const char short_name, float *reference, const float default_value, validation_func_t *validtion_func, const char *description);
CARGSDEF void cargs_char(const char *long_name, const char short_name, char *reference, const char default_value, validation_func_t *validtion_func, const char *description);
CARGSDEF void cargs_string(const char *long_name, const char short_name, char **reference, const char *default_value, validation_func_t *validtion_func, const char *description);

CARGSDEF void cargs_subcommand_start(const char *name, const char *description);
CARGSDEF void cargs_subcommand_end(void);

CARGSDEF void cargs_set_error(cargs_error error, const char *format, ...);
CARGSDEF cargs_error cargs_get_error(void);
CARGSDEF const char *cargs_get_error_message(void);
CARGSDEF void cargs_log_error(FILE *stream);

CARGSDEF int cargs_parse(int argc, char **argv);
CARGSDEF void cargs_reset(void);
CARGSDEF void cargs_print_help(FILE *stream);


#endif // CARGS_H

#ifdef CARGS_IMPLEMENTATION

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

enum cargs_flag_type {
    CARGS_FLAG_TYPE_INT,
    CARGS_FLAG_TYPE_UINT,
    CARGS_FLAG_TYPE_BOOL,
    CARGS_FLAG_TYPE_FLOAT,
    CARGS_FLAG_TYPE_CHAR,
    CARGS_FLAG_TYPE_STRING,
};

struct cargs_flag {
    struct cargs_flag *next;
    enum cargs_flag_type type;
    const char *long_name;
    char short_name;
    void *reference;
    validation_func_t *validation_func;
    const char *description;
};

struct cargs_subcommand {
    struct cargs_subcommand *next;
    const char *name;
    const char *description;
    struct cargs_subcommand *parent;
    struct cargs_subcommand *children_head;
    struct cargs_flag *flags_head;
};

static struct cargs_state {
    const char *program_name;

    struct cargs_flag *global_flags_head;
    struct cargs_subcommand *root_subcommands_head;
    struct cargs_subcommand *current_subcommand;

    // Static memory pools
    struct cargs_flag flag_pool[CARGS_MAX_FLAGS];
    size_t flags_allocated;

    struct cargs_subcommand subcommand_pool[CARGS_MAX_SUBCOMMANDS];
    size_t subcommands_allocated;

    cargs_error error;
    char error_message[CARGS_MAX_ERROR_LEN];
    bool parsed;
} cargs_state = {0};

CARGSDEF struct cargs_flag *cargs__new_flag(enum cargs_flag_type type, const char *long_name, const char short_name, validation_func_t *validation_func, const char *description);
CARGSDEF void cargs__panic_func(const char *file, int line, const char *message, ...);
#define cargs__panic(message, ...) cargs__panic_func(__FILE__, __LINE__, message, ##__VA_ARGS__)
CARGSDEF void cargs__check_duplicate_flags(struct cargs_flag *head, const char *long_name, char short_name);

#define CARGS_FLAG(enum_type, function_suffix, type_identifier) \
    void cargs_##function_suffix(const char *long_name, const char short_name, type_identifier *reference, const type_identifier default_value, validation_func_t *validation_func, const char *description) \
    { \
        struct cargs_flag *flag = cargs__new_flag(enum_type, long_name, short_name, validation_func, description); \
        flag->reference = reference; \
        *reference = (type_identifier)default_value; \
    }

CARGS_FLAG(CARGS_FLAG_TYPE_INT, int, int)
CARGS_FLAG(CARGS_FLAG_TYPE_UINT, uint, unsigned int)
CARGS_FLAG(CARGS_FLAG_TYPE_BOOL, bool, bool)
CARGS_FLAG(CARGS_FLAG_TYPE_FLOAT, float, float)
CARGS_FLAG(CARGS_FLAG_TYPE_CHAR, char, char)
CARGS_FLAG(CARGS_FLAG_TYPE_STRING, string, char *)

CARGSDEF void cargs_subcommand_start(const char *name, const char *description)
{
    if (cargs_state.parsed) cargs__panic("arguments already parsed");
    if (cargs_state.subcommands_allocated >= CARGS_MAX_SUBCOMMANDS) cargs__panic("too many subcommands, define bigger CARGS_MAX_SUBCOMMANDS");
    if (name == NULL) cargs__panic("subcommand name cannot be NULL");
    if (!isalnum(name[0])) cargs__panic("subcommand name must start with an alphanumeric character");
    if (description == NULL) cargs__panic("subcommand description cannot be NULL");

    struct cargs_subcommand **head;

    if (cargs_state.current_subcommand == NULL) {
        head = &cargs_state.root_subcommands_head;
    } else {
        head = &cargs_state.current_subcommand->children_head;
    }

    // check for duplicate subcommands at current level
    struct cargs_subcommand *curr = *head;
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0) cargs__panic("duplicate subcommand name: %s", name);
        curr = curr->next;
    }

    // create new subcommand
    struct cargs_subcommand *subcmd = &cargs_state.subcommand_pool[cargs_state.subcommands_allocated++];
    subcmd->next = NULL;
    subcmd->name = name;
    subcmd->description = description;
    subcmd->parent = cargs_state.current_subcommand;
    subcmd->children_head = NULL;
    subcmd->flags_head = NULL;

    // append subcommand to linked list
    if (*head == NULL) {
        *head = subcmd;
    } else {
        struct cargs_subcommand *curr = *head;
        while (curr->next != NULL) curr = curr->next;
        curr->next = subcmd;
    }

    // enter subcommand scope
    cargs_state.current_subcommand = subcmd;
}

CARGSDEF void cargs_subcommand_end(void)
{
    if (cargs_state.parsed) cargs__panic("arguments already parsed");
    if (cargs_state.current_subcommand == NULL) cargs__panic("unbalanced subcommand end: no active subcommand");

    cargs_state.current_subcommand = cargs_state.current_subcommand->parent;
}

CARGSDEF void cargs_set_error(cargs_error error, const char *format, ...)
{
    cargs_state.error = error;

    if (format != NULL) {
        va_list args;
        va_start(args, format);
        vsnprintf(cargs_state.error_message, sizeof(cargs_state.error_message), format, args);
        va_end(args);
    } else {
        cargs_state.error_message[0] = '\0';
    }
}

CARGSDEF cargs_error cargs_get_error(void)
{
    return cargs_state.error;
}

CARGSDEF const char *cargs_get_error_message(void)
{
    return cargs_state.error_message;
}

CARGSDEF void cargs_log_error(FILE *stream)
{
    fprintf(stream, "Error: %s\n", cargs_state.error_message);
}

CARGSDEF int cargs_parse(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return -1;
}

CARGSDEF void cargs_reset(void)
{
}

CARGSDEF void cargs_print_help(FILE *stream)
{
    (void)stream;
}



CARGSDEF struct cargs_flag *cargs__new_flag(enum cargs_flag_type type, const char *long_name, const char short_name, validation_func_t *validation_func, const char *description)
{
    if (cargs_state.parsed) cargs__panic("arguments already parsed");
    if (cargs_state.flags_allocated >= CARGS_MAX_FLAGS) cargs__panic("too many flags, define bigger CARGS_MAX_FLAGS");
    if (long_name == NULL) cargs__panic("flag long_name cannot be NULL");
    if (!isalnum(long_name[0])) cargs__panic("flag long_name must start with an alphanumeric character");
    if (short_name && !isalnum(short_name)) cargs__panic("flag short_name must be alphanumeric");
    if (description == NULL) cargs__panic("flag description cannot be NULL");
    if (short_name && strlen(long_name) == 1 && long_name[0] == short_name) cargs__panic("flag long_name and short_name cannot be the same");

    if (cargs_state.current_subcommand != NULL) {
        // check for duplicate flag names in current scope
        cargs__check_duplicate_flags(cargs_state.current_subcommand->flags_head, long_name, short_name);
    }

    // create new flag
    struct cargs_flag *flag = &cargs_state.flag_pool[cargs_state.flags_allocated++];
    flag->next = NULL;
    flag->type = type;
    flag->long_name = long_name;
    flag->short_name = short_name;
    flag->validation_func = validation_func;
    flag->description = description;

    // append flag to linked list
    struct cargs_flag **head = cargs_state.current_subcommand != NULL ? &cargs_state.current_subcommand->flags_head : &cargs_state.global_flags_head;
    if (*head == NULL) *head = flag; // list is empty
    else {
        struct cargs_flag *curr = *head;
        while (curr->next != NULL) curr = curr->next;
        curr->next = flag;
    }

    return flag;
}

CARGSDEF void cargs__panic_func(const char *file, int line, const char *message, ...)
{
    fprintf(stderr, "%s:%d: error: ", file, line);
    va_list args;
    va_start(args, message);
    vfprintf(stderr, message, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

CARGSDEF void cargs__check_duplicate_flags(struct cargs_flag *head, const char *long_name, char short_name)
{
    while (head != NULL) {
        if (strcmp(head->long_name, long_name) == 0) cargs__panic("duplicate flag name: %s", long_name);
        if (short_name != '\0' && head->short_name == short_name) cargs__panic("duplicate flag name: %c", short_name);
        head = head->next;
    }
}

#endif // CARGS_IMPLEMENTATION
