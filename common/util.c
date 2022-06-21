// Implementations of util.h

// All the stb headers (that're used) are included in common.h, which util.h includes
#define STB_DS_IMPLEMENTATION
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
	char *str2;
	size_t len;

	if (!str || !prefix || !strlen(prefix))
		return str;

	str2 = str;
	len = strlen(prefix);
	stbds_arraddnindex(str2, len);
	memmove(str2 + len, str2, strlen(str2));
	strncpy(str2, prefix, len);
	str2[stbds_arrlenu(str2) - 1] = 0;

	return str2;
}

char *util_append(char *str, const char *suffix)
{
	char *str2;
	size_t len;

	if (!str || !suffix || !strlen(suffix))
		return str;

	str2 = str;
	len = strlen(suffix);
	stbds_arraddnindex(str2, len);
	strncpy(str2 + strlen(str2), suffix, len);
	str2[stbds_arrlenu(str2) - 1] = 0;

	return str2;
}

char *util_strdup(const char *str)
{
	return util_strndup(str, strlen(str) + 1);
}

char *util_strndup(const char *str, size_t n)
{
	char *buf = NULL;

	stbds_arrsetlen(buf, n);
	strncpy(buf, str, n);
	buf[n - 1] = 0;

	return buf;
}

char *util_strfmt(size_t *size, const char *fmt, ...)
{
	va_list args;
	char *buf;

	va_start(args, fmt);
	buf = util_vstrfmt(size, fmt, args);
	va_end(args);

	return buf;
}

char *util_vstrfmt(size_t *size, const char *fmt, va_list args)
{
	va_list args2;
	char *buf = NULL;
	int len;

	if (!fmt)
		return NULL;

	va_copy(args2, args);

	len = stbsp_vsnprintf(NULL, 0, fmt, args2) + 1;
	stbds_arrsetlen(buf, len);
	stbsp_vsnprintf(buf, len, fmt, args2);
	if (size)
		*size = len;
	return buf;
}
