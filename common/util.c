// Implementations of util.h

// All the stb headers (that're used) are included in common.h, which util.h includes
#define STB_SPRINTF_IMPLEMENTATION

#include "util.h"

void util_log(const char *func, int line, const char *file, const char *fmt, ...)
{
	va_list args;
	char *buf;
	char *buf2;
	char *file2;
	char *filename;

	if (!func || !file || !fmt)
		return;

	file2 = util_normalize_path(file);
#ifdef PURPL_DEBUG
	filename = file2;
#else
	filename = strstr(file2, "purpl-engine/") + 13;
#endif

	va_start(args, fmt);
	buf = util_vstrfmt(fmt, args);
	va_end(args);

	buf2 = util_strfmt("[%s:%d:%s]\n%s", func, line, filename, buf);

	free(buf);
	free(file2);

	printf("%s", buf2);
#ifdef _WIN32
	OutputDebugStringA(buf2);
#endif

	free(buf2);
}

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

void util_free_list(void **list, size_t count)
{
	size_t i;

	if (!list)
		return;

	for (i = 0; i < count; i++) {
		if (list[i])
			free(list[i]);
	}
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
	size_t len;

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

#ifdef _WIN32
	uint32_t len;

	len = GetFullPathNameA(path, 0, NULL, 0);
	buf = util_alloc(len, sizeof(char), NULL);
	GetFullPathNameA(path, len, buf, NULL);
#else
	buf = util_alloc(PATH_MAX, sizeof(char), NULL);
	realpath(path, buf);
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
	ret = strcmp(path2, path3) == 0;

	free(path2);
	free(path3);

	return ret;
}

char *util_prepend(const char *str, const char *prefix)
{
	if (!str)
		return util_strdup(prefix);
	if (!prefix)
		return util_strdup(str);

	return util_strfmt("%s%s", prefix, str);
}

char *util_insert(const char *str, size_t pos, const char *insert)
{
	if (!str)
		return util_strdup(insert);
	if (!insert)
		return util_strdup(str);

	return util_strfmt("%.*s%s%s", pos + 1, str, insert, str + pos + 1);
}

char *util_append(const char *str, const char *suffix)
{
	if (!str)
		return util_strdup(suffix);
	if (!suffix)
		return util_strdup(str);

	return util_strfmt("%s%s", str, suffix);
}

char *util_replace(const char *str, const char *orig, const char *rplc)
{
	char *str2;
	char *str3;
	size_t i;

	if (!str || !orig || !rplc)
		return NULL;

	str2 = util_strdup(str);
	for (i = 0; i < strlen(str2); i++) {
		if (strncmp(str2 + i, orig, strlen(orig)) == 0) {
			str3 = util_strfmt("%.*s%s%s", i, str2, rplc, str2 + strlen(orig));
			free(str2);
			str2 = str3;
			i += strlen(rplc);
		}
	}

	return str2;
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
	if (strlen(str))
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

	len = stbsp_vsnprintf(NULL, 0, fmt, args) + 1;
	buf = util_alloc(len, sizeof(char), NULL);
	stbsp_vsnprintf(buf, len, fmt, args2);

	va_end(args2);

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

			// No error checking because opening a file in the directory will fail and that's pretty
			// much guaranteed to happen eventually after this function is called, because why else
			// would you create a directory?
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
	mkdir(path2, 0);
#endif
}

uint64_t util_getaccuratetime(void)
{
	uint64_t time;
#ifdef _WIN32
	GetSystemTimeAsFileTime((FILETIME *)&time);
	time /= 10000; // FILETIME is 100 nanosecond intervals, convert to milliseconds
	time -= 11644473600000; // 1601 -> 1970
#else

#endif
	return time;
}

uint32_t util_parse_version(const char *version)
{
	uint8_t i;
	char *version2;
	char *token;
	uint8_t parts[4];

	if (!version)
		return 0;

	if (version[0] == 'v')
		version2 = util_strdup(version + 1);
	else
		version2 = util_strdup(version);

	if (!strlen(version2)) {
		free(version2);
		return 0;
	}

	// strtok until 4 numbers or no more tokens
	i = 0;
	token = strtok(version2, ".");
	memset(parts, 0, 4);
	while (token && i < 4)
		parts[i] = strtol(token, NULL, 10) & 0xFF;

	free(version2);
	return PURPL_MAKE_VERSION(parts[0], parts[1], parts[2], parts[3]);
}

bool util_list_check(list_check_t *params)
{
	size_t i;
	bool have;

	if (!params || !params->source || !params->other || !params->callback)
		return false;

	have = true;
	for (i = 0; i < params->other_count && have; i += params->other_elem_size)
		have = bsearch((const uint8_t *)params->other + i, params->source, params->source_count,
			       params->source_elem_size, params->callback);

	return have;
}
