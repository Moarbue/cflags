// This file demonstrates every major feature of the cargs library
// and documents the parsing behavior in detail.
//
// ----------------------------------------------------------------
// FLAG PARSING RULES
// ----------------------------------------------------------------
// Long flags (--flag):
//   --flag           : for bool flags, sets to true.
//   --flag=VALUE     : for any flag type, assigns VALUE.
//   --flag VALUE     : for non-bool flags, assigns VALUE from next token.
//                      Bool flags do NOT consume a following token.
//
// Short flags (-f):
//   -f               : for bool flags, sets to true.
//   -fVALUE          : for any flag (including bool: -f0 / -f1), VALUE
//                      is taken from the rest of the cluster.
//   -f VALUE         : for non-bool flags, assigns VALUE from next token.
//                      Bool flags ignore VALUE and just set to true.
//   -abc             : clustering. If -a is non-bool, then "bc" becomes
//                      its argument. If -a is bool, -a is set to true
//                      and the remaining characters are processed.
//
// Boolean flag values:
//   true/false, 1/0 are accepted (case-sensitive) with --flag=...
//   or short -f1 / -f0. With no value, bool flags become true.
//
// Token "--" ends flag parsing; everything after is treated as positional.
//
// ----------------------------------------------------------------
// POSITIONAL ARGUMENTS
// ----------------------------------------------------------------
// Declared with cargs_mandatory_positional (required) or
// cargs_optional_positional (falls back to a default if not supplied).
// They are matched in the order they are defined. Mandatory args
// must come before optional args in the definition order (enforced).
//
// Positional arguments must not start with '-' unless preceded by "--".
//
// ----------------------------------------------------------------
// SUBCOMMANDS
// ----------------------------------------------------------------
// Subcommands are defined with cargs_subcommand_start/end. They can be
// nested. Flags and positionals are scoped to the current subcommand.
// Global flags are only recognised outside any subcommand.
//
// ----------------------------------------------------------------
// HELP & ERROR HANDLING
// ----------------------------------------------------------------
// cargs_print_help prints nicely formatted help for any level.
// cargs_parse returns the index of the first positional argument,
// or -1 on error. Use cargs_get_error / cargs_get_error_message
// to inspect errors.
//
// ----------------------------------------------------------------
// CUSTOM VALIDATION
// ----------------------------------------------------------------
// A validation_func_t can be provided for any flag; it receives the
// flag name and a pointer to the parsed value. If it returns false,
// parsing stops and the error set by the function is used.
// ----------------------------------------------------------------

#define CARGS_MAX_FLAGS 20
#define CARGS_MAX_SUBCOMMANDS 15
#define CARGS_MAX_POSITIONALS 10
#define CARGS_IMPLEMENTATION
#include "cargs.h"

#include <stdio.h>
#include <string.h>

// ------------------------------------------------------------
// Custom validation function for a flag
// ------------------------------------------------------------
static bool validate_port(const char *name, const void *value) {
    int port = *(const int*)value;
    if (port < 1 || port > 65535) {
        // The library will use this error message if validation fails.
        cargs_set_error(CARGS_INVALID_ARGUMENT,
                        "invalid port number %d for -%s (must be 1-65535)",
                        port, name);
        return false;
    }
    return true;
}

// ------------------------------------------------------------
// Command implementations (mock)
// ------------------------------------------------------------
static void exec_clone(bool verbose, const char *branch,
                      const char *url, const char *directory) {
    printf("[Clone] URL: %s | Branch: %s | Directory: %s | Verbose: %d\n",
           url, branch, directory, verbose);
}

static void exec_remote_add(const char *name, const char *url,
                            const char *branch, const char *tags) {
    printf("[Remote Add] Name: %s | URL: %s | Branch: %s | Tags: %s\n",
           name, url, branch, tags ? tags : "none");
}

static void exec_log(int max_count, bool oneline) {
    printf("[Log] Showing last %d commits%s\n",
           max_count, oneline ? " (oneline format)" : "");
}

