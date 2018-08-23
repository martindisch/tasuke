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
 * Inserts a task into a list at a specific position.
 *
 * Any existing items at that position and after are pushed down.
 *
 * @param file Full path to file
 * @param position_task Array containing the position (1-based, string), task
 *                      text and a terminating NULL element
 * @return Error message or NULL on success
 */
const char *insert(const char *file, char **position_task);

/**
 * Deletes tasks from a list.
 *
 * @param file Full path to file
 * @param positions Array of task indices (1-based, type string), terminated
 *                  by a NULL element
 * @return Error message or NULL on success
 */
const char *done(const char *file, char **positions);

/**
 * Prints task lists to stdout.
 *
 * @param files Array of file paths, terminated by a NULL element
 * @return Error message or NULL on success
 */
const char *list(char **files);

/**
 * Builds a full path to the task list file based on directory and list name.
 *
 * If the directory is NULL, the default will be used, which is .tasuke
 * inside the user home.
 * The same goes for the task list. The default is a list called todo.
 * If the directory does not yet exist, it will be created. This works only for
 * one directory, it won't recreate a full path that doesn't exist.
 * Because this returns a string of previously unknown length, it needs to
 * dynamically allocate memory for it. The user is responsible for freeing.
 * If there is an error accessing or creating the directory, NULL is returned.
 *
 * @param dir Path to directory where task list is stored (NULL for default)
 * @param list Name of the list (NULL for default)
 * @return Full path to task list file (freed by user) or NULL on error
 */
char *get_file(const char *dir, const char *list);

/**
 * Builds an array of full paths to the task list files based on dir and names.
 *
 * If the directory is NULL, the default will be used, which is .tasuke
 * inside the user home.
 * The same goes for the task lists. If the array is empty (containing only
 * a NULL element), an array containing only the path to the default list
 * (and of course the NULL terminator) is returned.
 * If the directory does not yet exist, it will be created. This works only for
 * one directory, it won't recreate a full path that doesn't exist.
 * Because this returns an array of previously unknown size, it needs to
 * dynamically allocate memory for it. The user is responsible for freeing.
 * If there is an error accessing or creating the directory, NULL is returned.
 *
 * @param dir Path to directory where task lists are stored (NULL for default)
 * @param lists Array of list names, terminated by a NULL element (containing
 *              only a NULL element for default list)
 * @return Array of full paths to task list files (freed by user, terminated
 *         by a NULL element) or NULL on error
 */
char **get_files(const char *dir, char **lists);

#endif // TASKLIB_H
