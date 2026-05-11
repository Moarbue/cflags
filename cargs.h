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

struct cargs_subcommand;

typedef enum {
    CARGS_OK,
    CARGS_INVALID_ARGUMENT,
    CARGS_MISSING_ARGUMENT,
    CARGS_UNKNOWN_FLAG,
    CARGS_UNKNOWN_SUBCOMMAND,
    CARGS_UNEXPECTED_ARGUMENT,
    CARGS_INVALID_BOOL,
    CARGS_INVALID_INT,
    CARGS_INVALID_UINT,
    CARGS_INVALID_FLOAT,
    CARGS_INVALID_CHAR,
    CARGS_POSITIONAL_STARTS_WITH_DASH,
} cargs_error;


CARGSDEF void cargs_int(const char *long_name, const char *short_name, int *reference, const int default_value, const char *argument, validation_func_t *validtion_func, const char *description);
CARGSDEF void cargs_uint(const char *long_name, const char *short_name, unsigned int *reference, const unsigned int default_value, const char *argument, validation_func_t *validtion_func, const char *description);
CARGSDEF void cargs_bool(const char *long_name, const char *short_name, bool *reference, const bool default_value, const char *argument, validation_func_t *validtion_func, const char *description);
CARGSDEF void cargs_float(const char *long_name, const char *short_name, float *reference, const float default_value, const char *argument, validation_func_t *validtion_func, const char *description);
CARGSDEF void cargs_char(const char *long_name, const char *short_name, char *reference, const char default_value, const char *argument, validation_func_t *validtion_func, const char *description);
CARGSDEF void cargs_string(const char *long_name, const char *short_name, char **reference, const char *default_value, const char *argument, validation_func_t *validtion_func, const char *description);

CARGSDEF void cargs_subcommand_start(const char *name, const char *description);
CARGSDEF void cargs_subcommand_end(void);

CARGSDEF void cargs_set_error(cargs_error error, const char *format, ...);
CARGSDEF cargs_error cargs_get_error(void);
CARGSDEF const char *cargs_get_error_message(void);
CARGSDEF void cargs_log_error(FILE *stream);

CARGSDEF void cargs_set_program_description(const char *description);
CARGSDEF int cargs_parse(int argc, char **argv);
CARGSDEF void cargs_reset(void);

CARGSDEF struct cargs_subcommand *cargs_get_active_subcommand(void);
CARGSDEF struct cargs_subcommand *cargs_get_subcommand(struct cargs_subcommand *parent, const char *name);
CARGSDEF void cargs_print_help(FILE *stream, struct cargs_subcommand *cmd);


#endif // CARGS_H

#ifdef CARGS_IMPLEMENTATION

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

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
    const char *short_name;
    void *reference;
    const char *argument;
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
    const char *program_description;

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

CARGSDEF struct cargs_flag *cargs__new_flag(enum cargs_flag_type type, const char *long_name, const char *short_name, const char *argument, validation_func_t *validation_func, const char *description);
CARGSDEF void cargs__panic_func(const char *file, int line, const char *message, ...);
#define cargs__panic(message, ...) cargs__panic_func(__FILE__, __LINE__, message, ##__VA_ARGS__)
CARGSDEF void cargs__check_duplicate_flags(struct cargs_flag *head, const char *long_name, const char *short_name);
CARGSDEF bool cargs__find_and_set_subcommand(struct cargs_subcommand *head, const char *name, struct cargs_subcommand **cmd);
CARGSDEF bool cargs__find_and_set_flag(struct cargs_flag *head, const char *name, struct cargs_flag **flag);
CARGSDEF bool cargs__parse_flag_value(struct cargs_flag *flag, const char *token);

#define CARGS_FLAG(enum_type, function_suffix, type_identifier) \
    void cargs_##function_suffix(const char *long_name, const char *short_name, type_identifier *reference, const type_identifier default_value, const char *argument, validation_func_t *validation_func, const char *description) \
    { \
        struct cargs_flag *flag = cargs__new_flag(enum_type, long_name, short_name, argument, validation_func, description); \
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
    if (cargs_state.error == CARGS_OK) return;
    fprintf(stream, "Error: %s\n", cargs_state.error_message);
}

CARGSDEF void cargs_set_program_description(const char *description)
{
    if (description == NULL) cargs__panic("program description must not be NULL");
    cargs_state.program_description = description;
}

