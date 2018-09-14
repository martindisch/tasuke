/* Using strdup & strndup, need POSIX 2008 */
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "tasklist.h"

#define STARTING_CAPACITY 16

struct tasklist {
    char *path;
    char *name;
    char **tasks;
    int array_size;
    int length;
};

/**
 * Returns the name of a task list based on its full path.
 *
 * It isolates the final part of the path, minus the file extension (.txt).
 * Because a new string needs to be allocated, the user must free it.
 *
 * @param path The full path to the file
 * @return The list name (freed by user)
 */
static char *path_to_name(const char *path) {
    // Find the first character of the name (inclusive)
    const char *name_start = strrchr(path, '/') + 1;
    // Continue and find the end of the name (exclusive)
    const char *name_end = strrchr(name_start, '.');
    // Get number of characters in the name
    int length = name_end - name_start;
    // Get a copy of the substring containing the list name
    char *name = strndup(name_start, length);

    return name;
}

/**
 * Copies at most n chars from src into dest, splitting at spaces.
 *
 * It returns a pointer to the next character after the space where the
 * split was made.
 * It's imperative that the string is at least n characters long, because this
 * function does not check for the end.
 * The dest buffer needs to be n + 1 bytes large for the \0 terminator.
 *
 * @param dest The buffer characters are copied into
 * @param src The string characters are copied from
 * @param n Maximum number of characters to copy
 * @return Pointer to the character that should be printed next
 */
static const char *fold(char *dest, const char *src, int n) {
    // Initialize end of copy (exclusive) to maximum number of characters,
    // because that's where we'll split if there are no spaces
    const char *end = src + n;
    // This is the next character that should be printed
    const char *next = src + n;
    // Find the last space within n characters
    for (int i = 0; i <= n; ++i) {
        if (src[i] == ' ') {
            // Set end (exclusive) to the space
            end = src + i;
            // The character after that is the next one that should be printed
            next = src + i + 1;
        }
    }
    // Copy characters before the last space into the destination buffer
    strncpy(dest, src, end - src);
    // Terminate the string
    dest[end - src] = '\0';

    return next;
}

TaskList tasklist_init(const char *path) {
    // Allocate memory for ADT
    TaskList list;
    list = malloc(sizeof(*list));

    // Initialize members
    list->path = strdup(path);
    list->name = path_to_name(list->path);
    list->tasks = malloc(STARTING_CAPACITY * sizeof(char *));
    list->array_size = STARTING_CAPACITY;
    list->length = 0;

    return list;
}

void tasklist_destroy(TaskList list) {
    // Free content of tasks array
    for (int i = 0; i < list->length; ++i) {
        free(list->tasks[i]);
    }
    // Free tasks array
    free(list->tasks);
    // Free other properties
    free(list->path);
    free(list->name);
    // Free ADT
    free(list);
}

void tasklist_print(TaskList list) {
    // Print list name
    printf("\x1b[4m\x1b[1m%s\x1b[0m\n", list->name);
    // If there are no tasks, show notice and return early
    if (list->length == 0) {
        printf(" No tasks\n");
        return;
    }
    // Determine format (for padding) depending on number of tasks
    const char *format, *pad;
    int space;
    if (list->length < 10) {
        format = " \x1b[1m%d\x1b[0m %s";
        pad = "   ";
        space = 80 - 3;
    } else if (list->length < 100) {
        format = " \x1b[1m%2d\x1b[0m %s";
        pad = "    ";
        space = 80 - 4;
    } else {
        format = " \x1b[1m%3d\x1b[0m %s";
        pad = "     ";
        space = 80 - 5;
    }
    // Print tasks
    for (int i = 0; i < list->length; ++i) {
        if (strlen(list->tasks[i]) <= space + 1) {
            // There is enough space to print the whole task on one line
            printf(format, i + 1, list->tasks[i]);
        } else {
            // Need to split the task over several lines
            const char *task = list->tasks[i];
            char out[space + 1];
            // Print the first line
            task = fold(out, task, space);
            printf(format, i + 1, out);
            printf("\n");
            // Print remaining lines
            while (strlen(task) > space + 1) {
                task = fold(out, task, space);
                printf("%s%s\n", pad, out);

            }
            // Print final line
            printf("%s%s", pad, task);
        }
    }
}

