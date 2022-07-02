// Implementations of util.h

// All the stb headers (that're used) are included in common.h, which util.h includes
#define STB_SPRINTF_IMPLEMENTATION

#include "util.h"

void *util_alloc(size_t count, size_t size, void *old)
{
	void *buf;

	buf = calloc(count, size);
	PURPL_ASSERT(buf);
	if (old) {
		memcpy(buf, old, count * size);
		free(old);
	}

	return buf;
}

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
	char *buf2;
	char *p;
	size_t len;

	if (!path)
		return NULL;

	buf = util_strdup(path);
	p = strchr(buf, '\\');
	while (p) {
		*p = '/';
		p = strchr(++p, '\\');
	}

	p = strstr(buf, "//");
	while (p) {
		len = strlen(buf) - (p - buf);
		if (len > strlen(buf))
			break;
		memmove(p, p + 1, len);
		p[len - 1] = 0;
	}

	buf2 = util_strdup(buf);
	free(buf);
	return buf2;
}

char *util_absolute_path(const char *path)
{
	char *path2;
	char *buf;
	uint32_t len;

#ifdef _WIN32
	len = GetFullPathNameA(path, 0, NULL, 0);
	buf = util_alloc(len, sizeof(char), NULL);
	GetFullPathNameA(path, len, buf, NULL);
#endif

	path2 = util_normalize_path(buf);
	free(buf);

	return path2;
}

bool util_isabsolute(const char *path)
{
	char *path2;
	char *path3;
	bool ret;

	path2 = util_normalize_path(path);
	path3 = util_absolute_path(path);
	ret = strcmp(path2, path2) == 0;

	free(path2);
	free(path3);

	return ret;
}

char *util_prepend(const char *str, const char *prefix)
{
	if (!str || !prefix || !strlen(prefix))
		return NULL;

	return util_strfmt("%s%s", prefix, str);
}

char *util_insert(const char *str, size_t pos, const char *insert)
{
	if (!str || !insert || !strlen(insert))
		return NULL;

	return util_strfmt("%.*s%s%s", pos + 1, str, insert, str + pos + 1);
}

char *util_append(const char *str, const char *suffix)
{
	if (!str || !suffix || !strlen(suffix))
		return NULL;

	return util_strfmt("%s%s", str, suffix);
}

char *util_replace(const char *str, const char *orig, const char *rplc)
{
	char *str1;
	char *str2;

	if (!str || !orig || !rplc)
		return NULL;

	str1 = util_strdup(str);
	str2 = strstr(str1, orig);
	while (str2) {
		str2 = util_strfmt("%.*s%s%s", str2 - str1, str1, rplc, str2 + strlen(orig));
		free(str1);
		str1 = str2;
		str2 = strstr(str1, orig);
	}

	return str1;
}

char *util_remove(const char *str, const char *del)
{
	return util_replace(str, del, "");
}

char *util_removeidx(const char *str, size_t start, size_t end)
{
	if (!str || start > strlen(str) || end > strlen(str))
		return NULL;

	return util_strfmt("%.*s%s", start, str, str + end);
}

char *util_strdup(const char *str)
{
	return util_strndup(str, strlen(str) + 1);
}

char *util_strndup(const char *str, size_t n)
{
	char *buf;

	buf = util_alloc(n, sizeof(char), NULL);
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
	buf = util_alloc(len, sizeof(char), NULL);
	stbsp_vsnprintf(buf, len, fmt, args2);

	return buf;
}

// Adapted from https://gist.github.com/JonathonReinhart/8c0d90191c38af2dcadb102c4e202950
void util_mkdir(const char *path)
{
	char *path2;
	char *p;

	path2 = util_normalize_path(path);
	for (p = path2 + 1; *p; p++) {
		if (*p == '/') {
			*p = 0;

			// No error checking because opening a file in the directory will fail and that's pretty much
			// guaranteed to happen eventually after this function is called, because why else would you
			// create a directory?
#ifdef _WIN32
			CreateDirectoryA(path2, NULL);
#else
			mkdir(path2, 777);
#endif
			*p = '/';
		}
	}

#ifdef _WIN32
	CreateDirectoryA(path2, NULL);
#else
	mkdir(path2, NULL);
#endif
}

uint64_t util_getaccuratetime(void)
{
	uint64_t time;
#ifdef _WIN32
	GetSystemTimeAsFileTime((FILETIME *)&time);
	time /= 10000; // FILETIME is 100 nanosecond intervals, convert to milliseconds
	time -= 11644473600000; // 1601 -> 1970
#endif
	return time;
}
