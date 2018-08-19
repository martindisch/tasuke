#include <stdio.h>
#include "tasklib.h"

const char *add(const char *dir, const char *list, char **tasks) {
    printf("List is in %s\n", dir);
    printf("Name is %s\n", list);
    while (*tasks != NULL) {
        printf("Element %s\n", *tasks);
        tasks++;
    }
    return NULL;
}
