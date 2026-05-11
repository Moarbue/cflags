# cargs

A robust, POSIX-compliant, single-header command-line argument parsing library for C99. 

`cargs` implements a deterministic Finite State Machine (FSM) to process arguments without dynamic memory allocation. It supports infinite-depth subcommand nesting, short flag clustering, strict GNU-style delimiter assignment, mandatory positional mapping, and automated help generation.

## Features

* **Single-Header Integration:** Drop `cargs.h` into your project. No build system dependencies.
* **Zero Dynamic Memory:** Operates entirely on bounded static memory pools. No `malloc` or `free`.
* **POSIX/GNU Syntax:** Supports `--flag=value`, `--flag value`, `-f value`, inline clusters `-abc`, inline assignment `-fValue`, and `--` positional termination.
* **Subcommand FSM:** Define isolated argument scopes for nested execution trees (e.g., `cmd remote add --tags=stable`).
* **Automatic Positional Mapping:** Securely bind mandatory positional arguments without manual `argc` index bounds checking.
* **Extended Type Parsing:** Native resolution for `bool`, `int`, `unsigned int`, `int64_t`, `uint64_t`, `float`, `double`, `char`, `string`, and `size_t` (e.g., `1.5M`, `2G`).
* **Safety & Validation:** Prevents negative integer wraparound, catches overflow out-of-bounds, and supports custom validation callback injection.

## Integration

Define the required static pool capacities prior to inclusion in exactly *one* C source file.

```c
#define CARGS_MAX_FLAGS 50
#define CARGS_MAX_SUBCOMMANDS 20
#define CARGS_MAX_POSITIONALS 20
#define CARGS_IMPLEMENTATION
#include "cargs.h"

```

## Quick Start

```c
#define CARGS_MAX_FLAGS 20
#define CARGS_MAX_SUBCOMMANDS 10
#define CARGS_MAX_POSITIONALS 10
#define CARGS_IMPLEMENTATION
#include "cargs.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    // Global scope flags
    bool help_flag;
    const char *config_path;

    cargs_set_program_description("vcs - Distributed version control system mockup");
    cargs_bool("help", "h", &help_flag, false, NULL, NULL, "Print global help message");
    cargs_string("config", "C", &config_path, "~/.vcsconfig", "<file>", NULL, "Custom config path");

    // Subcommand scope: clone
    bool clone_verbose, clone_help;
    const char *clone_branch;
    const char *clone_url;
    
    cargs_subcommand_start("clone", "Clone a repository into a new directory");
        cargs_positional("url", &clone_url, "The repository to clone");
        cargs_bool("help", "h", &clone_help, false, NULL, NULL, "Print clone help");
        cargs_bool("verbose", "V", &clone_verbose, false, NULL, NULL, "Clone verbosely");
        cargs_string("branch", "b", &clone_branch, "main", "<branch>", NULL, "Target branch");
    cargs_subcommand_end();

    // Execute FSM
    int opt_idx = cargs_parse(argc, argv);
    struct cargs_subcommand *active_cmd = cargs_get_active_subcommand();

    // Handle Parsing Failure (automatically catches missing positionals)
    if (opt_idx == -1) {
        cargs_log_error(stderr);
        cargs_print_help(stderr, active_cmd);
        return 1;
    }

    // Evaluate Global Triggers
    if (help_flag || argc == 1) {
        cargs_print_help(stdout, NULL);
        return 0;
    }

    // Evaluate Subcommands
    if (active_cmd && strcmp(active_cmd->name, "clone") == 0) {
        if (clone_help) {
            cargs_print_help(stdout, active_cmd);
            return 0;
        }
        printf("Cloning %s into branch %s. Verbose: %d\n", clone_url, clone_branch, clone_verbose);
    }

    return 0;
}

```

## API Reference

### Configuration Macros

Must be defined before `#include "cargs.h"`.

* `CARGS_MAX_FLAGS`: Total number of flags across all scopes.
* `CARGS_MAX_SUBCOMMANDS`: Total number of subcommands.
* `CARGS_MAX_POSITIONALS`: Total number of mandatory positional arguments.

### Flag Definition

Flags must be defined before calling `cargs_parse`. Scoping is determined by the currently open `cargs_subcommand_start` block.

```c
void cargs_bool(const char *long_name, const char *short_name, bool *reference, const bool default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_int(const char *long_name, const char *short_name, int *reference, const int default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_uint(const char *long_name, const char *short_name, unsigned int *reference, const unsigned int default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_int64(const char *long_name, const char *short_name, int64_t *reference, const int64_t default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_uint64(const char *long_name, const char *short_name, uint64_t *reference, const uint64_t default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_size(const char *long_name, const char *short_name, size_t *reference, const size_t default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_float(const char *long_name, const char *short_name, float *reference, const float default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_double(const char *long_name, const char *short_name, double *reference, const double default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_char(const char *long_name, const char *short_name, char *reference, const char default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_string(const char *long_name, const char *short_name, const char **reference, const char *default_value, const char *argument, validation_func_t *validation_func, const char *description);

```

### Positional Arguments

Register mandatory positional arguments. The FSM maps them sequentially after flag evaluation.

```c
void cargs_positional(const char *name, const char **reference, const char *description);

```

### Subcommand Management

```c
// Open a new subcommand scope
void cargs_subcommand_start(const char *name, const char *description);

// Close the current scope, returning to the parent scope
void cargs_subcommand_end(void);

```

### Execution & State

```c
// Parses argv array. Returns the starting index of optional positional arguments, or -1 on error.
int cargs_parse(int argc, char **argv);

// Returns a pointer to the active terminal subcommand.
struct cargs_subcommand *cargs_get_active_subcommand(void);

// Flushes the FSM state for consecutive re-parsing.
void cargs_reset(void);

```

### Diagnostics

```c
// Prints error reason to stream.
void cargs_log_error(FILE *stream);

// Auto-generates format-aligned help text based on the provided subcommand scope.
// Pass NULL for global scope.
void cargs_print_help(FILE *stream, struct cargs_subcommand *cmd);

// Retrieves the raw error enum for custom handling.
cargs_error cargs_get_error(void);

```

## Error Codes

Evaluate `cargs_get_error()` against the following enum if `cargs_parse` returns `-1`:

* `CARGS_UNKNOWN_FLAG`
* `CARGS_UNKNOWN_SUBCOMMAND`
* `CARGS_MISSING_ARGUMENT`
* `CARGS_MISSING_POSITIONAL`
* `CARGS_UNEXPECTED_ARGUMENT`
* `CARGS_INVALID_BOOL`
* `CARGS_INVALID_INT`
* `CARGS_INVALID_UINT`
* `CARGS_INVALID_INT64`
* `CARGS_INVALID_UINT64`
* `CARGS_INVALID_FLOAT`
* `CARGS_INVALID_DOUBLE`
* `CARGS_INVALID_SIZE`
* `CARGS_INVALID_CHAR`
* `CARGS_INVALID_ARGUMENT` (Triggered by custom validation callback)
* `CARGS_POSITIONAL_STARTS_WITH_DASH`
