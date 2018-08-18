#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static const char *usage =
    "Usage:\n"
    "   tasuke [LIST...]\n"
    "   tasuke -a [-n list] [-s directory] TASK...\n"
    "   tasuke -i [-n list] [-s directory] TASK POSITION\n"
    "   tasuke -d [-n list] [-s directory] POSITION...\n"
    "   tasuke -m [-n list] [-s directory] PREV_POS NEW_POS\n"
    "   tasuke -r [-s directory] LIST...\n";

int main(int argc, char **argv) {
    // Flags without option argument
    int aflg = 0, iflg = 0, dflg = 0, mflg = 0, rflg = 0;
    // Flags with option argument
    int nflg = 0, sflg = 0;
    // Set to 1 if there is a problem parsing options
    int errflg = 0;
    // Pointers to option arguments
    char *nvalue = NULL, *svalue = NULL;

    int c;
    while ((c = getopt(argc, argv, "aidmrn:s:")) != -1) {
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
            case 'n':
                nflg = 1;
                nvalue = optarg;
                break;
            case 's':
                sflg = 1;
                nvalue = optarg;
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

    // Check if there was a problem and usage should be shown
    if (errflg ||                               // Problem parsing
        aflg + iflg + dflg + mflg + rflg > 1 || // Exclusive flags
        nflg + rflg > 1) {                      // -r doesn't have -n option
        fprintf(stderr, usage);
        exit(EXIT_FAILURE);
    }

    // Otherwise print out the positional arguments
    for ( ; optind < argc; optind++) {
        printf("Found argument: %s\n", argv[optind]);
    }

    exit(EXIT_SUCCESS);
}
