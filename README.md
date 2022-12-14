# c-flags

A simple one-header-file library for parsing command-line flags. \
This library is heavily inspired by **[tsodings](https://github.com/tsoding/)** version: https://github.com/tsoding/flag.h \
which is inspired by golang's **[flag module](https://pkg.go.dev/flag)**.

## Installation

Just include **[flags.h](https://github.com/Moarbue/c-flags/blob/main/flags.h)** in your project and you're good to go.

## Usage

Creating a new flag:
```C

// name: name of the flag
// desc: short description of the flag
// def: default value of the flag
bool *flag = cflag_bool(const char *name, const char *desc, const bool def);

char **flag2 = cflag_string(const char *name, const char *desc, const char *def);

...

```
Setting bounds for the flags:
```C

cflag_int_minmax   (int      *flag, int      min, int      max);
cflag_uint64_minmax(uint64_t *flag, uint64_t min, uint64_t max);
cflag_float_minmax (float    *flag, float    min, float    max);

```

Parse the flags:
```C

// Checks if incomming flags match any of the created flags. Returns false on error
if (!cflag_parse(argc, argv)) {
    // Logs a useful error message to the standard out stream
    cflag_log_error(stdout);

    // Alternatively you can handle the errors by yourself
    cflag_error err = cflag_get_error();

    // All existing errors are inside the enum cflag_errors and all
    // start with the prefix CFLAG_ERROR
    switch (err.error) {
        case CFLAG_ERROR_UNKNOWN:
            ...
        break;
    
        ...
    }
}


```

Log all available options (to view output see example.c):
```C

// You can also print a useful message, where all the names, descriptions and optionally
// default values and bounds of the created flags are printed
//               stream   printdefault  printminmax
cflag_log_options(stdout, true,         true);

```

For an example program checkout **[example.c](https://github.com/Moarbue/c-flags/blob/main/example.c)**. \
Compile and run it like so:
```

./build_example.sh

./example -h

```