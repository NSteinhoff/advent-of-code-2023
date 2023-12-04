#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *readToString(const char *restrict fname);
char *readToString(const char *restrict fname) {
	FILE *file = fopen(fname, "r");
	if (!file) {
		perror(fname);
		return NULL;
	}

	if (fseek(file, 0L, SEEK_END)) {
		perror("Cannot seek to end of file");
		return NULL;
	}

	long bytes = ftell(file);
	if (bytes == -1) {
		perror("Cannot get position in file");
		return NULL;
	}
	size_t bytes_to_read = (size_t)bytes;

	if (fseek(file, 0L, SEEK_SET)) {
		perror("Cannot rewind file");
		return NULL;
	}

	char *content = malloc(bytes_to_read + 1);
	if (!content) {
		perror(NULL);
		return NULL;
	}

	size_t bytes_read = fread(content, 1, bytes_to_read, file);
	if (bytes_read != bytes_to_read) {
		if (feof(file))
			fprintf(stderr,
				"Expected to read %zu bytes, but read %zu\n",
				bytes_to_read, bytes_read);

		if (ferror(file))
			fprintf(stderr, "Could not read all bytes from %s\n",
				fname);

		free(content);
		return NULL;
	}
	content[bytes_read] = 0;

	if (fclose(file)) {
		perror("Cannot close file");
		free(content);
		return NULL;
	}

	return content;
}