CARGSDEF int cargs_parse(int argc, char **argv)
{
    if (argc == 0 || argv == NULL) cargs__panic("no arguments to parse, check argc/argv");
    if (cargs_state.parsed) cargs__panic("arguments already parsed");
    if (cargs_state.current_subcommand != NULL) cargs__panic("unterminated subcommand '%s'", cargs_state.current_subcommand->name);

    enum CARGS_PARSE_STATE {
        STATE_FETCH_TOKEN,
        STATE_CLASSIFY_TOKEN,
        STATE_EVALUATE_LONG_FLAG,
        STATE_EVALUATE_SHORT_CLUSTER,
        STATE_POSITIONAL_FETCH_CHECK,
        STATE_GET_FLAG_VALUE,
        STATE_EVALUATE_FLAG_VALUE,
        STATE_PARSING_COMPLETE,
    };
    enum CARGS_PARSE_STATE state = STATE_FETCH_TOKEN;

    int index = 1; // skip program name
    cargs_state.program_name = argv[0];

    char *token;
    struct cargs_flag *flag = NULL; // currently processed flag
    int first_positional = -1;
    bool positional_mode = false;

    while (state != STATE_PARSING_COMPLETE) {
        switch (state) {
            case STATE_FETCH_TOKEN: {
                if (index >= argc) {
                    state = STATE_PARSING_COMPLETE;
                    break;
                }
                token = argv[index++];
                state = STATE_CLASSIFY_TOKEN;
                break;
            }
            case STATE_CLASSIFY_TOKEN: {
                if (strcmp(token, "--") == 0) {
                    // Everything after '--' is positional, no validation needed
                    first_positional = index;
                    positional_mode = true;
                    index = argc; // skip remaining tokens
                    break;
                } else if (strncmp(token, "--", 2) == 0) {
                    state = STATE_EVALUATE_LONG_FLAG;
                } else if (token[0] == '-' && strlen(token) > 1) {
                    state = STATE_EVALUATE_SHORT_CLUSTER;
                } else {
                    // Check for subcommand
                    struct cargs_subcommand *subcmd = NULL;
                    struct cargs_subcommand *head = cargs_state.current_subcommand ?
                        cargs_state.current_subcommand->children_head :
                        cargs_state.root_subcommands_head;
                    if (cargs__find_and_set_subcommand(head, token, &subcmd)) {
                        cargs_state.current_subcommand = subcmd;
                        state = STATE_FETCH_TOKEN;
                    } else {
                        // This is the first positional argument
                        if (token[0] == '-') {
                            cargs_set_error(CARGS_POSITIONAL_STARTS_WITH_DASH, "unexpected option '%s'", token);
                            return -1;
                        }
                        first_positional = (index - 1);
                        positional_mode = true;
                        state = STATE_POSITIONAL_FETCH_CHECK;
                    }
                }
                break;
            }
            case STATE_EVALUATE_LONG_FLAG: {
                token += 2; // skip "--"
                char *equal_sign = strchr(token, '=');
                if (equal_sign != NULL) *equal_sign = '\0';

                // Look up flag in current scope (global or subcommand)
                struct cargs_flag **flag_head = cargs_state.current_subcommand ?
                    &cargs_state.current_subcommand->flags_head :
                    &cargs_state.global_flags_head;
                if (!cargs__find_and_set_flag(*flag_head, token, &flag)) {
                    cargs_set_error(CARGS_UNKNOWN_FLAG, "unknown flag: --%s", token);
                    return -1;
                }

                if (equal_sign != NULL) {
                    if (flag->argument == NULL) {
                        cargs_set_error(CARGS_UNEXPECTED_ARGUMENT, "flag --%s does not take an argument", flag->long_name);
                        return -1;
                    }
                    token = equal_sign + 1;
                    state = STATE_EVALUATE_FLAG_VALUE;
                } else {
                    if (flag->argument != NULL) {
                        state = STATE_GET_FLAG_VALUE;
                    } else {
                        // Boolean flag (or any without argument) set to true
                        if (flag->type == CARGS_FLAG_TYPE_BOOL) {
                            *(bool*)flag->reference = true;
                        }
                        state = STATE_FETCH_TOKEN;
                    }
                }
                break;
            }
            case STATE_EVALUATE_SHORT_CLUSTER: {
                const char *p = token + 1; // skip '-'
                while (*p != '\0') {
                    char sname[2] = { *p, '\0' };
                    struct cargs_flag *sflag = NULL;
                    struct cargs_flag *head = cargs_state.current_subcommand ?
                        cargs_state.current_subcommand->flags_head :
                        cargs_state.global_flags_head;
                    if (!cargs__find_and_set_flag(head, sname, &sflag)) {
                        cargs_set_error(CARGS_UNKNOWN_FLAG, "unknown flag: -%c", *p);
                        return -1;
                    }

                    if (sflag->argument != NULL) {
                        // Flag requires an argument
                        if (*(p + 1) != '\0') {
                            // Rest of cluster is its argument
                            if (!cargs__parse_flag_value(sflag, p + 1)) return -1;
                            p += strlen(p); // skip the rest
                        } else {
                            // Last char – next token is the argument
                            flag = sflag;
                            state = STATE_GET_FLAG_VALUE;
                            // We must exit this case; the while loop will break
                            goto finish_cluster;
                        }
                    } else {
                        // No argument – set boolean true (or whatever the default)
                        if (sflag->type == CARGS_FLAG_TYPE_BOOL) {
                            *(bool*)sflag->reference = true;
                        }
                        p++;
                    }
                }
                finish_cluster:
                if (state == STATE_EVALUATE_SHORT_CLUSTER) // not changed inside loop
                    state = STATE_FETCH_TOKEN;
                break;
            }
            case STATE_POSITIONAL_FETCH_CHECK: {
                // Continue checking remaining positionals
                while (index < argc) {
                    token = argv[index];
                    if (token[0] == '-') {
                        cargs_set_error(CARGS_POSITIONAL_STARTS_WITH_DASH, "unexpected option '%s'", token);
                        return -1;
                    }
                    index++;
                }
                // All done
                break;
            }
            case STATE_GET_FLAG_VALUE: {
                if (index >= argc) {
                    cargs_set_error(CARGS_MISSING_ARGUMENT, "missing argument for --%s",
                                    flag->long_name ? flag->long_name : flag->short_name);
                    return -1;
                }
                token = argv[index++];
                state = STATE_EVALUATE_FLAG_VALUE;
                break;
            }
            case STATE_EVALUATE_FLAG_VALUE: {
                if (!cargs__parse_flag_value(flag, token)) return -1;
                state = STATE_FETCH_TOKEN;
                break;
            }
            case STATE_PARSING_COMPLETE:
            default:
                break;
        }
    }

    cargs_state.parsed = true;
    return positional_mode ? first_positional : argc;
}

