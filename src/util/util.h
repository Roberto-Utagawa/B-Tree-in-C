#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NAME_SZ 15
#define SURNAME_SZ 15
#define COURSE_SZ 30

void strings_formatation(char *input, int *nUsp, char *name, char *surname, char *course, float *grade);
bool menu(char *input);
int take_nUsp(char *input);

#endif //UTIL_H