// Utility functions

#pragma once

#include "common.h"

// Allocate or grow memory (growing invalidates old pointers)
extern void *util_alloc(size_t count, size_t size, void *old);

// Free a list
extern void util_free_list(void **list, size_t count);

// Checks if a file exists
extern bool util_fexist(const char *path);

// Get the length of a file
extern size_t util_fsize(FILE *stream);

// Normalize a path
extern char *util_normalize_path(const char *path);

// Get an absolute path
extern char *util_absolute_path(const char *path);

// Determine if a path is absolute
extern bool util_isabsolute(const char *path);

// Prepend to a string
extern char *util_prepend(const char *str, const char *prefix);

// Insert into a string
extern char *util_insert(const char *str, size_t pos, const char *insert);

// Append to a string
extern char *util_append(const char *str, const char *suffix);

// Replace a string in a string
extern char *util_replace(const char *str, const char *orig, const char *rplc);

// Remove a string from a string
extern char *util_remove(const char *str, const char *del);

// Remove part of a string
extern char *util_removeidx(const char *str, size_t start, size_t end);

// Duplicate a string
extern char *util_strdup(const char *str);

// Duplicate part of a string
extern char *util_strndup(const char *str, size_t n);

// Format a string
extern char *util_strfmt(const char *fmt, ...);

// Format a string
extern char *util_vstrfmt(const char *fmt, va_list args);

// Make directories
extern void util_mkdir(const char *path);

// Get the number of milliseconds since 1970
extern uint64_t util_getaccuratetime(void);

// Parse a version number
extern uint32_t util_parse_version(const char *version);
