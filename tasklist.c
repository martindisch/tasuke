#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
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
    for (i = 0; i < list->array_size; i++) {
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
    for (i = 0; i < list->length; i++) {
        printf("  [%d] %s", i + 1, list->tasks[i]);
    }
    // Print empty line for visual separation between lists
    printf("\n");
}

char *tasklist_done(TaskList list, char **positions) {
    // Reset errno to use it for error checking in strtol
    errno = 0;
    // Iterate over all positional arguments
    for ( ; *positions; positions++) {
        char *endptr;
        long position = strtol(*positions, &endptr, 10);
        // Handle conversion error
        if (errno || *endptr != '\0') {
            return "Not a number\n";
        }
        // Handle position out of range
        if (position < 1 || position > list->length) {
            return "No task at this position\n";
        }
        // Turn 1-based position into 0-based index
        long index = position - 1;
        // Remove task from list
        free(list->tasks[index]);
        list->tasks[index] = NULL;
    }

    return NULL;
}

char *tasklist_read(TaskList list, const char *file) {
    // Open file in read mode
    FILE *fp;
    if ((fp = fopen(file, "r")) == NULL) {
        return "Unable to open task list\n";
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
        list->length++;
    }

    // Close file
    if (fclose(fp) == EOF) {
        return "Unable to close file\n";
    }

    return NULL;
}

char *tasklist_write(TaskList list, const char *file) {
    // Open file in write mode
    FILE *fp;
    if ((fp = fopen(file, "w")) == NULL) {
        return "Unable to open task list\n";
    }

    // Write all tasks to file
    int i;
    for (i = 0; i < list->length; i++) {
        // Check if task exists
        if (list->tasks[i]) {
            // Attempt write
            if (fputs(list->tasks[i], fp) == EOF) {
                fclose(fp);
                return "Unable to write to file\n";
            }
        }
    }

    // Close file
    if (fclose(fp) == EOF) {
        return "Unable to close file\n";
    }

    return NULL;
}
