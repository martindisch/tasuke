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
static char *get_name(const char *path) {
    // Pointers to name start (inclusive) and end (exclusive)
    const char *name_start = NULL, *name_end = NULL;
    // Iterate over the path
    for ( ; *path; ++path) {
        if (*path == '/') {
            // Update the name's start when a slash is encountered
            name_start = path + 1;
        } else if (*path == '.') {
            // Update the name's end when a dot is encountered
            name_end = path;
        }
    }
    // Get number of characters in the name
    int length = name_end - name_start;
    // Get a copy of the substring containing the list name
    char *name = strndup(name_start, length);

    return name;
}

TaskList tasklist_init(const char *path) {
    // Allocate memory for ADT
    TaskList list;
    list = malloc(sizeof(*list));

    // Initialize members
    list->path = strdup(path);
    list->name = get_name(list->path);
    list->tasks = malloc(STARTING_CAPACITY * sizeof(char *));
    list->array_size = STARTING_CAPACITY;
    list->length = 0;

    return list;
}

void tasklist_destroy(TaskList list) {
    // Free content of tasks array
    int i;
    for (i = 0; i < list->length; ++i) {
        // Only free if it wasn't already (i.e. task was removed)
        if (list->tasks[i]) {
            free(list->tasks[i]);
        }
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
    printf("\e[4m\e[1m%s\e[0m\n", list->name);
    // Print tasks
    int i;
    for (i = 0; i < list->length; ++i) {
        printf(" \e[1m%d\e[0m %s", i + 1, list->tasks[i]);
    }
    // Print empty line for visual separation between lists
    printf("\n");
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
        int i;
        for (i = index; i < list->length + 1; ++i) {
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
    }

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
    int i;
    if (from < to) {
        for (i = from; i < to; ++i) {
            char *current = list->tasks[i];
            list->tasks[i] = list->tasks[i + 1];
            list->tasks[i + 1] = current;
        }
    } else {
        for (i = from; i > to; --i) {
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
    int i;
    for (i = 0; i < list->length; ++i) {
        // Check if task exists
        if (list->tasks[i]) {
            // Attempt write
            if (fputs(list->tasks[i], fp) == EOF) {
                fclose(fp);
                return "Unable to write to list\n";
            }
        }
    }

    // Close file
    if (fclose(fp) == EOF) {
        return "Unable to close list\n";
    }

    return NULL;
}
