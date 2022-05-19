// Logging functions
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

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else // _WIN32
#include <syslog.h>
#endif // _WIN32

#include "purpl/core/log.h"

PURPL_API struct purpl_logger *purpl_log_create(const char *file, enum purpl_log_level level,
						enum purpl_log_level max_level, const char *format)
{
	struct purpl_logger *logger;
	time_t t;
	struct tm *t2;
	char *date;
	char *filename;
	char *filename2;

	if (!purpl_inst)
		return NULL;

	logger = calloc(1, sizeof(struct purpl_logger));
	if (!logger)
		return NULL;

	t = time(NULL);
	t2 = localtime(&t);
	if (t2)
		date = purpl_strfmt(NULL, "%d-%d-%d", t2->tm_mday, t2->tm_mon + 1, t2->tm_year + 1900);
	else
		date = purpl_strfmt(NULL, "?\?-?\?-????"); // Escapes are to
							   // prevent trigraph
							   // expansion

	filename2 = purpl_strrplc(file ? file : PURPL_LOG_DEFAULT_NAME, "<date>", date, NULL);
	free(date);
	filename = purpl_pathfmt(NULL, filename2, 0, false, true);
	free(filename2);

	logger->file = fopen(filename, "ab+");
	if (!logger->file) {
		free(filename);
		return NULL;
	}
	free(filename);

	logger->max_level = max_level >= PURPL_LOG_LEVEL_MAX ? PURPL_LOG_LEVEL_MAX - 1 : max_level;
	logger->level = level > logger->max_level ? logger->max_level : level;

	purpl_log_set_format(logger, format);

	return logger;
}

static void log_get_time(struct tm **t1, struct tm **t2)
{
	time_t t;

	t = time(NULL);
	if (t1)
		*t1 = localtime(&t);
	if (t2) {
		*t2 = localtime(&t);
		if ((*t2)->tm_mon == 3 && (*t2)->tm_mday == 1) {
			(*t2)->tm_mon = 2;
			(*t2)->tm_mday = 32;
		}
	}
}

