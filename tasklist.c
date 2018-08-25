#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "tasklist.h"

#define STARTING_CAPACITY 16

struct tasklist {
    char *name;
    char **tasks;
    int array_size;
    int length;
};

TaskList tasklist_init(const char *name) {
    // Allocate memory for ADT
    TaskList list;
    list = malloc(sizeof(*list));

    // Initialize members
    list->name = malloc((strlen(name) + 1) * sizeof(char));
    strcpy(list->name, name);
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
    // Free name
    free(list->name);
    // Free ADT
    free(list);
}

void tasklist_print(TaskList list) {
    // Print list name
    printf("%s:\n", list->name);
    // Print tasks
    int i;
    for (i = 0; i < list->length; ++i) {
        printf("  [%d] %s", i + 1, list->tasks[i]);
    }
    // Print empty line for visual separation between lists
    printf("\n");
}

const char *tasklist_insert(TaskList list, long position, const char *task) {
    /*
     * Preparatory work: sanity check, memory allocation
     */
    // Handle position out of range
    if (position < 1 || position > list->length + 1) {
        return "Invalid position\n";
    }
    // If the task array is full, allocate memory for an additional element
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

const char *tasklist_read(TaskList list, const char *file) {
    // Open file in read mode
    FILE *fp;
    if ((fp = fopen(file, "r")) == NULL) {
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
        // Allocate memory for the task
        char *task = malloc((strlen(line) + 1) * sizeof(char));
        // Copy the task into the new memory
        strcpy(task, line);
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

const char *tasklist_write(TaskList list, const char *file) {
    // Open file in write mode
    FILE *fp;
    if ((fp = fopen(file, "w")) == NULL) {
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
