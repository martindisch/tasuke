#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <errno.h>
#include "tasklib.h"
#include "tasklist.h"

/*
 * Private helper functions
 */

static int has_trailing_slash(const char *path) {
    // Iterate over string until we find the end
    while (*path != '\0') ++path;
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
    for (i = 0; lists[i]; ++i);

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
        for (y = 0; y < i; ++y) {
            if ((files[y] = get_file(dir, lists[y])) == NULL) {
                // Free paths we've already acquired at this point
                for (--y; y >= 0; --y) {
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

const char *tasklib_add(const char *file, char **tasks) {
    // Open file in append mode
    FILE *fp;
    if ((fp = fopen(file, "a")) == NULL) {
        return "Unable to open list\n";
    }

    // Write all tasks to file
    for ( ; *tasks; ++tasks) {
        if (fprintf(fp, "%s\n", *tasks) < 0) {
            fclose(fp);
            return "Unable to write to list\n";
        }
    }

    // Close file
    if (fclose(fp) == EOF) {
        return "Unable to close list\n";
    }

    return NULL;
}

const char *tasklib_insert(const char *file, char **position_task) {
    /*
     * Extract position and task argument, checking for sanity
     */
    long position = -1;
    const char *task = NULL;
    int i;
    for (i = 0; *position_task; ++position_task, ++i) {
        if (i == 0) {
            // Extract position
            errno = 0;
            char *endptr;
            position = strtol(*position_task, &endptr, 10);
            // Handle conversion error
            if (errno || *endptr != '\0') {
                return "Position not a number\n";
            }
        } else if (i == 1) {
            // Extract task text
            task = *position_task;
        } else {
            // There is an additional invalid argument
            return "Too many arguments\n";
        }
    }
    // Abort if we don't have all required arguments
    if (position == -1 || task == NULL) {
        return "Not enough arguments\n";
    }

    /*
     * Use TaskList to handle the insertion
     */
    // Build TaskList ADT
    TaskList list = tasklist_init(file);
    // Try reading the list
    const char *error = tasklist_read(list, file);
    if (error) {
        tasklist_destroy(list);
        return error;
    }
    // Try inserting the task
    error = tasklist_insert(list, position, task);
    if (error) {
        tasklist_destroy(list);
        return error;
    }
    // Try writing the updated list to file
    error = tasklist_write(list, file);
    tasklist_destroy(list);

    return error;
}

const char *tasklib_done(const char *file, char **posargs) {
    // Determine number of positional arguments
    int length;
    for (length = 0; posargs[length]; ++length);
    // Create array to store converted positions
    long positions[length + 1];
    // Set terminator element
    positions[length] = -1;
    // Reset errno to use it for error checking in strtol
    errno = 0;
    // Iterate over all positional arguments, building array of positions
    int i;
    for (i = 0; i < length; ++i) {
        char *endptr;
        long position = strtol(posargs[i], &endptr, 10);
        // Handle conversion error
        if (errno || *endptr != '\0') {
            return "Position not a number\n";
        }
        positions[i] = position;
    }

    // Build TaskList ADT
    TaskList list = tasklist_init(file);
    // Try reading the list
    const char *error = tasklist_read(list, file);
    if (error) {
        tasklist_destroy(list);
        return error;
    }
    // Try deleting tasks
    error = tasklist_done(list, positions);
    if (error) {
        tasklist_destroy(list);
        return error;
    }
    // Try writing the updated list to file
    error = tasklist_write(list, file);
    tasklist_destroy(list);

    return error;
}

const char *tasklib_list(char **files) {
    // Iterate over path array until terminator is encountered
    for ( ; *files; ++files) {
        // Initialize TaskList ADT
        TaskList list = tasklist_init(*files);
        // Attempt reading current list
        const char *error = tasklist_read(list, *files);
        if (error) {
            tasklist_destroy(list);
            return error;
        }
        // If there was no problem, print it
        tasklist_print(list);
        // Cleanup
        tasklist_destroy(list);
    }

    return NULL;
}

const char *tasklib_move(const char *file, char **from_to) {
    /*
     * Extract position arguments, checking for sanity
     */
    long from_pos = -1, to_pos = -1;
    int i;
    for (i = 0; *from_to; ++from_to, ++i) {
        if (i == 0) {
            // Extract from position
            errno = 0;
            char *endptr;
            from_pos = strtol(*from_to, &endptr, 10);
            // Handle conversion error
            if (errno || *endptr != '\0') {
                return "Position not a number\n";
            }
        } else if (i == 1) {
            // Extract to position
            errno = 0;
            char *endptr;
            to_pos = strtol(*from_to, &endptr, 10);
            // Handle conversion error
            if (errno || *endptr != '\0') {
                return "Position not a number\n";
            }
        } else {
            // There is an additional invalid argument
            return "Too many arguments\n";
        }
    }
    // Abort if we don't have all required arguments
    if (from_pos == -1 || to_pos == -1) {
        return "Not enough arguments\n";
    }

    /*
     * Use TaskList to handle the insertion
     */
    // Build TaskList ADT
    TaskList list = tasklist_init(file);
    // Try reading the list
    const char *error = tasklist_read(list, file);
    if (error) {
        tasklist_destroy(list);
        return error;
    }
    // Try inserting the task
    error = tasklist_move(list, from_pos, to_pos);
    if (error) {
        tasklist_destroy(list);
        return error;
    }
    // Try writing the updated list to file
    error = tasklist_write(list, file);
    tasklist_destroy(list);

    return error;
}

const char *tasklib_remove(char **files) {
    // Iterate over path array until terminator is encountered
    for ( ; *files; ++files) {
        // Attempt unlinking
        if (unlink(*files) != 0) {
            return "Unable to delete list\n";
        }
    }

    return NULL;
}
