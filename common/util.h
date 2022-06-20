// Utility functions

#pragma once

#include "common.h"

// Checks if a file exists
extern bool util_fexist(const char *path);

// Get the length of a file
extern size_t util_fsize(FILE *stream);

// Normalize a path
extern char *util_normalize_path(const char *path);

// Prepend to a string
extern void util_prepend(char *str, const char *prefix);

// Append to a string
extern void util_append(char *str, const char *suffix);

// Duplicate a string
extern char *util_strdup(const char *str);

// Duplicate part of a string
extern char *util_strndup(const char *str, size_t n);

// Format a string
extern char *util_strfmt(size_t *size, const char *fmt, ...);

// Format a string
extern char *util_vstrfmt(size_t *size, const char *fmt, va_list args);
