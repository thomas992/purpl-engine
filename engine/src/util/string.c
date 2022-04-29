// Definitions of the functions in purpl/util/string.h
//
// Copyright 2022 MobSlicer152
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

#include "stb_ds.h"
#include "stb_sprintf.h"

#include "purpl/core/inst.h"

#include "purpl/util/string.h"

#include "purpl/core/warnings.h"

PURPL_API size_t purpl_strcount(const char *str, const char *sub)
{
	const char *p;
	size_t count = 0;

	if (!str)
		return 0;

	p = strstr(str, sub);
	while (p) {
		count++;
		p = strstr(++p, sub);
	}

	return count;
}

PURPL_API char *purpl_strrplc(const char *str, const char *old,
			      const char *new, size_t *size)
{
	char *buf;
	char *tmp;
	char *p;
	size_t buf_size;
	size_t occurences;

	if (!str || !old || !new)
		return NULL;

	occurences = purpl_strcount(str, old);
	buf_size = strlen(str) + 2;
	buf_size = buf_size - (occurences * strlen(old) + 1) +
		   (occurences * strlen(new) + 1);

	buf = calloc(buf_size, sizeof(char));
	if (!buf)
		return NULL;

	tmp = calloc(buf_size, sizeof(char));
	if (!tmp) {
		strncpy(buf, str, buf_size);
		return buf;
	}

	strncpy(buf, str, buf_size);
	p = strstr(buf, old);

	while (p) {
		stbsp_snprintf(tmp, buf_size, "%.*s%s%s", (s32)(p - buf), buf,
			       new, p + strlen(old));
		strncpy(buf, tmp, buf_size);
		p = strstr(++p, old);
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
	buf_size = stbsp_vsnprintf(NULL, 0, fmt, _args) + 1;

	buf = calloc(buf_size, sizeof(char));
	if (!buf) {
		if (size)
			*size = 0;
		return NULL;
	}

	va_copy(_args, args);

	stbsp_vsnprintf(buf, buf_size, fmt, _args);

	va_end(_args);

	if (size)
		*size = buf_size;
	return buf;
}

PURPL_API char *purpl_strdup(const char *str)
{
	return purpl_strndup(str, strlen(str));
}

PURPL_API char *purpl_strndup(const char *str, size_t n)
{
	char *buf;

	if (!str || !n)
		return NULL;

	buf = calloc(n + 1, sizeof(char));
	if (!buf)
		return NULL;

	strncpy(buf, str, n);
	return buf;
}

PURPL_API char *purpl_strins(const char *str, const char *ins, size_t index,
			     size_t size)
{
	char *buf;

	if (!str || !ins || index >= size)
		return NULL;

	buf = calloc(size + strlen(ins), sizeof(char));
	if (!buf)
		return NULL;
	
	strncpy(buf, str, index);
	strncpy(buf + index, ins, strlen(str));
	strncpy(buf + index + strlen(str), str + index, strlen(str + index));

	return buf;
}

PURPL_API char *purpl_strrm(const char *str, size_t index, size_t n,
			    size_t size)
{
	char *buf;

	if (!str || index - n >= size)
		return NULL;

	buf = calloc(size - n, sizeof(char));
	if (!buf)
		return NULL;

	strncpy(buf, str, index);
	strncat(buf, str + index + n, size - n);

	return buf;
}

PURPL_API s8 purpl_wcscasecmp(const wchar_t *s1, const wchar_t *s2)
{
	wchar_t c1 = towlower(*s1);
	wchar_t c2 = towlower(*s2);

	while (*s1 != L'\0' && *s2 != L'\0') {
		// Compare
		if (c1 != c2)
			break;

		// Advance the pointers
		c1 = towlower(*s1++);
		c2 = towlower(*s2++);
	}

	return c1 - c2;
}

PURPL_API const char *purpl_format_version(u32 version)
{
	static char buf[PURPL_STATIC_BUF_MAX];

	stbsp_snprintf(buf, PURPL_STATIC_BUF_MAX, "%d.%d.%d",
		       (version >> 16) & 0xFF, (version >> 8) & 0xFF,
		       version & 0xFF);

	return buf;
}

const char *purpl_get_graphics_api_name(u32 api)
{
	const char *graphics_apis[] = { "Software", "OpenGL", "Vulkan",
					"DirectX",  "Metal",  "Unknown" };

	// Make sure the API is within the array
	return graphics_apis[PURPL_MIN(api, PURPL_GRAPHICS_API_MAX)];
}
