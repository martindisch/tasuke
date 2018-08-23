#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "tasklib.h"

static const char *usage =
    "Usage: %1$s [-s directory] [LIST]...\n"
    "  or   %1$s -a [-n list] [-s directory] TASK...\n"
    "  or   %1$s -i [-n list] [-s directory] POSITION TASK\n"
    "  or   %1$s -d [-n list] [-s directory] POSITION...\n"
    "  or   %1$s -m [-n list] [-s directory] PREV_POS NEW_POS\n"
    "  or   %1$s -r [-s directory] [LIST]...\n"
    "Manage your todo/task lists with this small utility.\n"
    "\n"
    "Options:\n"
    "  -a            Add tasks by appending them to a list\n"
    "  -d            Delete tasks\n"
    "  -h            Print usage information\n"
    "  -i            Insert a task into a list at a specific position\n"
    "  -m            Move a task inside a list from one position to another\n"
    "  -n list       Select a specific list for your current operation\n"
    "  -r            Remove task lists\n"
    "  -s directory  Select a specific directory to store task lists\n"
    "\n"
    "Copyright (c) 2018 Martin Disch <martindisch@gmail.com>\n"
    "Project website <https://github.com/martindisch/tasuke>\n";

int main(int argc, char **argv) {
    /*
     * Flags & option argument variables for user input
     */
    // Flags without option argument
    int aflg = 0, iflg = 0, dflg = 0, mflg = 0, rflg = 0;
    // Flags with option argument
    int nflg = 0, sflg = 0;
    // Set to 1 if there is a problem parsing options
    int errflg = 0;
    // Pointers to option arguments
    const char *nvalue = NULL, *svalue = NULL;

    /*
     * Simple argument parsing, mostly just setting flags.
     * Would be simpler with argp, but we need to use getopt for portability.
     */
    int c;
    while ((c = getopt(argc, argv, "aidmrhn:s:")) != -1) {
        switch (c) {
            case 'a':
                aflg = 1;
                break;
            case 'i':
                iflg = 1;
                break;
            case 'd':
                dflg = 1;
                break;
            case 'm':
                mflg = 1;
                break;
            case 'r':
                rflg = 1;
                break;
            case 'h':
                printf(usage, argv[0]);
                exit(EXIT_SUCCESS);
            case 'n':
                nflg = 1;
                nvalue = optarg;
                break;
            case 's':
                sflg = 1;
                svalue = optarg;
                break;
            case ':':
                // Option flag without required option argument
                errflg = 1;
                break;
            case '?':
                // Unrecognized option
                errflg = 1;
                break;
        }
    }

    /*
     * Several checks for parsing problems and bad usage of flags
     */
    if (
        // Problem noticed by getopt
        errflg ||
        // Mutually exclusive flags
        aflg + iflg + dflg + mflg + rflg > 1 ||
        // -r doesn't have -n option
        nflg + rflg > 1 ||
        // -n can't occur on its own
        nflg > aflg + iflg + dflg + mflg
    ) {
        fprintf(stderr, usage, argv[0]);
        exit(EXIT_FAILURE);
    }

    /*
     * Get the filename(s) the commands will need
     */
    char *file = NULL, **files = NULL;
    if (aflg + iflg + dflg + mflg) {
        // These commands use only a single task list
        if ((file = get_file(svalue, nvalue)) == NULL) {
            fprintf(stderr, "Unable to access directory\n");
            exit(EXIT_FAILURE);
        }
    } else {
        // The other commands (list list(s), remove list(s)) may need several
        if ((files = get_files(svalue, &argv[optind])) == NULL) {
            fprintf(stderr, "Unable to access directory\n");
            exit(EXIT_FAILURE);
        }
    }

    /*
     * Handle command
     */
    const char *error = NULL;
    if (aflg) {
        error = add(file, &argv[optind]);
    } else if (iflg) {
        error = insert(file, &argv[optind]);
    } else if (dflg) {
        error = done(file, &argv[optind]);
    } else if (mflg) {
        error = "Not yet implemented\n";
    } else if (rflg) {
        error = delete(files);
    } else {
        // No command flag (= list command)
        error = list(files);
    }

    /*
     * Free file name(s)
     */
    if (file) {
        free(file);
    }
    if (files) {
        // Free all paths in files array
        int i;
        for (i = 0; files[i]; i++) {
            free(files[i]);
        }
        // Free the array itself
        free(files);
    }

    /*
     * If there was a problem executing the command, exit with error
     */
    if (error) {
        fprintf(stderr, error);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
