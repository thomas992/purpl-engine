// Definition of the engine instance structure
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

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "purpl/util/string.h"

#include "coredefs.h"
#include "types.h"

/// Levels for the logging functions
enum purpl_log_level {
	PURPL_LOG_LEVEL_CURRENT, // Use this to use the logger's current log
				 // level
	PURPL_LOG_LEVEL_CRITICAL = 0, // Very bad errors
	PURPL_LOG_LEVEL_ERROR, // Minor errors (like most of them)
	PURPL_LOG_LEVEL_WARNING, // Minor errors unlikely to impact anything
	PURPL_LOG_LEVEL_INFO, // Information
#ifdef PURPL_DEBUG
	PURPL_LOG_LEVEL_DEBUG, // Debugging messages
#endif
	PURPL_LOG_LEVEL_MAX, // Use this to use the logger's maximum level
};

/// A logger
struct purpl_logger {
	FILE *file; // Log file this logger logs to
	enum purpl_log_level level; // Current default log level
	enum purpl_log_level max_level; // Maximum log level (i.e. messages with a higher level will be discarded)
	char *format; // The format used for the messages
};

/// Initialize a logger
///
/// \param file The name of the log file to use
/// \param level The level to write messages at when PURPL_LOG_LEVEL_CURRENT is
///		 given to purpl_log_write
/// \param max_level The maximum level of messages to keep.
///		     pass PURPL_LOG_LEVEL_MAX to use the highest available level.
/// \param format The pattern for messages. The following sequences will be
///		  replaced with what is specified in their descriptions:
///
/// 	#t	- The current time in the format #h:#m:#s
///	#t12	- The current time in the format #h12:#m:#s
///	#h	- The current hour in 24-hour format
///	#h12	- The current hour in 12-hour format
///	#m	- The current minute of the current hour
///	#s	- The current second of the current minute
///
///	#d	- The current date in the format #WD, #D/#M/#Y, or #jd if it's April
///		  1st and PURPL_ENABLE_MEMING is defined (enable_meming in GN
///		  args)
///	#jd	- The current date in the format #WD, #JD/#JM/#JY
///	#D	- The current day of the month
///	#JD	- The current day of the month, but 32 if it's April 1st
///	#WD	- The name of the current day of the week
///	#M	- The current month (by name)
///	#JM	- The current month, but will be March if it's April 1st
///	#Y	- The current year
///	#JY	- The current year, minus 1970
///
///	#P	- The process ID of the engine
///	#T	- The thread ID of the current thread
///	#W	- The location of this call to whatever logging function was
///		  used in the format #F:#sl@#f
///	#F	- The source file the logger was called from
///	#f	- The function the logger was called from
///	#sl	- The line the logger was called from
///	#L	- The current log level in all caps
///	#l	- The current log level in lowercase
///
///	#msg	- The message logged (can be used multiple times, but don't do
///		  that)
///	#def	- The default pattern (passing NULL for format also gives
///		  this), which is equivalent to the following:
///		  "[ PID #P TID #T ] [ #d ] [ #t ] [ #W ] [ #L ]: #msg"
///
/// 	The sequences can also be used in purpl_log_write (and the macros
///	wrapping it), as they are processed by that function and #msg is
///	expanded before the other sequences.
///
/// \return Returns a pointer to the new logger
extern PURPL_API struct purpl_logger *
purpl_log_create(const char *file, enum purpl_log_level level,
		 enum purpl_log_level max_level,
		 const char *format);

/// Log a message to the given logger
///
/// \param logger The logger to write to
/// \param level The level to write the message at
/// \param file Pass __FILE__
/// \param line Pass __LINE__
/// \param function Pass PURPL_CURRENT_FUNCTION (because some compilers support
///		    nicer formats than __func__)
/// \param msg The message to write. Supports printf format specifiers.
extern PURPL_API void purpl_log_write(struct purpl_logger *logger,
				      enum purpl_log_level level,
				      const char *file, int line,
				      const char *function, const char *msg,
				      ...);

/// Set the given logger's default level
///
/// \param logger The logger to set the level of
/// \param level The new default level for the logger
///
/// \return Returns the previous level of the logger
extern PURPL_API enum purpl_log_level
purpl_log_set_level(struct purpl_logger *logger, enum purpl_log_level level);

/// Set the given logger's max level
///
/// \param logger The logger to set the max level of
/// \param max_level The new max level for the logger
///
/// \return Returns the previous level of the logger
extern PURPL_API enum purpl_log_level
purpl_log_set_max_level(struct purpl_logger *logger, enum purpl_log_level level);

/// Close the given logger
///
/// \param logger The logger to close
/// \param last_message Whether to write a final message to the log
extern PURPL_API void purpl_log_close(struct purpl_logger *logger,
				      bool last_message);

/// Log a message at level (without the PURPL_LOG_LEVEL_ prefix)
#define PURPL_LOG_WRITE(logger, level, msg, ...)                             \
	purpl_log_write(logger, PURPL_LOG_LEVEL_##level, __FILE__, __LINE__, \
			PURPL_CURRENT_FUNCTION, msg, __VA_ARGS__)

/// Log a message at the current log level
#define PURPL_LOG_CURRENT(logger, msg, ...) \
	PURPL_LOG_WRITE(logger, CURRENT, msg, __VA_ARGS__)

/// Log a message at PURPL_LOG_LEVEL_CRITICAL
#define PURPL_LOG_CRITICAL(logger, msg, ...) \
	PURPL_LOG_WRITE(logger, CRITICAL, msg, __VA_ARGS__)

/// Log a message at PURPL_LOG_LEVEL_ERROR
#define PURPL_LOG_ERROR(logger, msg, ...) \
	PURPL_LOG_WRITE(logger, ERROR, msg, __VA_ARGS__)

/// Log a message at PURPL_LOG_LEVEL_WARNING
#define PURPL_LOG_WARNING(logger, msg, ...) \
	PURPL_LOG_WRITE(logger, WARNING, msg, __VA_ARGS__)

/// Log a message at PURPL_LOG_LEVEL_INFO
#define PURPL_LOG_INFO(logger, msg, ...) \
	PURPL_LOG_WRITE(logger, INFO, msg, __VA_ARGS__)

#ifdef PURPL_DEBUG
/// Log a message at PURPL_LOG_LEVEL_DEBUG
#define PURPL_LOG_DEBUG(logger, msg, ...) \
	PURPL_LOG_WRITE(logger, DEBUG, msg, __VA_ARGS__)
#endif