CARGSDEF void cargs_reset(void)
{
    // Simple reset: clear all flags/subcommands, reuse static pools
    cargs_state.global_flags_head = NULL;
    cargs_state.root_subcommands_head = NULL;
    cargs_state.current_subcommand = NULL;
    cargs_state.flags_allocated = 0;
    cargs_state.subcommands_allocated = 0;
    cargs_state.error = CARGS_OK;
    cargs_state.error_message[0] = '\0';
    cargs_state.parsed = false;
    // program_name and program_description can persist (or reset if desired)
}

CARGSDEF struct cargs_subcommand *cargs_get_active_subcommand(void)
{
    return cargs_state.current_subcommand;
}

CARGSDEF struct cargs_subcommand *cargs_get_subcommand(struct cargs_subcommand *parent, const char *name)
{
    if (name == NULL) return NULL;

    struct cargs_subcommand *head = (parent != NULL) ? parent->children_head : cargs_state.root_subcommands_head;

    while (head != NULL) {
        if (strcmp(head->name, name) == 0) return head;
        head = head->next;
    }

    cargs__panic("subcommand '%s' not found", name);
    return NULL; // suppress compiler warning
}

CARGSDEF void cargs_print_help(FILE *stream, struct cargs_subcommand *cmd)
{
    char prefix[512] = {0};
    snprintf(prefix, sizeof (prefix), "%s", cargs_state.program_name);

    // traverse execution path to build prefix
    if (cmd != NULL) {
        struct cargs_subcommand *chain[CARGS_MAX_SUBCOMMANDS];
        int depth = 0;
        struct cargs_subcommand *curr = cmd;

        while (curr != NULL) {
            chain[depth++] = curr;
            curr = curr->parent;
        }

        for (int i = depth - 1; i >= 0; i--) {
            size_t p_len = strlen(prefix);
            snprintf(prefix + p_len, sizeof(prefix) - p_len, " %s", chain[i]->name);
        }
    }

    struct cargs_subcommand *children = cmd != NULL ? cmd->children_head : cargs_state.root_subcommands_head;
    struct cargs_flag *flags = cmd != NULL ? cmd->flags_head : cargs_state.global_flags_head;

    // print usage
    fprintf(stream, "Usage: %s", prefix);
    if (flags != NULL) fprintf(stream, " [<options>]");
    if (children != NULL) fprintf(stream, " [<command>]");
    fprintf(stream, "\n\n");

    // print command description
    if (cmd != NULL) {
        fprintf(stream, "%s\n\n", cmd->description);
    } else if (cargs_state.program_description != NULL) {
        fprintf(stream, "%s\n\n", cargs_state.program_description);
    }

    // print child subcommands
    if (children != NULL) {
        int max_cmd_len = 0;
        struct cargs_subcommand *curr = children;
        while (curr != NULL) {
            int len = strlen(curr->name);
            if (len > max_cmd_len) max_cmd_len = len;
            curr = curr->next;
        }

        fprintf(stream, "Commands:\n");
        curr = children;
        while (curr != NULL) {
            fprintf(stream, "  %-*s  %s\n", max_cmd_len, curr->name, curr->description);
            curr = curr->next;
        }
        fprintf(stream, "\n");
    }

    // print flags
    if (flags != NULL) {
        int max_flag_len = 0;
        struct cargs_flag *curr = flags;
        while (curr != NULL) {
            int len = 0;
            if (curr->short_name && curr->long_name) len += 4;
            else if (curr->short_name) len += 3;
            if (curr->long_name) len += strlen(curr->long_name) + 3;
            if (curr->argument) len += strlen(curr->argument) + 1;
            if (len > max_flag_len) max_flag_len = len;
            curr = curr->next;
        }

        fprintf(stream, "Options:\n");
        curr = flags;
        while (curr != NULL) {
            fprintf(stream, "  ");

            int cur_len = 0;
            if (curr->short_name && curr->long_name) {
                fprintf(stream, "-%c, ", curr->short_name[0]);
                cur_len += 4;
            } else if (curr->short_name) {
                fprintf(stream, "-%c ", curr->short_name[0]);
                cur_len += 3;
            }

            if (curr->long_name) {
                fprintf(stream, "--%s ", curr->long_name);
                cur_len += strlen(curr->long_name) + 3;
            }

            if (curr->argument) {
                fprintf(stream, "%s ", curr->argument);
                cur_len += strlen(curr->argument) + 1;
            }

            int pad = max_flag_len - cur_len;

            fprintf(stream, "%*s%s\n", pad, "", curr->description);

            curr = curr->next;
        }
        fprintf(stream, "\n");
    }
}