const char *tasklist_insert(
    TaskList list, long position, const char *task) {
    /*
     * Preparatory work: sanity check, memory allocation
     */
    // Handle position out of range
    if (position < 1 || position > list->length + 1) {
        return "Invalid position\n";
    }
    // If task array is full, allocate memory for an additional element
    if (list->array_size == list->length) {
        list->tasks = realloc(
            list->tasks, (list->array_size + 1) * sizeof(char *));
        ++(list->array_size);
    }
    // Allocate memory for the task
    char *new_task = malloc((strlen(task) + 2) * sizeof(char));
    // Copy the task into the new memory, appending newline
    sprintf(new_task, "%s\n", task);
    // Turn 1-based position into 0-based index
    long index = position - 1;

    /*
     * Insertion
     */
    if (index == list->length) {
        // Simple case: append task to end
        list->tasks[list->length] = new_task;
    } else {
        // Normal case: insert somewhere and bubble other elements down
        char *current, *previous = new_task;
        for (int i = index; i < list->length + 1; ++i) {
            current = list->tasks[i];
            list->tasks[i] = previous;
            previous = current;
        }
    }
    // Increment length
    ++(list->length);

    return NULL;
}

const char *tasklist_done(TaskList list, const long *positions) {
    /*
     * Delete selected tasks
     */
    int done_count = 0;
    // Iterate over given positions
    for ( ; *positions != -1; ++positions) {
        // Handle position out of range
        if (*positions < 1 || *positions > list->length) {
            return "Invalid position\n";
        }
        // Turn 1-based position into 0-based index
        long index = *positions - 1;
        // Remove task from list
        free(list->tasks[index]);
        list->tasks[index] = NULL;
        // Remember we deleted a task
        ++done_count;
    }

    /*
     * Build new task list with the remaining ones
     */
    int new_length = list->length - done_count;
    // If no tasks remain, just update the count and terminate
    if (new_length == 0) {
        list->length = 0;
        return NULL;
    }
    // Allocate memory for the new task list
    char **tasks = malloc(new_length * sizeof(char *));
    // Iterate over the old list, copying over the surviving elements
    for (int i = 0, y = 0; i < list->length; ++i) {
        if (list->tasks[i]) {
            tasks[y++] = list->tasks[i];
        }
    }
    // Free the old list of task references
    free(list->tasks);
    // Use the new list of references instead
    list->tasks = tasks;
    // Update the count
    list->length = new_length;
    list->array_size = new_length;

    return NULL;
}

const char *tasklist_move(TaskList list, long from_pos, long to_pos) {
    /*
     * Preparatory work with sanity checking
     */
    // Handle positions out of range
    if (from_pos < 1 || from_pos > list->length ||
        to_pos < 1 || to_pos > list->length) {
        return "Invalid position\n";
    }
    // Abort when there's nothing to do
    if (from_pos == to_pos) {
        return NULL;
    }
    // Turn 1-based positions into 0-based indices
    long from = from_pos - 1, to = to_pos - 1;

    /*
     * Movement
     */
    if (from < to) {
        for (int i = from; i < to; ++i) {
            char *current = list->tasks[i];
            list->tasks[i] = list->tasks[i + 1];
            list->tasks[i + 1] = current;
        }
    } else {
        for (int i = from; i > to; --i) {
            char *current = list->tasks[i];
            list->tasks[i] = list->tasks[i - 1];
            list->tasks[i - 1] = current;
        }
    }

    return NULL;
}

const char *tasklist_read(TaskList list) {
    // Open file in read mode
    FILE *fp;
    if ((fp = fopen(list->path, "r")) == NULL) {
        return "Unable to open list\n";
    }

    // Read tasks
    char line[LINE_MAX];
    int i = 0;
    while (fgets(line, LINE_MAX, fp) != NULL) {
        if (list->array_size - i == 0) {
            // Double array size
            list->tasks = realloc(
                list->tasks, 2 * list->array_size * sizeof(char *));
            list->array_size *= 2;
        }
        // Make a copy of the task
        char *task = strdup(line);
        // Add task to list
        list->tasks[i++] = task;
        ++(list->length);
    }

    // Close file
    if (fclose(fp) == EOF) {
        return "Unable to close list\n";
    }

    return NULL;
}

const char *tasklist_write(TaskList list) {
    // Open file in write mode
    FILE *fp;
    if ((fp = fopen(list->path, "w")) == NULL) {
        return "Unable to open list\n";
    }

    // Write all tasks to file
    for (int i = 0; i < list->length; ++i) {
        // Attempt write
        if (fputs(list->tasks[i], fp) == EOF) {
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
