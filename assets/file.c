#include <stdio.h>
#include <stdlib.h>
#include "file.h"

void *
file_load(const char *path, size_t *size)
{
	FILE *f;
	void *buffer;
	f = fopen(path, "rb");
	if (!f) {
		goto error;
	}
	*size = file_size(f);
	if (!*size) {
		goto error;
	}
	buffer = file_read(f, *size);
	if (!buffer) {
		goto error;
	}
	fclose(f);
	return buffer;
error:
	fclose(f);
	return NULL;
}

void *
file_read(FILE *f, size_t size)
{
	void *buffer;
	size_t readen;
	void *tmp;
	buffer = malloc(size);
	if (!buffer) {
		goto error;
	}
	readen = fread(buffer, 1, size, f);
	if (readen != size) {
		if (ferror(f)) {
			free(buffer);
			goto error;
		}
		tmp = realloc(buffer, readen);
		if (!tmp) {
			free(buffer);
			goto error;
		}
		buffer = tmp;
	}
	return buffer;
error:
	free(buffer);
	return NULL;
}

size_t
file_size(FILE *f)
{
	long size;
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	if (size < 0) {
		return 0;
	}
	rewind(f);
	return size;
}