// Implementations of util.h

// All the stb headers (that're used) are included in common.h, which util.h includes
#define STB_SPRINTF_IMPLEMENTATION

#include "util.h"

bool util_fexist(const char *path)
{
	FILE *fp;

	fp = fopen(path, "r");
	if (!fp)
		return errno != ENOENT;

	fclose(fp);
	return true;
}

size_t util_fsize(FILE *stream)
{
	long offs;
	size_t len = 0;

	if (!stream)
		return 0;

	offs = ftell(stream);
	fseek(stream, 0, SEEK_END);
	len = ftell(stream);
	fseek(stream, offs, SEEK_SET);

	return len;
}

char *util_normalize_path(const char *path)
{
	char *buf;
	char *p;

	if (!path)
		return NULL;

	buf = util_strdup(path);
	p = strchr(buf, '\\');
	while (p) {
		*p = '/';
		p = strchr(++p, '\\');
	}

	return buf;
}

char *util_prepend(char *str, const char *prefix)
{
	if (!str || !prefix || !strlen(prefix))
		return str;

	return util_strfmt("%s%s", prefix, str);
}

char *util_append(char *str, const char *suffix)
{
	if (!str || !suffix || !strlen(suffix))
		return str;

	return util_strfmt("%s%s", str, suffix);
}

char *util_strdup(const char *str)
{
	return util_strndup(str, strlen(str) + 1);
}

char *util_strndup(const char *str, size_t n)
{
	char *buf;

	buf = calloc(n, sizeof(char));
	PURPL_ASSERT(buf);
	strncpy(buf, str, n);

	return buf;
}

char *util_strfmt(const char *fmt, ...)
{
	va_list args;
	char *buf;

	va_start(args, fmt);
	buf = util_vstrfmt(fmt, args);
	va_end(args);

	return buf;
}

char *util_vstrfmt(const char *fmt, va_list args)
{
	va_list args2;
	char *buf;
	int len;

	if (!fmt)
		return NULL;

	va_copy(args2, args);

	len = stbsp_vsnprintf(NULL, 0, fmt, args2) + 1;
	buf = calloc(len, sizeof(char));
	PURPL_ASSERT(buf);
	stbsp_vsnprintf(buf, len, fmt, args2);

	return buf;
}
