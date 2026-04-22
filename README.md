# cargs

A simple, C99-compatible, single-header library for parsing command-line flags and positional arguments.

## Installation

Just include **[cargs.h](cargs.h)** in your project:

```c
#define CARGS_IMPLEMENTATION
#include "cargs.h"
```

## Usage

The library supports both internal storage (managed by the library) and external storage (binding to your own variables).

### 1. Creating Flags

#### Internal Storage
The library allocates memory and returns a pointer to the value.
```c
// bool *flag = cargs_bool(name, description, default_value);
bool *verbose = cargs_bool("-v", "Enable verbose output", false);
int *count    = cargs_int("-c", "Number of iterations", 10);
char **file   = cargs_string("-f", "Input file path", "input.txt");
```

#### External Storage (Reference Binding)
The library updates your existing variables directly.
```c
int timeout = 30;
cargs_int_ref("-t", "Connection timeout", &timeout, 30);

bool debug = false;
cargs_bool_ref("-d", "Enable debug mode", &debug, false);
```

### 2. Positional Arguments

You can define mandatory or optional positional arguments that are filled in the order they appear on the command line.

```c
// char **arg = cargs_positional(name, description, is_mandatory);
char **input = cargs_positional("input", "The input file", true);
char **output = cargs_positional("output", "The output file", false);
```

### 3. Supported Types

The library supports a wide range of types:
- **Booleans**: `cargs_bool`
- **Integers**: `cargs_int` (platform dependent), `cargs_int8`, `cargs_int16`, `cargs_int32`, `cargs_int64` (and unsigned versions)
- **Floating Point**: `cargs_float`, `cargs_double`, `cargs_long_double`
- **Strings**: `cargs_string`
- **Characters**: `cargs_char`
- **Positionals**: `cargs_positional`

### 4. Parsing Arguments

Use `cargs_parse` to process `argc` and `argv`. It returns `false` if an unknown flag is encountered, a value is missing/invalid, or a mandatory positional argument is missing.

```c
if (!cargs_parse(argc, argv)) {
    // Log the error to stderr
    cargs_log_error(stderr);
    
    // Or handle the error manually
    cargs_error err = cargs_get_error();
    if (err.error == CARGS_ERROR_UNKNOWN) {
        printf("Unknown flag: %s\n", err.flag);
    }
}
```

### 5. Logging Options

You can automatically generate a help menu listing all registered flags and positional arguments.

```c
// stream, print_defaults
cargs_log_options(stdout, true);
```

## Example

For a complete demonstration, see **[example.c](example.c)**.

Compile and run:
```bash
gcc example.c -o example
./example -v -l 50 -t 0.75 input.txt
```