CARGSDEF struct cargs_flag *cargs__new_flag(enum cargs_flag_type type, const char *long_name, const char *short_name, const char *argument, validation_func_t *validation_func, const char *description)
{
    if (cargs_state.parsed) cargs__panic("arguments already parsed");
    if (cargs_state.flags_allocated >= CARGS_MAX_FLAGS) cargs__panic("too many flags, define bigger CARGS_MAX_FLAGS");
    if (long_name == NULL && short_name == NULL) cargs__panic("flag long_name and short_name cannot both be NULL");
    if (long_name)
        for (size_t i = 0; i < strlen(long_name); i++)
            if (!isalnum(long_name[i])) cargs__panic("flag long_name can only contain alphanumeric characters");
    if (short_name && strlen(short_name) != 1) cargs__panic("flag short_name must be a single character");
    if (short_name && !isalnum(short_name[0])) cargs__panic("flag short_name must be alphanumeric");
    if (description == NULL) cargs__panic("flag description cannot be NULL");
    if (long_name && short_name && strlen(long_name) == 1 && long_name[0] == short_name[0]) cargs__panic("flag long_name and short_name cannot be the same");

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
    flag->argument = argument;
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

CARGSDEF void cargs__check_duplicate_flags(struct cargs_flag *head, const char *long_name, const char *short_name)
{
    while (head != NULL) {
        if (long_name  && head->long_name  && strcmp(head->long_name, long_name) == 0) cargs__panic("duplicate flag name: %s", long_name);
        if (short_name && head->short_name && head->short_name[0] == short_name[0]) cargs__panic("duplicate flag name: %c", short_name[0]);
        if (long_name  && head->short_name && head->short_name[0] == long_name[0]) cargs__panic("duplicate flag name: %s", long_name);
        if (short_name && head->long_name  && head->long_name[0]  == short_name[0]) cargs__panic("duplicate flag name: %c", short_name[0]);
        head = head->next;
    }
}

CARGSDEF bool cargs__find_and_set_subcommand(struct cargs_subcommand *head, const char *name, struct cargs_subcommand **cmd)
{
    struct cargs_subcommand *curr = head;
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0) {
            *cmd = curr;
            return true;
        }
        curr = curr->next;
    }
    return false;
}

