#ifndef FILE_H
#define FILE_H

#include <stdio.h>

void *file_load(const char *path, size_t *size);
void *file_read(FILE *f, size_t size);
size_t file_size(FILE *f);

#endif