// Callbacks for bgfx to use
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

#include <bgfx/c99/bgfx.h>

#include "purpl/graphics/private/callbacks.h"

static void fatal_callback(bgfx_callback_interface_t *this, const char *file,
			   u16 line, bgfx_fatal_t code, const char *msg);
static void trace_vargs_callback(bgfx_callback_interface_t *this,
				 const char *file, u16 line,
				 const char *format, va_list args);
static void profiler_begin_callback(bgfx_callback_interface_t *this,
				    const char *name, u32 abgr,
				    const char *file, u16 line);
static void profiler_begin_literal_callback(bgfx_callback_interface_t *this,
					    const char *name, u32 abgr,
					    const char *file, u16 line);
static void profiler_end_callback(bgfx_callback_interface_t *this);
static u32 cache_get_size_callback(bgfx_callback_interface_t *this, u64 id);
static bool cache_read_callback(bgfx_callback_interface_t *this, u64 id,
				void *buf, u32 size);
static void cache_write_callback(bgfx_callback_interface_t *this, u64 id,
				 const void *data, u32 size);
static void screen_shot_callback(bgfx_callback_interface_t *this,
				 const char *file, u32 width, u32 height,
				 u32 pitch, const void *data, u32 size,
				 bool yflip);
static void capture_begin_callback(bgfx_callback_interface_t *this, u32 width,
				   u32 height, u32 pitch,
				   bgfx_texture_format_t format, bool yflip);
static void capture_end_callback(bgfx_callback_interface_t *this);
static void capture_frame_callback(bgfx_callback_interface_t *this,
				   const void *data, u32 size);

bgfx_callback_interface_t *purpl_init_bgfx_callbacks(void)
{
	bgfx_callback_interface_t *bgfx_callbacks;
	bgfx_callback_vtbl_t *vtbl;

	PURPL_LOG_INFO(purpl_inst->logger, "Setting up bgfx callback table");

	bgfx_callbacks = calloc(1, sizeof(bgfx_callback_interface_t));
	if (!bgfx_callbacks) {
		PURPL_LOG_ERROR(
			purpl_inst->logger,
			"Failed to allocate memory for the callback table: %s",
			purpl_strerror());
		return NULL;
	}

	bgfx_callbacks->vtbl = calloc(1, sizeof(bgfx_callback_vtbl_t));
	if (!bgfx_callbacks->vtbl) {
		PURPL_LOG_ERROR(
			purpl_inst->logger,
			"Failed to allocate memory for the callback table: %s",
			purpl_strerror());
		return NULL;
	}

	vtbl = (bgfx_callback_vtbl_t *)bgfx_callbacks->vtbl;
	vtbl->fatal = fatal_callback;
	vtbl->trace_vargs = trace_vargs_callback;
	vtbl->profiler_begin = profiler_begin_callback;
	vtbl->profiler_begin_literal = profiler_begin_literal_callback;
	vtbl->profiler_end = profiler_end_callback;
	vtbl->cache_read_size = cache_get_size_callback;
	vtbl->cache_read = cache_read_callback;
	vtbl->cache_write = cache_write_callback;
	vtbl->screen_shot = screen_shot_callback;
	vtbl->capture_begin = capture_begin_callback;
	vtbl->capture_end = capture_end_callback;
	vtbl->capture_frame = capture_frame_callback;

	return bgfx_callbacks;
}

static void fatal_callback(bgfx_callback_interface_t *this, const char *file,
			   u16 line, bgfx_fatal_t code, const char *msg)
{
	purpl_log_write(purpl_inst->logger, PURPL_LOG_LEVEL_ERROR, file, line,
			"<bgfx fatal>", "%.*s", (u32)(strlen(msg) - 1), msg);
#ifndef PURPL_DEBUG
	if (code != BGFX_FATAL_DEBUG_CHECK)
		exit(code);
#else // !PURPL_DEBUG
	purpl_debug_break();
#endif // !PURPL_DEBUG
}

