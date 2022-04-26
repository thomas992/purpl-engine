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
#endif

#include "purpl/core/log.h"

#include "purpl/core/warnings.h"

PURPL_API struct purpl_logger *purpl_log_create(const char *file,
						enum purpl_log_level level,
						enum purpl_log_level max_level,
						const char *format)
{
	struct purpl_logger *logger;
	time_t t;
	struct tm *t2;
	char *date;
	char *filename;
	char *filename2;
	char *message_format;

	if (!purpl_inst)
		return NULL;

	logger = calloc(1, sizeof(struct purpl_logger));
	if (!logger)
		return NULL;

	t = time(NULL);
	t2 = localtime(&t);
	if (t2)
		date = purpl_strfmt(NULL, "%d-%d-%d", t2->tm_mday,
				    t2->tm_mon + 1, t2->tm_year + 1900);
	else
		date = purpl_strfmt(NULL, "?\?-?\?-????"); // Escapes are to
							   // prevent trigraph
							   // expansion

	filename2 = purpl_strrplc(file ? file : PURPL_LOG_DEFAULT_NAME,
				  "<date>", date, NULL);
	free(date);
	filename = purpl_pathfmt(NULL, filename2, 0, false, true);
	free(filename2);

	logger->file = fopen(filename, "ab+");
	if (!logger->file) {
		free(filename);
		return NULL;
	}
	free(filename);

	logger->max_level = max_level >= PURPL_LOG_LEVEL_MAX ?
				    PURPL_LOG_LEVEL_MAX - 1 :
					  max_level;
	logger->level = level > logger->max_level ? logger->max_level : level;

	message_format = purpl_strdup(format ? format : "#def");
#ifdef PURPL_DEBUG
	logger->format =
		purpl_strrplc(message_format, "#def",
			      "[PID #P TID #T] [#d #t] [#W] [#L] #msg", NULL);
#else // PURPL_DEBUG
	logger->format = purpl_strrplc(message_format, "#def",
				       "[#d #t] [#n #v] [#L] #msg", NULL);
#endif // PURPL_DEBUG

	free(message_format);

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

static char *log_format(struct purpl_logger *logger,
			enum purpl_log_level level, const char *file, int line,
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
	char *file2;
	struct tm *t1;
	struct tm *t2;

	va_list args;

	const char *days[] = { "Monday", "Tuesday",  "Wednesday", "Thursday",
			       "Friday", "Saturday", "Sunday" };

	const char *months[] = { "January", "February", "March",
				 "April",   "May",	"June",
				 "July",    "August",	"September",
				 "October", "November", "December" };

	const char *levels_lower[] = { "critical", "error", "warning", "info",
				       "debug" };
	const char *levels_upper[] = { "CRITICAL", "ERROR", "WARNING", "INFO",
				       "DEBUG" };

	va_copy(args, a);
	msg_fmt = purpl_vstrfmt(&msg_size, msg, args);
	va_end(args);

	buf_size = strlen(logger->format) + msg_size;
	buf = calloc(buf_size, sizeof(char));
	if (!buf)
		return NULL;

	log_get_time(&t1, &t2);

	if (strncmp(file, PURPL_SOURCE_DIR, PURPL_SIZEOF_ARRAY(PURPL_SOURCE_DIR) - 1) == 0)
		file2 = purpl_pathfmt(NULL, file + PURPL_SIZEOF_ARRAY(PURPL_SOURCE_DIR), 0, false, false);
	else
		file2 = purpl_pathfmt(NULL, file, 0, false, false);

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
			if (*p == 'm' && *++p == 's' && *++p == 'g') {
				p++;
				str = purpl_strdup(msg_fmt);
				passes++;
			} else if (*p == 't') {
				u64 h = t1->tm_hour;

				p++;
				if (strcmp(p, "12") == 0) {
					h %= 12;
					p += 2;
				}
				str = purpl_strfmt(NULL, "%02d:%02d:%02d", h,
						   t1->tm_min, t1->tm_sec);
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
				struct tm *t = joke ? t1 : t2;
				int year;

				if (joke)
					year = t->tm_year - 70;
				else
					year = t->tm_year + 1900;

				str = purpl_strfmt(NULL, "%s, %02d/%02d/%04d",
						   days[t->tm_wday],
						   t->tm_mday, t->tm_mon + 1,
						   year);

				p++;
			} else if (*p == 'J' || *p == 'D' || *p == 'M' ||
				   *p == 'Y') {
				struct tm *t = t1;
				int year = t->tm_year + 1900;

				if (*p == 'J') {
					t = t2;
					p++;
					year = t->tm_year - 70;
				}

				if (*p == 'D')
					str = purpl_strfmt(NULL, "%02d",
							   t->tm_mday);
				else if (*p == 'M' && *++p == 'N')
					str = purpl_strdup(months[t->tm_mon]);
				else if (*p == 'M')
					str = purpl_strfmt(NULL, "%02d",
							   t->tm_mon + 1);
				else if (*p == 'Y')
					str = purpl_strfmt(NULL, "%04d", year);

				p++;
			} else if (*p == 'P') {
				p++;
				str = purpl_strfmt(NULL, "%lld",
						   purpl_get_pid());
			} else if (*p == 'T') {
				p++;
				str = purpl_strfmt(NULL, "%lld",
						   purpl_get_tid());
			} else if (*p == 'W') {
				p++;
				str = purpl_strfmt(NULL, "%s:%d@%s", file2,
						   line, function);
			} else if (*p == 'F') {
				p++;
				str = purpl_strdup(file2);
			} else if (*p == 'f') {
				p++;
				str = purpl_strdup(function);
			} else if (*p == 's' && *++p == 'l') {
				p++;
				str = purpl_strfmt(NULL, "%d", line);
			} else if (*p == 'L') {
				p++;
				str = purpl_strdup(levels_upper[level]);
			} else if (*p == 'l') {
				p++;
				str = purpl_strdup(levels_lower[level]);
			} else if (*p == 'V') {
				p++;
				str = purpl_strdup(
					purpl_format_version(PURPL_VERSION));
			} else if (*p == 'n') {
				p++;
				str = purpl_strdup(purpl_inst->app_name);
			} else if (*p == 'v') {
				p++;
				str = purpl_strdup(purpl_format_version(
					purpl_inst->app_version));
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
			strncat(buf + i, str, len);
			strncat(buf + i + len, p, strlen(p));
			free(str);
			i += len;
		}
		base = purpl_strdup(buf);
		passes--;
	}
	
	free(base);
	free(file2);
	free(msg_fmt);

	buf[i] = 0;
	return buf;
}

