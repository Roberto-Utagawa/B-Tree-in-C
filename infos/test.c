#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define COURSE_SZ 30


void main()
{
    long a = sizeof(long) + 1;
    FILE* b;
    b = fopen("a.txt","rb+");
    fwrite(&a, sizeof(long), 1, b);
    fwrite("\n", sizeof(char), 1, b);
    fwrite("oioi", sizeof(char), 4, b);
}
