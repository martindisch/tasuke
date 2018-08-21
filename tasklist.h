#ifndef TASKLIST_H
#define TASKLIST_H

typedef struct tasklist *TaskList;

/**
 * Returns an initialized TaskList.
 *
 * @param name The list's name
 * @return The new TaskList
 */
TaskList tasklist_init(const char *name);

/**
 * Releases the TaskList.
 *
 * @param list The TaskList to free
 */
void tasklist_destroy(TaskList list);

/**
 * Prints the TaskList to stdout.
 *
 * @param list The TaskList
 */
void tasklist_print(TaskList list);

/**
 * Removes the tasks at the given positions from the list.
 *
 * @param list The TaskList
 * @param positions Array of task indices (1-based, type string), terminated
 *                  by a NULL element
 * @return Error message or NULL on success
 */
char *tasklist_done(TaskList list, char **positions);

/**
 * Build the TaskList based on the file at the given path.
 *
 * @param list The TaskList
 * @param file Full path to file
 * @return Error message or NULL on success
 */
char *tasklist_read(TaskList list, const char *file);

/**
 * Writes the TaskList to the file at the given path.
 *
 * @param list The TaskList
 * @param file Full path to file
 * @return Error message or NULL on success
 */
char *tasklist_write(TaskList list, const char *file);

#endif // TASKLIST_H