static void exec_push(const char *remote, const char *branch, bool force) {
    printf("[Push] Remote: %s | Branch: %s | Force: %s\n",
           remote ? remote : "origin",
           branch ? branch : "HEAD",
           force ? "yes" : "no");
}

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
int main(int argc, char *argv[])
{
    // --------------------------------------------------------
    // Global flags
    // --------------------------------------------------------
    bool version_flag, help_flag;
    const char *config_path;

    cargs_set_program_description(
        "vcs - Distributed version control system mockup");
    cargs_set_show_defaults(true);

    // bool flag: can be given as --version, -v (sets to true),
    // or --version=false / --version=true / -v0 / -v1.
    cargs_bool("version", "v", &version_flag, false, NULL, NULL,
               "Print version information");
    cargs_bool("help", "h", &help_flag, false, NULL, NULL,
               "Print global help message");

    // string flag: accepts --config=FILE, --config FILE, -C FILE,
    // or -C=FILE (short flag with = is also allowed).
    // Default is shown in help as [default: ~/.vcsconfig].
    cargs_string("config", "C", &config_path, "~/.vcsconfig",
                 "<file>", NULL, "Custom config file");

    // --------------------------------------------------------
    // Subcommand: clone
    // --------------------------------------------------------
    bool clone_verbose, clone_help;
    const char *clone_branch;
    const char *clone_url;
    const char *clone_dir;   // optional

    cargs_subcommand_start("clone",
        "Clone a repository into a new directory");
        // Mandatory positional: must be provided or parse error.
        // Example: vcs clone https://example.com/repo.git
        cargs_mandatory_positional("url", &clone_url,
                                   "The repository to clone");
        // Optional positional: if omitted, directory becomes ".".
        // Example: vcs clone https://example.com/repo.git mydir
        //            (url + directory)
        //           vcs clone https://example.com/repo.git
        //            (only url, directory defaults to ".")
        cargs_optional_positional("directory", &clone_dir, ".",
                                  "Target directory (default: current)");

        // Flags for clone; all parsing rules apply.
        cargs_bool("help", "h", &clone_help, false, NULL, NULL,
                   "Print clone help");
        cargs_bool("verbose", "V", &clone_verbose, false, NULL, NULL,
                   "Clone verbosely");
        // -b or --branch can be given in any of these ways:
        //   --branch=feature
        //   --branch feature
        //   -b feature
        //   -bfeature
        cargs_string("branch", "b", &clone_branch, "main",
                     "<branch>", NULL, "Target branch");
    cargs_subcommand_end();

    // --------------------------------------------------------
    // Subcommand: log
    // --------------------------------------------------------
    bool log_oneline, log_help;
    const char *log_count_str;  // default value, filled by optional positional

    cargs_subcommand_start("log", "Show commit history");
        // Optional positional: if given, used as count; else default 10.
        // Since positionals are always strings, we later convert with atoi.
        cargs_optional_positional("count", &log_count_str,
                                  "10",
                                  "Number of commits to display");
        // Flag with only a long name (no short).
        cargs_bool("oneline", NULL, &log_oneline, false, NULL, NULL,
                   "Show commits in oneline format");
        cargs_bool("help", "h", &log_help, false, NULL, NULL,
                   "Print log help");
    cargs_subcommand_end();

    // --------------------------------------------------------
    // Subcommand: remote (contains nested "add")
    // --------------------------------------------------------
    bool remote_verbose, remote_help;
    cargs_subcommand_start("remote", "Manage remote repositories");
        cargs_bool("help", "h", &remote_help, false, NULL, NULL,
                   "Print remote help");
        cargs_bool("verbose", "V", &remote_verbose, false, NULL, NULL,
                   "Remote operations verbosely");

        // --- remote add ---
        bool remote_add_help;
        const char *remote_add_tags;
        const char *remote_add_branch;
        const char *remote_add_name;
        const char *remote_add_url;

        cargs_subcommand_start("add", "Add a new remote repository");
            // Two mandatory positionals.
            // Example: vcs remote add origin https://...
            cargs_mandatory_positional("name", &remote_add_name,
                                       "Short name of the new remote");
            cargs_mandatory_positional("url", &remote_add_url,
                                       "Remote repository URL");
            cargs_bool("help", "h", &remote_add_help, false, NULL, NULL,
                       "Print remote add help");
            // tags flag: may be given as --tags="v1.0" or --tags "v1.0"
            // or -t v1.0, -tv1.0.
            cargs_string("tags", "t", &remote_add_tags, NULL,
                         "<tags>", NULL, "Tags to attach");
            cargs_string("branch", "b", &remote_add_branch, "main",
                         "<branch>", NULL, "Upstream branch");
        cargs_subcommand_end();

    cargs_subcommand_end();

    // --------------------------------------------------------
    // Subcommand: push
    // --------------------------------------------------------
    bool push_force, push_help;
    const char *push_remote;
    const char *push_branch;

    cargs_subcommand_start("push", "Push changes to a remote");
        // Both positionals are optional; missing → defaults used.
        cargs_optional_positional("remote", &push_remote, "origin",
                                  "Remote name");
        cargs_optional_positional("branch", &push_branch, "HEAD",
                                  "Branch to push");
        // Short flag -f, long --force.
        cargs_bool("force", "f", &push_force, false, NULL, NULL,
                   "Force push");
        cargs_bool("help", "h", &push_help, false, NULL, NULL,
                   "Print push help");
    cargs_subcommand_end();

    // --------------------------------------------------------
    // Subcommand: serve (custom validation)
    // --------------------------------------------------------
    bool serve_help;
    int serve_port = 8080;  // default

    cargs_subcommand_start("serve", "Start a simple HTTP server");
        // int flag with only a short name (-p) and no long name.
        // Requires a value: -p 9090, -p9090, --port=9090 (not available).
        // The validate_port callback ensures 1-65535.
        cargs_int(NULL, "p", &serve_port, 8080, "<port>",
                  validate_port, "Listening port");
        cargs_bool("help", "h", &serve_help, false, NULL, NULL,
                   "Print serve help");
    cargs_subcommand_end();

    // --------------------------------------------------------
    // Parsing
    // --------------------------------------------------------
    int pos_idx = cargs_parse(argc, argv);
    struct cargs_subcommand *active_cmd = cargs_get_active_subcommand();

    if (pos_idx == -1) {
        cargs_log_error(stderr);
        cargs_print_help(stderr, active_cmd);
        return 1;
    }

    // Global flags.
    if (help_flag || argc == 1) {
        cargs_print_help(stdout, NULL);   // top-level help
        return 0;
    }
    if (version_flag) {
        printf("vcs version 1.0.0\n");
        return 0;
    }

    if (active_cmd == NULL) {
        fprintf(stderr, "Error: No command specified.\n");
        cargs_print_help(stderr, NULL);
        return 1;
    }

    // --------------------------------------------------------
    // Dispatch
    // --------------------------------------------------------
    if (strcmp(active_cmd->name, "clone") == 0) {
        if (clone_help) {
            cargs_print_help(stdout, active_cmd);
            return 0;
        }
        exec_clone(clone_verbose, clone_branch, clone_url, clone_dir);
    }
    else if (strcmp(active_cmd->name, "log") == 0) {
        if (log_help) {
            cargs_print_help(stdout, active_cmd);
            return 0;
        }
        int count = atoi(log_count_str);
        if (count <= 0) count = 10;
        exec_log(count, log_oneline);
    }
    else if (strcmp(active_cmd->name, "add") == 0 &&
             active_cmd->parent &&
             strcmp(active_cmd->parent->name, "remote") == 0) {
        if (remote_add_help) {
            cargs_print_help(stdout, active_cmd);
            return 0;
        }
        exec_remote_add(remote_add_name, remote_add_url,
                        remote_add_branch, remote_add_tags);
    }
    else if (strcmp(active_cmd->name, "remote") == 0) {
        if (remote_help) {
            cargs_print_help(stdout, active_cmd);
            return 0;
        }
        printf("Remote verbosity: %d. Specify 'add' to mutate remotes.\n",
               remote_verbose);
    }
    else if (strcmp(active_cmd->name, "push") == 0) {
        if (push_help) {
            cargs_print_help(stdout, active_cmd);
            return 0;
        }
        exec_push(push_remote, push_branch, push_force);
    }
    else if (strcmp(active_cmd->name, "serve") == 0) {
        if (serve_help) {
            cargs_print_help(stdout, active_cmd);
            return 0;
        }
        printf("Serving on port %d\n", serve_port);
    }
    else {
        fprintf(stderr, "Unknown command: %s\n", active_cmd->name);
        cargs_print_help(stderr, NULL);
        return 1;
    }

    return 0;
}
