#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

FILE* stdout2 = NULL;

void init() {
    stdout2 = stdout;
    if (!stdout2) return -1;
    return 1;
}