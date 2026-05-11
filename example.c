#define CARGS_MAX_FLAGS 20
#define CARGS_MAX_SUBCOMMANDS 10
#define CARGS_MAX_POSITIONALS 10
#define CARGS_IMPLEMENTATION
#include "cargs.h"

#include <stdio.h>
#include <string.h>

static void exec_clone(bool verbose, const char *branch, const char *url) {
    printf("[Clone] URL: %s | Branch: %s | Verbose: %d\n", url, branch, verbose);
}

static void exec_remote_add(const char *name, const char *url, const char *branch, const char *tags) {
    printf("[Remote Add] Name: %s | URL: %s | Branch: %s | Tags: %s\n",
           name, url, branch, tags ? tags : "none");
}

int main(int argc, char *argv[])
{
    bool version_flag, help_flag;
    const char *config_path;

    cargs_set_program_description("vcs - Distributed version control system mockup");
    cargs_bool("version", "v", &version_flag, false, NULL, NULL, "Print version");
    cargs_bool("help", "h", &help_flag, false, NULL, NULL, "Print global help message");
    cargs_string("config", "C", &config_path, "~/.vcsconfig", "<file>", NULL, "Custom config path");

    bool clone_verbose, clone_help;
    const char *clone_branch;
    const char *clone_url;

    cargs_subcommand_start("clone", "Clone a repository into a new directory");
        cargs_positional("url", &clone_url, "The repository to clone");
        cargs_bool("help", "h", &clone_help, false, NULL, NULL, "Print clone help");
        cargs_bool("verbose", "V", &clone_verbose, false, NULL, NULL, "Clone verbosely");
        cargs_string("branch", "b", &clone_branch, "main", "<branch>", NULL, "Target branch");
    cargs_subcommand_end();

    bool remote_verbose, remote_help;
    cargs_subcommand_start("remote", "Manage remote repositories");
        cargs_bool("help", "h", &remote_help, false, NULL, NULL, "Print remote help");
        cargs_bool("verbose", "V", &remote_verbose, false, NULL, NULL, "Remote operations verbosely");

        bool remote_add_help;
        const char *remote_add_tags;
        const char *remote_add_branch;
        const char *remote_add_name;
        const char *remote_add_url;

        cargs_subcommand_start("add", "Add a new remote repository");
            cargs_positional("name", &remote_add_name, "Short name of the new remote");
            cargs_positional("url", &remote_add_url, "Remote repository URL");
            cargs_bool("help", "h", &remote_add_help, false, NULL, NULL, "Print remote add help");
            cargs_string("tags", "t", &remote_add_tags, NULL, "<tags>", NULL, "Tags to attach");
            cargs_string("branch", "b", &remote_add_branch, "main", "<branch>", NULL, "Upstream branch");
        cargs_subcommand_end();

    cargs_subcommand_end();

    int pos_idx = cargs_parse(argc, argv);
    struct cargs_subcommand *active_cmd = cargs_get_active_subcommand();

    if (pos_idx == -1) {
        cargs_log_error(stderr);
        cargs_print_help(stderr, active_cmd);
        return 1;
    }

    if (help_flag || argc == 1) {
        cargs_print_help(stdout, NULL);
        return 0;
    }
    if (version_flag) {
        printf("vcs version 1.0.0\n");
        return 0;
    }

    if (active_cmd == NULL) {
        fprintf(stderr, "Error: No command specified.\n");
        return 1;
    }

    if (strcmp(active_cmd->name, "clone") == 0) {
        if (clone_help) {
            cargs_print_help(stdout, active_cmd);
            return 0;
        }
        exec_clone(clone_verbose, clone_branch, clone_url);
    }
    else if (strcmp(active_cmd->name, "add") == 0 && active_cmd->parent && strcmp(active_cmd->parent->name, "remote") == 0) {
        if (remote_add_help) {
            cargs_print_help(stdout, active_cmd);
            return 0;
        }
        exec_remote_add(remote_add_name, remote_add_url, remote_add_branch, remote_add_tags);
    }
    else if (strcmp(active_cmd->name, "remote") == 0) {
        if (remote_help) {
            cargs_print_help(stdout, active_cmd);
            return 0;
        }
        printf("Remote verbosity: %d. Specify 'add' to mutate remotes.\n", remote_verbose);
    }

    return 0;
}
