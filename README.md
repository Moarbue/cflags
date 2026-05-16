# cargs

A robust, POSIX-compliant, single-header command-line argument parsing library for C99. 

`cargs` implements a deterministic Finite State Machine (FSM) to process arguments without dynamic memory allocation. It supports infinite-depth subcommand nesting, short flag clustering, strict GNU-style delimiter assignment, mandatory and optional positional mapping, automated help generation with word wrapping, and toggleable default-value display.

## Contents

- [Features](#features)
- [Integration](#integration)
- [Quick Start](#quick-start)
- [API Reference](#api-reference)
  - [Configuration Macros](#configuration-macros)
  - [Flag Definition](#flag-definition)
  - [Positional Arguments](#positional-arguments)
  - [Subcommand Management](#subcommand-management)
  - [Execution & State](#execution--state)
  - [Diagnostics & Help](#diagnostics--help)
  - [Validation Callback](#validation-callback)
- [Error Codes](#error-codes)
- [License](#license)

## Features

* **Single-Header Integration:** Drop `cargs.h` into your project. No build system dependencies.
* **Zero Dynamic Memory:** Operates entirely on bounded static memory pools. No `malloc` or `free`.
* **POSIX/GNU Syntax:** Supports `--flag=value`, `--flag value`, `-f value`, inline clusters `-abc`, inline assignment `-fValue`, and `--` positional termination.
* **Subcommand FSM:** Define isolated argument scopes for nested execution trees (e.g., `cmd remote add --tags=stable`).
* **Mandatory & Optional Positionals:** Securely bind required arguments and provide defaults for optional ones.
* **Extended Type Parsing:** Native resolution for `bool`, `int`, `unsigned int`, `int64_t`, `uint64_t`, `float`, `double`, `char`, `string`, and `size_t` with suffix parsing (e.g., `1.5M`, `2G`).
* **Safety & Validation:** Prevents negative integer wraparound, catches overflow out-of-bounds, and supports custom validation callback injection.
* **Rich Help Output:** Auto-wraps descriptions at configurable width (`CARGS_HELP_WIDTH`). Optionally displays default values via `cargs_set_show_defaults`.

## Integration

Define the required static pool capacities prior to inclusion in exactly *one* C source file.

```c
#define CARGS_MAX_FLAGS 50
#define CARGS_MAX_SUBCOMMANDS 20
#define CARGS_MAX_POSITIONALS 20
#define CARGS_IMPLEMENTATION
#include "cargs.h"
```

Additional configuration macros (all optional):

| Macro | Default | Description |
|-------|---------|-------------|
| `CARGS_HELP_WIDTH` | 80 | Column width for help text wrapping |
| `CARGS_MAX_DEFAULT_VALUE_LEN` | 64 | Buffer size for default-value strings |
| `CARGS_MAX_ERROR_LEN` | 512 | Buffer size for error messages |

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
    const char *clone_dir;

    cargs_subcommand_start("clone", "Clone a repository into a new directory");
        cargs_mandatory_positional("url", &clone_url, "The repository to clone");
        cargs_optional_positional("directory", &clone_dir, ".", "Target directory (default: current)");
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
        printf("Cloning %s into branch %s, directory %s. Verbose: %d\n",
               clone_url, clone_branch, clone_dir, clone_verbose);
    }

    return 0;
}
```

## API Reference

### Configuration Macros

Define before `#include "cargs.h"`.

| Macro | Purpose |
|-------|---------|
| `CARGS_MAX_FLAGS` | Total flags across all scopes |
| `CARGS_MAX_SUBCOMMANDS` | Total subcommands |
| `CARGS_MAX_POSITIONALS` | Total positional arguments |
| `CARGS_HELP_WIDTH` | Help text wrap width (default: 80) |
| `CARGS_MAX_DEFAULT_VALUE_LEN` | Default string buffer size (default: 64) |
| `CARGS_MAX_ERROR_LEN` | Error message buffer size (default: 512) |

### Flag Definition

Flags must be defined before calling `cargs_parse`. Scoping is determined by the currently open `cargs_subcommand_start` block. All reference pointers must be non‑NULL.

```c
void cargs_bool(const char *long_name, const char *short_name, bool *reference, bool default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_int(const char *long_name, const char *short_name, int *reference, int default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_uint(const char *long_name, const char *short_name, unsigned int *reference, unsigned int default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_int64(const char *long_name, const char *short_name, int64_t *reference, int64_t default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_uint64(const char *long_name, const char *short_name, uint64_t *reference, uint64_t default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_size(const char *long_name, const char *short_name, size_t *reference, size_t default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_float(const char *long_name, const char *short_name, float *reference, float default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_double(const char *long_name, const char *short_name, double *reference, double default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_char(const char *long_name, const char *short_name, char *reference, char default_value, const char *argument, validation_func_t *validation_func, const char *description);
void cargs_string(const char *long_name, const char *short_name, const char **reference, const char *default_value, const char *argument, validation_func_t *validation_func, const char *description);
```

- Either `long_name` or `short_name` may be `NULL` (but not both).
- `argument` is a placeholder string displayed in help (e.g., `"<file>"`); can be `NULL`.
- `validation_func` is optional; receives the flag name and a `void*` to the parsed value. Must return `true` to accept, or `false` after setting an error with `cargs_set_error`.

### Positional Arguments

Define positional arguments in the order they should appear. Mandatory positionals must come before optional ones; violation causes a panic.

```c
void cargs_mandatory_positional(const char *name, const char **reference, const char *description);
void cargs_optional_positional(const char *name, const char **reference, const char *default_value, const char *description);
```

- `reference` must be non‑NULL.
- For optional positionals, `default_value` is assigned if the user does not supply the argument. It may be `NULL` (resulting in a `NULL` assignment).

### Subcommand Management

```c
void cargs_subcommand_start(const char *name, const char *description);
void cargs_subcommand_end(void);
```

- Nesting depth is limited only by `CARGS_MAX_SUBCOMMANDS`.
- Global flags are only recognised outside subcommands; subcommand flags are isolated.

### Execution & State

```c
int cargs_parse(int argc, char **argv);
```

Parses `argv`, returns the index of the first positional argument (or `argc` if none). Returns `-1` on error; use `cargs_get_error()` to retrieve the error code and `cargs_get_error_message()` for a human-readable string.

```c
struct cargs_subcommand *cargs_get_active_subcommand(void);
```

Returns the deepest subcommand reached during parsing, or `NULL` if no subcommand was entered.

```c
struct cargs_subcommand *cargs_get_subcommand(struct cargs_subcommand *parent, const char *name);
```

Retrieves a subcommand by name from the given parent (or from the root if `parent` is `NULL`). Panics if not found.

```c
void cargs_reset(void);
```

Flushes all defined flags, subcommands, and positionals so the library can be reused for a fresh parse.

### Diagnostics & Help

```c
void cargs_log_error(FILE *stream);
void cargs_print_help(FILE *stream, struct cargs_subcommand *cmd);
```

- `cargs_print_help` generates formatted help for the specified subcommand (or global scope if `NULL`). Help text wraps at `CARGS_HELP_WIDTH` and includes `[default: ...]` annotations unless suppressed via `cargs_set_show_defaults(false)`.

```c
cargs_error cargs_get_error(void);
const char *cargs_get_error_message(void);
```

- After a failed parse, retrieve the error code and message.

```c
void cargs_set_error(cargs_error error, const char *format, ...);
```

- Used inside custom validation functions to signal a failure.

```c
void cargs_set_show_defaults(bool show);
```

- Toggles the display of `[default: ...]` in help output. Default is `true`.

### Validation Callback

```c
typedef bool (validation_func_t)(const char *name, const void *value);
```

- `name`: the flag’s long name if available, else its short name.
- `value`: pointer to the parsed value (e.g., `const char **` for strings, `int *` for ints).

## Error Codes

Evaluate `cargs_get_error()` after a failing parse:

| Code | Meaning |
|------|---------|
| `CARGS_OK` | No error |
| `CARGS_UNKNOWN_FLAG` | Flag not recognised in current scope |
| `CARGS_UNKNOWN_SUBCOMMAND` | Subcommand not found (currently only used in navigation) |
| `CARGS_MISSING_ARGUMENT` | A non‑bool flag expects a value but none provided |
| `CARGS_MISSING_POSITIONAL` | Required positional argument not supplied |
| `CARGS_UNEXPECTED_ARGUMENT` | A flag that does not take an argument received one |
| `CARGS_INVALID_BOOL` | Boolean value not recognised (expects `true`/`false`/`1`/`0`) |
| `CARGS_INVALID_INT` | Invalid or out‑of‑range integer |
| `CARGS_INVALID_UINT` | Invalid or out‑of‑range unsigned integer |
| `CARGS_INVALID_INT64` | Invalid or out‑of‑range 64‑bit integer |
| `CARGS_INVALID_UINT64` | Invalid or out‑of‑range 64‑bit unsigned integer |
| `CARGS_INVALID_SIZE` | Invalid size suffix or value |
| `CARGS_INVALID_FLOAT` | Invalid or out‑of‑range float |
| `CARGS_INVALID_DOUBLE` | Invalid or out‑of‑range double |
| `CARGS_INVALID_CHAR` | Character flag given a multi‑character value |
| `CARGS_INVALID_ARGUMENT` | Custom validation failed |
| `CARGS_POSITIONAL_STARTS_WITH_DASH` | A positional argument looks like a flag |

## License

MIT License. See the header of `cargs.h`.
