# c-flags

A simple, C99-compatible, single-header library for parsing command-line flags.

## Installation

Just include **[cflag.h](cflag.h)** in your project:

```c
#define CFLAG_IMPLEMENTATION
#include "cflag.h"
```

## Usage

The library supports both internal storage (managed by the library) and external storage (binding to your own variables).

### 1. Creating Flags

#### Internal Storage
The library allocates memory and returns a pointer to the value.
```c
// bool *flag = cflag_bool(name, description, default_value);
bool *verbose = cflag_bool("-v", "Enable verbose output", false);
int *count    = cflag_int("-c", "Number of iterations", 10);
char **file   = cflag_string("-f", "Input file path", "input.txt");
```

#### External Storage (Reference Binding)
The library updates your existing variables directly.
```c
int timeout = 30;
cflag_int_ref("-t", "Connection timeout", &timeout, 30);

bool debug = false;
cflag_bool_ref("-d", "Enable debug mode", &debug, false);
```

### 2. Supported Types
The library supports a wide range of types:
- **Booleans**: `cflag_bool`
- **Integers**: `cflag_int` (platform dependent), `cflag_int8`, `cflag_int16`, `cflag_int32`, `cflag_int64` (and unsigned versions)
- **Floating Point**: `cflag_float`, `cflag_double`, `cflag_long_double`
- **Strings**: `cflag_string`
- **Characters**: `cflag_char`

### 3. Parsing Flags

Use `cflag_parse` to process `argc` and `argv`. It returns `false` if an unknown flag is encountered or a value is missing/invalid.

```c
if (!cflag_parse(argc, argv)) {
    // Log the error to stderr
    cflag_log_error(stderr);
    
    // Or handle the error manually
    cflag_error err = cflag_get_error();
    if (err.error == CFLAG_ERROR_UNKNOWN) {
        printf("Unknown flag: %s\n", err.flag);
    }
}
```

### 4. Logging Options

You can automatically generate a help menu listing all registered flags.

```c
// stream, print_defaults
cflag_log_options(stdout, true);
```

## Example

For a complete demonstration, see **[example.c](example.c)**.

Compile and run:
```bash
gcc example.c -o example
./example -v -l 50 -t 0.75 -f logs.txt
```
