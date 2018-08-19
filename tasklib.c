#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "tasklib.h"

static int has_trailing_slash(const char *);

const char *add(const char *file, char **tasks) {
    printf("Adding tasks to file %s\n", file);
    while (*tasks != NULL) {
        printf("Element %s\n", *tasks);
        tasks++;
    }
    return NULL;
}

char *get_file(const char *dir, const char *list) {
    /*
     * Use default values if dir or list have not been set
     */
    // Pointer where we will build our copy of dir
    char *dir_cpy;
    // Find the user home by checking $HOME and falling back to getpwuid
    if (!dir) {
        if ((dir = getenv("HOME")) == NULL) {
            dir = getpwuid(getuid())->pw_dir;
        }
        // Prepare format to append the .tasuke directory to the path
        const char *dir_format =
            has_trailing_slash(dir) ? "%s.tasuke" : "%s/.tasuke";
        // Allocate memory to copy in dir & the default .tasuke
        dir_cpy = calloc(strlen(dir) + 9, sizeof(char));
        // Build the new directory path
        sprintf(dir_cpy, dir_format, dir);
    } else {
        // Allocate memory for simple copy of dir
        dir_cpy = calloc(strlen(dir) + 1, sizeof(char));
        // Since the user supplied a custom directory, just use this one
        strcpy(dir_cpy, dir);
    }
    // If no list name was set, use the default
    if (!list) {
        list = "todo";
    }

    /*
     * Check if the directory still exists and try to create it if not
     */
    struct stat buffer;
    // Does directory exist?
    if (!(stat(dir_cpy, &buffer) == 0 && S_ISDIR(buffer.st_mode))) {
        // If not, create it
        if (mkdir(dir_cpy, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
            // There was an error, free memory and return
            free(dir_cpy);
            return NULL;
        }
    }

    /*
     * Build full path to file
     */
    // Allocate memory for full path (freed by user)
    char *file = calloc(strlen(dir_cpy) + strlen(list) + 6, sizeof(char));
    // Choose format based on whether there is a trailing slash already
    const char *path_format =
        has_trailing_slash(dir_cpy) ? "%s%s.txt" : "%s/%s.txt";
    // Build the full path
    sprintf(file, path_format, dir_cpy, list);

    // Free the string copy we made earlier
    free(dir_cpy);

    return file;
}

static int has_trailing_slash(const char *path) {
    // Iterate over string until we find the end
    while (*path != '\0') path++;
    // Compare last character (before terminator) with slash
    return *(path - 1) == '/';
}