static void trace_vargs_callback(bgfx_callback_interface_t *this,
				 const char *file, u16 line,
				 const char *format, va_list args)
{
	const char *buf;

	buf = purpl_vstrfmt(NULL, format, args);

	purpl_log_write(purpl_inst->logger, PURPL_LOG_LEVEL_DEBUG, file, line,
			PURPL_CURRENT_FUNCTION, "%.*s", (u32)(strlen(buf) - 1),
			buf);

	free(buf);
}

static void profiler_begin_callback(bgfx_callback_interface_t *this,
				    const char *name, u32 abgr,
				    const char *file, u16 line)
{
}

static void profiler_begin_literal_callback(bgfx_callback_interface_t *this,
					    const char *name, u32 abgr,
					    const char *file, u16 line)
{
}

static void profiler_end_callback(bgfx_callback_interface_t *this) {}

static u32 cache_get_size_callback(bgfx_callback_interface_t *this, u64 id)
{
	u32 size;
	char *path;

	if (!purpl_path_exists("cache"))
		return 0;

	path = purpl_strfmt(NULL, "cache/%X.bin", id);
	if (!purpl_path_exists(path)) {
		free(path);
		return 0;
	}

	size = purpl_get_size(path);
	
	free(path);

	return size;
}

static bool cache_read_callback(bgfx_callback_interface_t *this, u64 id,
				void *buf, u32 size)
{
	FILE *fp;
	char *path;

	if (!purpl_path_exists("cache"))
		purpl_mkdir("cache", PURPL_FS_MKDIR_RECURSE,
			    PURPL_FS_MODE_ALL);

	path = purpl_strfmt(NULL, "cache/%X.bin", id);
	fp = fopen(path, "wb+");
	if (!fp) {
		PURPL_LOG_ERROR(purpl_inst->logger,
				"Failed to open cache file %s: %s", path,
				purpl_strerror());
		return false;
	}

	if (fread(buf, 1, size, fp) < size) {
		PURPL_LOG_ERROR(purpl_inst->logger,
				"Failed to read %u bytes to cache file %s: %s",
				size, path, purpl_strerror());
		return false;
	}

	PURPL_LOG_INFO(purpl_inst->logger, "Read %u bytes from cache file %s",
		       size, path);

	fclose(fp);
	return true;
}

static void cache_write_callback(bgfx_callback_interface_t *this, u64 id,
				 const void *data, u32 size)
{
	FILE *fp;
	char *path;

	if (!purpl_path_exists("cache"))
		purpl_mkdir("cache", PURPL_FS_MKDIR_RECURSE,
			    PURPL_FS_MODE_ALL);

	path = purpl_strfmt(NULL, "cache/%X.bin", id);
	fp = fopen(path, "wb+");
	if (!fp) {
		PURPL_LOG_ERROR(purpl_inst->logger,
				"Failed to open cache file %s: %s", path,
				purpl_strerror());
		return;
	}

	if (fwrite(data, 1, size, fp) < size) {
		PURPL_LOG_ERROR(
			purpl_inst->logger,
			"Failed to write %u bytes to cache file %s: %s", size,
			path, purpl_strerror());
		return;
	}

	PURPL_LOG_INFO(purpl_inst->logger, "Wrote %u bytes to cache file %s",
		       size, path);

	fclose(fp);
}

static void screen_shot_callback(bgfx_callback_interface_t *this,
				 const char *file, u32 width, u32 height,
				 u32 pitch, const void *data, u32 size,
				 bool yflip)
{
}

static void capture_begin_callback(bgfx_callback_interface_t *this, u32 width,
				   u32 height, u32 pitch,
				   bgfx_texture_format_t format, bool yflip)
{
}

static void capture_end_callback(bgfx_callback_interface_t *this) {}

static void capture_frame_callback(bgfx_callback_interface_t *this,
				   const void *data, u32 size)
{
}
