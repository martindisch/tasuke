#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include "tasklib.h"
#include "tasklist.h"

/*
 * POSIX methods
 */

extern char *strdup(const char *);
extern char *strndup(const char *, size_t);
extern int strcasecmp(const char *, const char *);

/*
 * Private helper functions
 */

 /**
  * Returns whether the path ends with a slash.
  *
  * @param path The path to check
  * @return 0 if there is no trailing slash
  */
static int has_trailing_slash(const char *path) {
    /* Iterate over string until we find the end */
    while (*path != '\0') ++path;
    /* Compare last character (before terminator) with slash */
    return *(path - 1) == '/';
}

/**
* Converts a position string to long, handling all possible errors.
*
* @param posarg The string position to convert
* @return The position as a long or -1 on error
*/
static long strtopos(const char *posarg) {
    long position;
    /* Will point to first character that is not a digit */
    char *endptr;

    /* Reset errno to use it for strtol error checking */
    errno = 0;
    position = strtol(posarg, &endptr, 10);
    /* Handle conversion error */
    if (errno || *endptr != '\0') {
        return -1;
    }

    return position;
}

/**
 * Returns the name of a task list based on its filename.
 *
 * It works by simply returning a copy without the extension.
 * Because a new string needs to be allocated, the user must free it.
 *
 * @param path The list's filename
 * @return The list name (freed by user)
 */
static char *filename_to_name(const char *filename) {
    /* Find the last dot */
    const char *name_end = strrchr(filename, '.');
    /* Get number of characters in the name */
    int length = name_end - filename;
    /* Get a copy of the substring containing the list name */
    char *name = strndup(filename, length);

    return name;
}

/**
 * Compares two strings, ignoring case.
 *
 * This is a comparison function to be passed into qsort().
 *
 * @param s1 The first string
 * @param s2 The second string
 * @return Integer greater than, equal to or less than 0 depending on how
 *         s1 compares to s2.
 */
static int cmpstringp(const void *s1, const void *s2) {
    /* Cast & dereference to turn pointer to pointer to char
       into pointer to char */
    return strcasecmp(* (char * const *) s1, * (char * const *) s2);
}

/*
 * Public helper functions
 */

