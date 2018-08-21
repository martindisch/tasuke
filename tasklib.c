#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include "tasklib.h"
#include "tasklist.h"

/*
 * Private helper functions
 */

static int has_trailing_slash(const char *path) {
    // Iterate over string until we find the end
    while (*path != '\0') path++;
    // Compare last character (before terminator) with slash
    return *(path - 1) == '/';
}

/*
 * Public helper functions
 */

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
        dir_cpy = malloc((strlen(dir) + 9) * sizeof(char));
        // Build the new directory path
        sprintf(dir_cpy, dir_format, dir);
    } else {
        // Allocate memory for simple copy of dir
        dir_cpy = malloc((strlen(dir) + 1) * sizeof(char));
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
    if (stat(dir_cpy, &buffer) == -1 || S_ISDIR(buffer.st_mode) == 0) {
        // If not, create it
        if (mkdir(dir_cpy, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
            // There was an error, free memory and return
            free(dir_cpy);
            return NULL;
        }
    }

    /*
     * Build full path to file
     */
    // Allocate memory for full path (freed by user)
    char *file = malloc((strlen(dir_cpy) + strlen(list) + 6) * sizeof(char));
    // Choose format based on whether there is a trailing slash already
    const char *path_format =
        has_trailing_slash(dir_cpy) ? "%s%s.txt" : "%s/%s.txt";
    // Build the full path
    sprintf(file, path_format, dir_cpy, list);

    // Free the string copy we made earlier
    free(dir_cpy);

    return file;
}

char **get_files(const char *dir, char **lists) {
    // Get number of lists by iterating over array until NULL terminator found
    int i;
    for (i = 0; lists[i]; i++);

    // Build path array
    char **files;
    if (i == 0) {
        // No lists given, allocate memory for default list + terminator
        files = malloc(2 * sizeof(char *));
        // Build path to default list
        if ((files[0] = get_file(dir, NULL)) == NULL) {
            // Free the empty array
            free(files);
            return NULL;
        }
        // Add terminator
        files[1] = NULL;
    } else {
        // Some lists were given, allocate memory for them + terminator
        files = malloc((i + 1) * sizeof(char *));
        // Iterate over lists, building paths
        int y;
        for (y = 0; y < i; y++) {
            if ((files[y] = get_file(dir, lists[y])) == NULL) {
                // Free paths we've already acquired at this point
                for (y--; y >= 0; y--) {
                    free(files[y]);
                }
                // Free the empty array
                free(files);
                return NULL;
            }
        }
        // Add terminator
        files[i] = NULL;
    }

    return files;
}

/*
 * Commands
 */

const char *add(const char *file, char **tasks) {
    // Open file in append mode
    FILE *fp;
    if ((fp = fopen(file, "a")) == NULL) {
        return "Unable to open task list\n";
    }

    // Write all tasks to file
    while (*tasks) {
        if (fprintf(fp, "%s\n", *tasks) < 0) {
            fclose(fp);
            return "Unable to write to file\n";
        }
        tasks++;
    }

    // Close file
    if (fclose(fp) == EOF) {
        return "Unable to close file\n";
    }

    return NULL;
}

const char *done(const char *file, char **positions) {
    return "Not yet implemented\n";
}

const char *list(char **files) {
    // Iterate over path array until terminator is encountered
    while (*files) {
        // Initialize TaskList ADT
        TaskList list = tasklist_init(*files);
        // Attempt reading current list
        char *error = tasklist_read(list, *files);
        if (error) {
            tasklist_destroy(list);
            return error;
        }
        // If there was no problem, print it
        tasklist_print(list);
        // Cleanup
        tasklist_destroy(list);
        files++;
    }

    return NULL;
}
