// Definitions of the functions in purpl/util/string.h
//
// Copyright 2021 MobSlicer152
// This file is part of Purpl Engine
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "stb_sprintf.h"

#include "purpl/util/string.h"

PURPL_API size_t purpl_strcount(const char *str, const char *sub)
{
	const char *p = str;
	size_t count = 0;

	if (!str)
		return 0;

	while ((p = strstr(p, sub))) {
		count++;
		p++;
	}

	return count;
}

PURPL_API char *purpl_strrplc(const char *str, const char *old, const char *new,
			      size_t *size)
{
	char *buf;
	char *tmp;
	char *p;
	size_t buf_size;
	size_t occurences;
	size_t idx;

	if (!str)
		return NULL;

	occurences = purpl_strcount(str, old);
	buf_size = strlen(str) + 2;
	buf_size = buf_size - (occurences * strlen(old)) +
		   (occurences * strlen(new));

	buf = calloc(buf_size, sizeof(char));
	if (!buf)
		return NULL;

	tmp = calloc(buf_size, sizeof(char));
	if (!tmp) {
		strncpy(buf, str, buf_size);
		return buf;
	}

	strncpy(buf, str, buf_size);
	buf[buf_size - 1] = 0;
	p = buf;

	while ((p = strstr(p, old))) {
		idx = p - buf;
		strncpy(tmp, buf, idx);
		tmp[idx] = 0;
		strncat(tmp, new, buf_size);
		strncat(tmp, p + strlen(old), buf_size);
		strncpy(buf, tmp, buf_size);
	}

	free(tmp);

	if (size)
		*size = buf_size;

	return buf;
}

PURPL_API char *purpl_strrplcn(const char *str, const char *old,
			       const char *new, size_t *size)
{
	char *buf;
	char *p;
	size_t buf_size;

	if (!str)
		return NULL;

	buf_size = strlen(str) - strlen(old) + strlen(new);

	buf = calloc(buf_size, sizeof(char));
	if (!buf)
		return NULL;

	p = strstr(str, old);
	strncpy(buf, str, p - str);
	buf[p - str] = 0;
	strncat(buf, new, buf_size);
	strncat(buf, p + strlen(old), buf_size);

	if (size)
		*size = buf_size;

	return buf;
}

PURPL_API char *purpl_strfmt(size_t *size, const char *fmt, ...)
{
	char *buf;
	va_list args;

	va_start(args, fmt);

	buf = purpl_vstrfmt(size, fmt, args);

	va_end(args);

	return buf;
}

PURPL_API char *purpl_vstrfmt(size_t *size, const char *fmt, va_list args)
{
	char *buf;
	size_t buf_size;
	va_list _args;

	if (!args)
		return NULL;

	va_copy(_args, args);

	// Returns the number of characters that would be written
	buf_size = stbsp_vsnprintf(NULL, 0, fmt, _args);

	buf = calloc(buf_size, sizeof(char));
	if (!buf) {
		if (size)
			*size = 0;
		return NULL;
	}

	va_copy(_args, args);

	stbsp_vsprintf(buf, fmt, _args);

	va_end(_args);

	if (size)
		*size = buf_size;
	return buf;
}