char *get_dir(const char *dir) {
    struct stat buffer;
    /* Pointer where we will build our copy of dir */
    char *dir_cpy;

    /*
     * Use default value if dir has not been set
     */
    /* Find the user home by checking $HOME and falling back to getpwuid */
    if (!dir) {
        const char *dir_format;
        if ((dir = getenv("HOME")) == NULL) {
            dir = getpwuid(getuid())->pw_dir;
        }
        /* Prepare format to append the .tasuke directory to the path */
        dir_format = has_trailing_slash(dir) ? "%s.tasuke" : "%s/.tasuke";
        /* Allocate memory to copy in dir & the default .tasuke */
        dir_cpy = malloc((strlen(dir) + 9) * sizeof(char));
        /* Build the new directory path */
        sprintf(dir_cpy, dir_format, dir);
    } else {
        /* Since the user supplied a directory, make a plain copy of this */
        dir_cpy = strdup(dir);
    }

    /*
     * Check if the directory still exists and try to create it if not
     */
    /* Does directory exist? */
    if (stat(dir_cpy, &buffer) == -1 || S_ISDIR(buffer.st_mode) == 0) {
        /* If not, create it */
        if (mkdir(dir_cpy, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
            /* There was an error, free memory and return */
            free(dir_cpy);
            return NULL;
        }
    }

    return dir_cpy;
}

char *get_file(const char *dir, const char *list) {
    char *file;
    const char *path_format;

    /*
     * Use default values if dir or list have not been set
     */
    /* Get the directory and create it if it doesn't exist yet */
    char *dir_cpy = get_dir(dir);
    /* Abort on error */
    if (dir_cpy == NULL) {
        return NULL;
    }
    /* If no list name was set, use the default */
    if (!list) {
        list = "todo";
    }

    /*
     * Build full path to file
     */
    /* Allocate memory for full path (freed by user) */
    file = malloc((strlen(dir_cpy) + strlen(list) + 6) * sizeof(char));
    /* Choose format based on whether there is a trailing slash already */
    path_format = has_trailing_slash(dir_cpy) ? "%s%s.txt" : "%s/%s.txt";
    /* Build the full path */
    sprintf(file, path_format, dir_cpy, list);

    /* Free the dir string we no longer need */
    free(dir_cpy);

    return file;
}

char **get_files(const char *dir, char **lists) {
    char **files;
    int i;

    /* Get number of lists by iterating over array until NULL terminator */
    for (i = 0; lists[i]; ++i);
    /* Build path array */
    if (i == 0) {
        /* No lists given, allocate memory for default list + terminator */
        files = malloc(2 * sizeof(char *));
        /* Build path to default list */
        if ((files[0] = get_file(dir, NULL)) == NULL) {
            /* Free the empty array */
            free(files);
            return NULL;
        }
        /* Add terminator */
        files[1] = NULL;
    } else {
        int y;
        /* Some lists were given, allocate memory for them + terminator */
        files = malloc((i + 1) * sizeof(char *));
        /* Iterate over lists, building paths */
        for (y = 0; y < i; ++y) {
            if ((files[y] = get_file(dir, lists[y])) == NULL) {
                /* Free paths we've already acquired at this point */
                for (--y; y >= 0; --y) {
                    free(files[y]);
                }
                /* Free the empty array */
                free(files);
                return NULL;
            }
        }
        /* Add terminator */
        files[i] = NULL;
    }

    return files;
}

/*
 * Commands
 */

const char *tasklib_add(const char *file, char **tasks, int verbose) {
    FILE *fp;

    /* Open file in append mode */
    if ((fp = fopen(file, "a")) == NULL) {
        return "Unable to open list\n";
    }

    /* Write all tasks to file */
    for ( ; *tasks; ++tasks) {
        if (fprintf(fp, "%s\n", *tasks) < 0) {
            fclose(fp);
            return "Unable to write to list\n";
        }
    }

    /* Close file */
    if (fclose(fp) == EOF) {
        return "Unable to close list\n";
    }

    /* Show new list */
    if (verbose) {
        /* Initialize TaskList ADT */
        TaskList list = tasklist_init(file);
        /* Attempt reading the list */
        const char *error = tasklist_read(list);
        if (error) {
            tasklist_destroy(list);
            return error;
        }
        /* If there was no problem, print it */
        tasklist_print(list);
        /* Cleanup */
        tasklist_destroy(list);
    }

    return NULL;
}

const char *tasklib_insert(
    const char *file, char **position_task, int verbose) {
    int i;
    long position = -1;
    const char *task = NULL;
    TaskList list;
    const char *error;

    /*
     * Extract position and task argument, checking for sanity
     */
    for (i = 0; *position_task; ++position_task, ++i) {
        if (i == 0) {
            /* Extract position */
            position = strtopos(*position_task);
            /* Handle conversion error */
            if (position == -1) {
                return "Position not a number\n";
            }
        } else if (i == 1) {
            /* Extract task text */
            task = *position_task;
        } else {
            /* There is an additional invalid argument */
            return "Too many arguments\n";
        }
    }
    /* Abort if we don't have all required arguments */
    if (position == -1 || task == NULL) {
        return "Not enough arguments\n";
    }

    /*
     * Use TaskList to handle the insertion
     */
    /* Build TaskList ADT */
    list = tasklist_init(file);
    /* Try reading the list */
    error = tasklist_read(list);
    if (error) {
        tasklist_destroy(list);
        return error;
    }
    /* Try inserting the task */
    error = tasklist_insert(list, position, task);
    if (error) {
        tasklist_destroy(list);
        return error;
    }
    /* Try writing the updated list to file */
    error = tasklist_write(list);
    if (error) {
        tasklist_destroy(list);
        return error;
    }
    /* Show the modified list */
    if (verbose) {
        tasklist_print(list);
    }
    tasklist_destroy(list);

    return NULL;
}

const char *tasklib_done(const char *file, char **posargs, int verbose) {
    int length;
    long *positions;
    int i;
    TaskList list;
    const char *error;

    /* Determine number of positional arguments */
    for (length = 0; posargs[length]; ++length);
    /* Create array to store converted positions */
    positions = malloc((length + 1) * sizeof(long));
    /* Set terminator element */
    positions[length] = -1;
    /* Iterate over all positional arguments, building array of positions */
    for (i = 0; i < length; ++i) {
        long position = strtopos(posargs[i]);
        /* Handle conversion error */
        if (position == -1) {
            return "Position not a number\n";
        }
        positions[i] = position;
    }

    /* Build TaskList ADT */
    list = tasklist_init(file);
    /* Try reading the list */
    error = tasklist_read(list);
    if (error) {
        tasklist_destroy(list);
        return error;
    }
    /* Try deleting tasks */
    error = tasklist_done(list, positions);
    if (error) {
        tasklist_destroy(list);
        return error;
    }
    /* Try writing the updated list to file */
    error = tasklist_write(list);
    if (error) {
        tasklist_destroy(list);
        return error;
    }
    /* Show the modified list */
    if (verbose) {
        tasklist_print(list);
    }
    tasklist_destroy(list);
    free(positions);

    return NULL;
}

const char *tasklib_names(const char *dir) {
    DIR *dp;
    struct dirent *ep;
    char **names = malloc(8 * sizeof(char *));
    int size = 8;
    int i = 0, y;

    /* Attempt opening the directory stream */
    if ((dp = opendir(dir)) == NULL) {
        return "Unable to open directory\n";
    }

    /* Read dir entries into array */
    while ((ep = readdir (dp))) {
        if (strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0) {
            /* Increase array size if necessary */
            if (i == size) {
                names = realloc(names, 2 * size * sizeof(char *));
                size *= 2;
            }
            /* Insert name into array */
            names[i++] = filename_to_name(ep->d_name);
        }
    }

    /* Sort array alphabetically */
    qsort(names, i, sizeof(char *), cmpstringp);

    /* Print list names */
    for (y = 0; y < i; ++y) {
        printf("%s\n", names[y]);
    }

    /* Cleanup */
    closedir(dp);
    for (i -= 1; i >= 0; --i) {
        free(names[i]);
    }
    free(names);

    return NULL;
}

const char *tasklib_list(char **files) {
    /* Iterate over path array until terminator is encountered */
    for ( ; *files; ++files) {
        /* Initialize TaskList ADT */
        TaskList list = tasklist_init(*files);
        /* Attempt reading current list */
        const char *error = tasklist_read(list);
        if (error) {
            tasklist_destroy(list);
            return error;
        }
        /* If there was no problem, print it */
        tasklist_print(list);
        /* Cleanup */
        tasklist_destroy(list);
        /* Print empty line if there is yet another list */
        if (*(files + 1)) {
            printf("\n");
        }
    }

    return NULL;
}

const char *tasklib_move(const char *file, char **from_to, int verbose) {
    int i;
    long from_pos = -1, to_pos = -1;
    TaskList list;
    const char *error;

    /*
     * Extract position arguments, checking for sanity
     */
    for (i = 0; *from_to; ++from_to, ++i) {
        if (i == 0) {
            /* Extract from position */
            from_pos = strtopos(*from_to);
            /* Handle conversion error */
            if (from_pos == -1) {
                return "Position not a number\n";
            }
        } else if (i == 1) {
            /* Extract to position */
            to_pos = strtopos(*from_to);
            /* Handle conversion error */
            if (to_pos == -1) {
                return "Position not a number\n";
            }
        } else {
            /* There is an additional invalid argument */
            return "Too many arguments\n";
        }
    }
    /* Abort if we don't have all required arguments */
    if (from_pos == -1 || to_pos == -1) {
        return "Not enough arguments\n";
    }

    /*
     * Use TaskList to handle the insertion
     */
    /* Build TaskList ADT */
    list = tasklist_init(file);
    /* Try reading the list */
    error = tasklist_read(list);
    if (error) {
        tasklist_destroy(list);
        return error;
    }
    /* Try inserting the task */
    error = tasklist_move(list, from_pos, to_pos);
    if (error) {
        tasklist_destroy(list);
        return error;
    }
    /* Try writing the updated list to file */
    error = tasklist_write(list);
    if (error) {
        tasklist_destroy(list);
        return error;
    }
    /* Show the modified list */
    if (verbose) {
        tasklist_print(list);
    }
    tasklist_destroy(list);

    return NULL;
}

const char *tasklib_remove(char **files) {
    /* Iterate over path array until terminator is encountered */
    for ( ; *files; ++files) {
        /* Attempt unlinking */
        if (unlink(*files) != 0) {
            return "Unable to delete list\n";
        }
    }

    return NULL;
}