CARGSDEF bool cargs__find_and_set_flag(struct cargs_flag *head, const char *name, struct cargs_flag **flag)
{
    struct cargs_flag *curr = head;
    while (curr != NULL) {
        if (curr->long_name && strcmp(curr->long_name, name) == 0) {
            *flag = curr;
            return true;
        }
        if (curr->short_name && strcmp(curr->short_name, name) == 0) {
            *flag = curr;
            return true;
        }
        curr = curr->next;
    }
    return false;
}

CARGSDEF bool cargs__parse_flag_value(struct cargs_flag *flag, const char *token)
{
    if (flag->type == CARGS_FLAG_TYPE_BOOL) {
        if (strcmp(token, "true") == 0 || strcmp(token, "1") == 0) {
            *(bool*)flag->reference = true;
        } else if (strcmp(token, "false") == 0 || strcmp(token, "0") == 0) {
            *(bool*)flag->reference = false;
        } else {
            cargs_set_error(CARGS_INVALID_BOOL, "invalid boolean value '%s' for --%s", token, flag->long_name ? flag->long_name : flag->short_name);
            return false;
        }
    } else if (flag->type == CARGS_FLAG_TYPE_INT || flag->type == CARGS_FLAG_TYPE_UINT || flag->type == CARGS_FLAG_TYPE_FLOAT) {
        char *endptr;
        if (flag->type == CARGS_FLAG_TYPE_INT) {
            long val = strtol(token, &endptr, 0);
            if (endptr == token || *endptr != '\0') {
                cargs_set_error(CARGS_INVALID_INT, "invalid integer '%s' for --%s", token, flag->long_name ? flag->long_name : flag->short_name);
                return false;
            }
            if (val > INT_MAX || val < INT_MIN) {
                cargs_set_error(CARGS_INVALID_INT, "integer out of range '%s'", token);
                return false;
            }
            *(int*)flag->reference = (int)val;
        } else if (flag->type == CARGS_FLAG_TYPE_UINT) {
            unsigned long val = strtoul(token, &endptr, 0);
            if (endptr == token || *endptr != '\0') {
                cargs_set_error(CARGS_INVALID_UINT, "invalid unsigned integer '%s'", token);
                return false;
            }
            if (val > UINT_MAX) {
                cargs_set_error(CARGS_INVALID_UINT, "unsigned integer out of range '%s'", token);
                return false;
            }
            *(unsigned int*)flag->reference = (unsigned int)val;
        } else { // float
            float val = strtof(token, &endptr);
            if (endptr == token || *endptr != '\0') {
                cargs_set_error(CARGS_INVALID_FLOAT, "invalid float '%s'", token);
                return false;
            }
            *(float*)flag->reference = val;
        }
    } else if (flag->type == CARGS_FLAG_TYPE_CHAR) {
        if (strlen(token) != 1) {
            cargs_set_error(CARGS_INVALID_CHAR, "expected single character for --%s, got '%s'", flag->long_name ? flag->long_name : flag->short_name, token);
            return false;
        }
        *(char*)flag->reference = token[0];
    } else if (flag->type == CARGS_FLAG_TYPE_STRING) {
        *(const char**)flag->reference = token;
    }

    if (flag->validation_func) {
        const char *name = flag->long_name ? flag->long_name : flag->short_name;
        if (!flag->validation_func(name, token)) {
            // validation_func should have set error via cargs_set_error
            return false;
        }
    }
    return true;
}

#endif // CARGS_IMPLEMENTATION
