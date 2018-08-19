#ifndef TASKLIB_H
#define TASKLIB_H

/**
 * Appends tasks to a list stored in a directory.
 *
 * @param dir Path to the directory where the task list is stored
 * @param list Name of the list
 * @param tasks Array of tasks, terminated by a NULL element
 * @return Error message or NULL on success
 */
const char *add(const char *dir, const char *list, char **tasks);

#endif // TASKLIB_H