PURPL_API void purpl_log_write(struct purpl_logger *logger,
			       enum purpl_log_level level, const char *file,
			       int line, const char *function, const char *msg,
			       ...)
{
	char *buf;
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

	va_start(args, msg);
	buf = log_format(logger, effective_level, file, line, function, msg,
			 args);
	va_end(args);

	fprintf(logger->file, "%s\n", buf);
	fflush(logger->file);
#ifdef PURPL_DEBUG
	printf("\r%s\n", buf);
	fflush(stdout);
#else // PURPL_DEBUG
#ifdef _WIN32
	OutputDebugStringA(buf);
#endif // _WIN32
	if (level < PURPL_LOG_LEVEL_INFO) {
		printf("\r%s", buf);
		fflush(stdout);
	}
#endif // PURPL_DEBUG

	free(buf);
}

PURPL_API enum purpl_log_level purpl_log_get_level(struct purpl_logger *logger)
{
	return logger->level;
}

PURPL_API enum purpl_log_level purpl_log_set_level(struct purpl_logger *logger,
						   enum purpl_log_level level)
{
	enum purpl_log_level old;

	old = logger->level;
	if (level > PURPL_LOG_LEVEL_CURRENT)
		logger->level = level;

	if (level >= PURPL_LOG_LEVEL_MAX)
		logger->level = logger->max_level;

	return old;
}

PURPL_API enum purpl_log_level
purpl_log_get_max_level(struct purpl_logger *logger)
{
	return logger->max_level;
}

PURPL_API enum purpl_log_level
purpl_log_set_max_level(struct purpl_logger *logger,
			enum purpl_log_level level)
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

PURPL_API void purpl_log_close(struct purpl_logger *logger, bool last_message)
{
	char *msg;
	const char *adjectives[] = { "nice", "good", "pleasant", "amazing" };
	const char *time_of_day = "day";
	struct tm *t;
	size_t random = purpl_random(PURPL_SIZEOF_ARRAY(adjectives)) - 1;

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

		msg = purpl_strfmt(
			NULL, "This logger is shutting down. Have a%s %s %s.",
			PURPL_ISVOWEL(adjectives[random][0]) ? "n" : "",
			adjectives[random], time_of_day);
		PURPL_LOG_INFO(purpl_inst->logger, "%s", msg);
		free(msg);
	}

	fclose(logger->file);
	free(logger->format);
	free(logger);
}
