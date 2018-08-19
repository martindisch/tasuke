#ifndef TASKLIB_H
#define TASKLIB_H

/**
 * Appends tasks to a file.
 *
 * @param file Full path to the file
 * @param tasks Array of tasks, terminated by a NULL element
 * @return Error message or NULL on success
 */
const char *add(const char *file, char **tasks);

/**
 * Builds a full path to the task list file based on directory and list name.
 *
 * If the directory is NULL, the default will be used, which is .tasuke
 * inside the user home.
 * The same goes for the task list. The default is a list called todo.
 * Because this returns a string of previously unknown length, it needs to
 * dynamically allocate memory for it. The user is responsible for freeing.
 *
 * @param dir Path to directory where task list is stored (NULL for default)
 * @param list Name of the list (NULL for default)
 * @return Full path to task list file (freed by user)
 */
char *get_file(const char *dir, const char *list);

#endif // TASKLIB_H
