#ifndef TASKLIST_H
#define TASKLIST_H

typedef struct tasklist *TaskList;

/**
 * Returns an initialized TaskList.
 *
 * @param path Full path to file
 * @return The new TaskList
 */
TaskList tasklist_init(const char *path);

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
 * Inserts a task into a list at a specific position.
 *
 * Any existing items at that position and after are pushed down.
 *
 * @param list The TaskList
 * @param position The position to insert to (1-based)
 * @param task The task text
 * @return Error message or NULL on success
 */
const char *tasklist_insert(TaskList list, long position, const char *task);

/**
 * Removes the tasks at the given positions from the list.
 *
 * @param list The TaskList
 * @param positions Array of task positions (1-based), terminated by -1
 * @return Error message or NULL on success
 */
const char *tasklist_done(TaskList list, const long *positions);

/**
 * Moves a task inside a list by bubbling it up or down.
 *
 * @param list The TaskList
 * @param from The source position (1-based)
 * @param to The destination position (1-based)
 * @return Error message or NULL on success
 */
const char *tasklist_move(TaskList list, long from, long to);

/**
 * Builds the TaskList by reading it from file.
 *
 * @param list The TaskList
 * @return Error message or NULL on success
 */
const char *tasklist_read(TaskList list);

/**
 * Writes the TaskList to its file.
 *
 * @param list The TaskList
 * @return Error message or NULL on success
 */
const char *tasklist_write(TaskList list);

#endif // TASKLIST_H