static char *log_format(struct purpl_logger *logger, enum purpl_log_level level, const char *file, int line,
			const char *function, const char *msg, va_list a)
{
	char *buf;
	size_t buf_size;
	char *tmp;
	char *p;
	char *begin;
	char *str;
	size_t len;
	size_t amt;
	size_t i;

	char *base;
	size_t passes;

	char *msg_fmt;
	size_t msg_size;
	const char *file2;
	struct tm *t1;
	struct tm *t2;

	va_list args;

	const char *days[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

	const char *months[] = { "January", "February", "March",     "April",	"May",	    "June",
				 "July",    "August",	"September", "October", "November", "December" };

	const char *levels_lower[] = { "critical", "error", "warning", "info", "debug" };
	const char *levels_upper[] = { "CRITICAL", "ERROR", "WARNING", "INFO", "DEBUG" };

	va_copy(args, a);
	msg_fmt = purpl_vstrfmt(&msg_size, msg, args);
	va_end(args);

	buf_size = strlen(logger->format) + msg_size;
	buf = calloc(buf_size, sizeof(char));
	if (!buf)
		return NULL;

	log_get_time(&t1, &t2);

	file2 = file;
#ifndef PURPL_LOG_FULL_PATHS
	if (strncmp(file, PURPL_SOURCE_DIR, PURPL_SIZEOF_ARRAY(PURPL_SOURCE_DIR) - 1) == 0)
		file2 += PURPL_SIZEOF_ARRAY(PURPL_SOURCE_DIR);
#endif // !PURPL_LOG_FULL_PATHS

	base = logger->format;
	passes = 1;
	while (passes) {
		p = base;
		i = 0;
		while (*p) {
			if (p[0] != '#' || p[1] == '#') {
				if (p[0] == '#')
					p++;
				amt = 1;
				while (p[amt] && p[amt] != '#')
					amt++;
				strncpy(buf + i, p, amt);
				i += amt;
				p += amt;
				continue;
			}

			begin = p++;
			str = NULL;
			if (strncmp(p, "msg", 3) == 0) {
				p += 3;
				str = purpl_strdup(msg_fmt);
				passes++;
			} else if (*p == 't') {
				p++;
				bool twelve = (strcmp(p, "12") == 0);
				str = purpl_strfmt(NULL, "%s:#m:#s", twelve ? "#h12" : "#h");
				passes++;
			} else if (*p == 'h') {
				u64 h = t1->tm_hour;

				p++;
				if (strcmp(p, "12") == 0) {
					h %= 12;
					p += 2;
				}
				str = purpl_strfmt(NULL, "%02d", h);
			} else if (*p == 'm') {
				p++;
				str = purpl_strfmt(NULL, "%02d", t1->tm_min);
			} else if (*p == 's') {
				p++;
				str = purpl_strfmt(NULL, "%02d", t1->tm_sec);
			} else if (*p == 'j' || *p == 'd') {
				bool joke = (*p == 'j');
				bool short_form = false;
				
				if (joke)
					p++;
				if (*(p + 1) == 's') {
					p++;
					short_form = true;
				}

				str = purpl_strfmt(NULL, "%s %s/%s/%s", short_form ? "#wD" : "#WD,",
						   joke ? "#JD" : "#D", joke ? "#JM" : "#M", joke ? "#JY" : "#Y");

				p++;
				passes++;
			} else if (*p == 'J' || strncmp(p, "WD", 2) == 0 || strncmp(p, "wD", 2) == 0 || *p == 'D' ||
				   *p == 'M' || *p == 'Y') {
				struct tm *t = t1;
				int year = t->tm_year + 1900;
				bool short_form = false;
				bool wday_name = false;

				if (*p == 'J') {
					t = t2;
					p++;
					year = t->tm_year - 70;
				} else if (*p == 'w') {
					p++;
					short_form = true;
					wday_name = true;
				} else if (*p == 'W') {
					p++;
					wday_name = true;
				}

				if (*p == 'D' && wday_name && short_form)
					str = purpl_strndup(days[t->tm_wday], 3);
				else if (*p == 'D' && wday_name)
					str = purpl_strdup(days[t->tm_wday]);
				else if (*p == 'D')
					str = purpl_strfmt(NULL, "%02d", t->tm_mday);
				else if (strncmp(p, "MN", 2) == 0)
					str = purpl_strdup(months[t->tm_mon]);
				else if (*p == 'M')
					str = purpl_strfmt(NULL, "%02d", t->tm_mon + 1);
				else if (*p == 'Y')
					str = purpl_strfmt(NULL, "%04d", year);

				p++;
			} else if (*p == 'P') {
				p++;
				str = purpl_strfmt(NULL, "%lld", purpl_get_pid());
			} else if (*p == 'T') {
				p++;
				str = purpl_strfmt(NULL, "%lld", purpl_get_tid());
			} else if (*p == 'W') {
				p++;
				str = purpl_strdup("#F:#FL@#f");
				passes++;
			} else if (strncmp(p, "FL", 2) == 0) {
				p += 2;
				str = purpl_strfmt(NULL, "%d", line);
			} else if (*p == 'F') {
				p++;
				str = purpl_strdup(file2);
			} else if (*p == 'f') {
				p++;
				str = purpl_strdup(function);
			} else if (*p == 'L') {
				p++;
				str = purpl_strdup(levels_upper[level]);
			} else if (*p == 'l') {
				p++;
				str = purpl_strdup(levels_lower[level]);
			} else if (*p == 'V') {
				p++;
				str = purpl_strfmt(NULL, "v%s", purpl_format_version(PURPL_VERSION));
			} else if (*p == 'n') {
				p++;
				str = purpl_strdup(purpl_inst->app_name);
			} else if (*p == 'v') {
				p++;
				str = purpl_strfmt(NULL, "v%s", purpl_format_version(purpl_inst->app_version));
			} else {
				p = begin + 1;
				str = purpl_strdup(p);
			}

			len = strlen(str);
			tmp = purpl_strdup(buf);
			free(buf);
			buf_size += len;
			buf = calloc(buf_size, sizeof(char));
			if (!buf) {
				free(tmp);
				return NULL;
			}
			strncpy(buf, tmp, i);
			free(tmp);
			strncpy(buf + i, str, len);
			strncpy(buf + i + len, p, strlen(p));
			free(str);
			i += len;
		}
		base = purpl_strdup(buf);
		passes--;
	}

	free(base);
	free(msg_fmt);

	buf[i] = '\n';
	return buf;
}

PURPL_API void purpl_log_write(struct purpl_logger *logger, enum purpl_log_level level, const char *file, int line,
			       const char *function, const char *msg, ...)
{
	char *buf;
	char *file2;
	enum purpl_log_level effective_level;
	va_list args;

	if (!logger || !msg)
		return;

	effective_level = level;
	if (effective_level >= PURPL_LOG_LEVEL_MAX)
		effective_level = logger->max_level;
	else if (effective_level <= PURPL_LOG_LEVEL_CURRENT)
		effective_level = logger->level;

#ifndef PURPL_DEBUG
	if (effective_level == PURPL_LOG_LEVEL_DEBUG)
		effective_level = PURPL_LOG_LEVEL_INFO;
#endif

	if (effective_level > logger->max_level)
		return;

	file2 = purpl_pathfmt(NULL, file, 0, false, false);
	va_start(args, msg);
	buf = log_format(logger, effective_level, file2, line, function, msg, args);
	va_end(args);
	free(file2);

	fwrite(buf, sizeof(char), strlen(buf), logger->file);
	fflush(logger->file);
#ifdef _WIN32
	OutputDebugStringA(buf);
#else
	syslog(LOG_INFO, "%s", buf);
#endif // _WIN32
#ifdef PURPL_DEBUG
	fprintf(stderr, "\r%s", buf);
	fflush(stderr);
#else // PURPL_DEBUG
	if (level < PURPL_LOG_LEVEL_INFO) {
		fprintf(stderr, "\r%s", buf);
		fflush(stderr);
	}
#endif // PURPL_DEBUG

	free(buf);
}

PURPL_API enum purpl_log_level purpl_log_get_level(struct purpl_logger *logger)
{
	return logger->level;
}

PURPL_API enum purpl_log_level purpl_log_set_level(struct purpl_logger *logger, enum purpl_log_level level)
{
	enum purpl_log_level old;

	old = logger->level;
	if (level > PURPL_LOG_LEVEL_CURRENT)
		logger->level = level;

	if (level >= PURPL_LOG_LEVEL_MAX)
		logger->level = logger->max_level;

	return old;
}

PURPL_API enum purpl_log_level purpl_log_get_max_level(struct purpl_logger *logger)
{
	return logger->max_level;
}

PURPL_API enum purpl_log_level purpl_log_set_max_level(struct purpl_logger *logger, enum purpl_log_level level)
{
	enum purpl_log_level old;

	old = logger->max_level;
	if (level >= PURPL_LOG_LEVEL_MAX)
		logger->max_level = PURPL_LOG_LEVEL_MAX - 1;
	else if (level <= PURPL_LOG_LEVEL_CURRENT)
		logger->max_level = logger->level;
	else
		logger->max_level = level;

	return old;
}

PURPL_API void purpl_log_set_format(struct purpl_logger *logger, const char *format)
{
	char *message_format;

	if (!logger)
		return;

	message_format = purpl_strdup(format ? format : "#def");
	logger->format = purpl_strrplc(message_format, "#def", PURPL_LOG_DEFAULT_FORMAT, NULL);

	free(message_format);
}

PURPL_API void purpl_log_close(struct purpl_logger *logger, bool last_message)
{
	char *msg;
	const char *adjectives[] = { "nice", "good", "pleasant", "amazing" };
	const char *time_of_day = "day";
	struct tm *t;
	size_t random = purpl_random(PURPL_SIZEOF_ARRAY(adjectives) - 1);

	if (!logger)
		return;

	log_get_time(&t, NULL);

	if (last_message) {
		if (t->tm_hour >= 4 && t->tm_hour < 6)
			time_of_day = "dawn";
		else if (t->tm_hour >= 6 && t->tm_hour < 12)
			time_of_day = "morning";
		else if (t->tm_hour >= 12 && t->tm_hour < 17)
			time_of_day = "afternoon";
		else if (t->tm_hour >= 17 && t->tm_hour < 18)
			time_of_day = "evening";
		else if (t->tm_hour >= 18 && t->tm_hour < 22)
			time_of_day = "dusk";
		else if (t->tm_hour >= 22 || t->tm_hour < 4)
			time_of_day = "night";

		msg = purpl_strfmt(NULL, "This logger is shutting down. Have a%s %s %s.",
				   PURPL_ISVOWEL(adjectives[random][0]) ? "n" : "", adjectives[random], time_of_day);
		PURPL_LOG_INFO(purpl_inst->logger, "%s", msg);
		free(msg);
	}

	fclose(logger->file);
	free(logger->format);
	free(logger);
}
