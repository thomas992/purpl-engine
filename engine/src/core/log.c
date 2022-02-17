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

#include "purpl/core/log.h"

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

	filename = purpl_strrplc(file ? file : PURPL_LOG_DEFAULT_NAME,
				 "<date>", date, NULL);
	free(date);

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
#else
	logger->format = purpl_strrplc(message_format, "#def",
				       "[ #d ] [ #t ] [ #n #v ] [ #L ] #msg",
				       NULL);
#endif

	free(message_format);

	return logger;
}

static void purpl_log_get_time(struct tm **t1, struct tm **t2)
{
	time_t t;

	t = time(NULL);
	if (t1)
		*t1 = localtime(&t);
	if (t2) {
		*t2 = localtime(&t);
		(*t2)->tm_year -= 70; // tm_year is already year - 1900
		if ((*t2)->tm_mon == 3 && (*t2)->tm_mday == 1) {
			(*t2)->tm_mon = 2;
			(*t2)->tm_mday = 32;
		}
	}
}

static char *purpl_log_format(struct purpl_logger *logger,
			      enum purpl_log_level level, const char *file,
			      int line, const char *function, const char *msg,
			      va_list a)
{
	char *p1;
	char *p2;
	char *p3;
	struct tm *t1;
	struct tm *t2;
	va_list args;

	const char *days[] = { "Sunday",   "Monday", "Tuesday", "Wednesday",
			       "Thursday", "Friday", "Saturday" };

	const char *levels_upper[] = { "CRITICAL", "ERROR", "WARNING", "INFO",
				       "DEBUG" };
	const char *levels_lower[] = { "critical", "error", "warning", "info",
				       "debug" };

	// This function is only ever called by purpl_log_write (which checks
	// its parameters), so no parameter validation is necessary

	va_copy(args, a);

	// Unfortunately, doing several replacements in sequence is a pain in
	// the ass, but I think the tradeoff for singular ones being convenient
	// is worth it, and this is the only case where this should be
	// necessary, at least within the engine

	p1 = purpl_vstrfmt(NULL, msg, args);
	p2 = purpl_strrplc(logger->format, "#msg", p1, NULL);
	free(p1);

	purpl_log_get_time(&t1, &t2);

	p1 = purpl_strrplc(p2, "#t12", "#h12:#m:#s", NULL);
	free(p2);

	p2 = purpl_strrplc(p1, "#t", "#h:#m:#s", NULL);
	free(p1);

	p3 = purpl_strfmt(NULL, "%d", t1->tm_hour % 12);
	p1 = purpl_strrplc(p2, "#h12", p3, NULL);
	free(p2);
	free(p3);

	p3 = purpl_strfmt(NULL, "%d", t1->tm_hour);
	p2 = purpl_strrplc(p1, "#h", p3, NULL);
	free(p1);
	free(p3);

	p3 = purpl_strfmt(NULL, "%02d", t1->tm_min, NULL);
	p1 = purpl_strrplc(p2, "#m", p3, NULL);
	free(p2);
	free(p3);

	p3 = purpl_strfmt(NULL, "%02d", t1->tm_sec);
	p2 = purpl_strrplc(p1, "#s", p3, NULL);
	free(p1);
	free(p3);

#ifdef PURPL_ENABLE_MEMING
	if ((t1->tm_mon == 3 && t1->tm_mday == 1) ||
	    (t2->tm_mon == 2 && t2->tm_mday == 32)) {
		p3 = purpl_strrplc(p2, "#d", "#jd", NULL);
		free(p2);
		p2 = p3;
	}
#endif

	p1 = purpl_strrplc(p2, "#d", "#WD, #D/#M/#Y", NULL);
	free(p2);

	p2 = purpl_strrplc(p1, "#jd", "#WD, #JD/#JM/#JY", NULL);
	free(p1);

	p3 = purpl_strfmt(NULL, "%02d", t1->tm_mday);
	p1 = purpl_strrplc(p2, "#D", p3, NULL);
	free(p2);
	free(p3);

	p3 = purpl_strfmt(NULL, "%02d", t2->tm_mday);
	p2 = purpl_strrplc(p1, "#JD", p3, NULL);
	free(p1);
	free(p3);

	p3 = days[t1->tm_wday];
	p1 = purpl_strrplc(p2, "#WD", p3, NULL);
	free(p2);

	p3 = purpl_strfmt(NULL, "%02d", t1->tm_mon + 1);
	p2 = purpl_strrplc(p1, "#M", p3, NULL);
	free(p1);
	free(p3);

	p3 = purpl_strfmt(NULL, "%02d", t2->tm_mon + 1);
	p1 = purpl_strrplc(p2, "#JM", p3, NULL);
	free(p2);
	free(p3);

	p3 = purpl_strfmt(NULL, "%04d", t1->tm_year + 1970);
	p2 = purpl_strrplc(p1, "#Y", p3, NULL);
	free(p1);
	free(p3);

	p3 = purpl_strfmt(NULL, "%04d", t2->tm_year);
	p1 = purpl_strrplc(p2, "#JY", p3, NULL);
	free(p2);
	free(p3);

	p3 = purpl_strfmt(NULL, "%lu", purpl_get_pid());
	p2 = purpl_strrplc(p1, "#P", p3, NULL);
	free(p1);
	free(p3);

	p3 = purpl_strfmt(NULL, "%lu", purpl_get_tid());
	p1 = purpl_strrplc(p2, "#T", p3, NULL);
	free(p2);
	free(p3);

	p2 = purpl_strrplc(p1, "#W", "#F:#sl@#f", NULL);
	free(p1);

	p1 = purpl_strrplc(p2, "#F", file, NULL);
	free(p2);

	p2 = purpl_strrplc(p1, "#f", function, NULL);
	free(p1);

	p3 = purpl_strfmt(NULL, "%d", line);
	p1 = purpl_strrplc(p2, "#sl", p3, NULL);
	free(p2);
	free(p3);

	p3 = levels_upper[level];
	p2 = purpl_strrplc(p1, "#L", p3, NULL);
	free(p1);

	p3 = levels_lower[level];
	p1 = purpl_strrplc(p2, "#l", p3, NULL);
	free(p2);

	p2 = purpl_strrplc(p1, "#V", purpl_format_version(PURPL_VERSION),
			   NULL);
	free(p1);

	p1 = purpl_strrplc(
		p2, "#n",
		purpl_inst->app_name ? purpl_inst->app_name : "unknown", NULL);
	free(p2);

	p2 = purpl_strrplc(
		p1, "#v", purpl_format_version(purpl_inst->app_version), NULL);
	free(p1);

	va_end(args);

	return p2;
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
	buf = purpl_log_format(logger, effective_level, file, line, function,
			       msg, args);
	va_end(args);

	fprintf(logger->file, "%s\n", buf);
	if (level < PURPL_LOG_LEVEL_INFO)
		printf("%s\n", buf);
#ifdef PURPL_DEBUG
	if (level == PURPL_LOG_LEVEL_DEBUG) {
		printf("%s\n", buf);
#ifdef _WIN32
		DbgPrint_l("%s\n", buf);
#endif // _WIN32
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
	const char *adjectives[] = { "nice", "good" };
	const char *time_of_day = "day";
	struct tm *t;

	if (!logger)
		return;

	purpl_log_get_time(&t, NULL);

	if (last_message) {
		if (t->tm_hour < 12)
			time_of_day = "morning";
		else if (t->tm_hour >= 12 && t->tm_hour < 17)
			time_of_day = "afternoon";
		else if (t->tm_hour >= 17 && t->tm_hour < 18)
			time_of_day = "evening";
		else if (t->tm_hour >= 18)
			time_of_day = "night";

		msg = purpl_strfmt(
			NULL, "This logger is shutting down. Have a %s %s.",
			adjectives[PURPL_RANDOM(100) % 2 == 0 ? 0 : 1],
			time_of_day);
		PURPL_LOG_INFO(purpl_inst->logger, "%s", msg);
		free(msg);
	}

	fclose(logger->file);
	free(logger->format);
	free(logger);
}
