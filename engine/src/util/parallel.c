#include "SDL.h"

#include "purpl/util/parallel.h"

PURPL_API purpl_thread_t purpl_thread_create(purpl_thread_func_t func, const char *name, size_t stack_size, void *data)
{
	return SDL_CreateThreadWithStackSize(func, name ? name : "unnamed", stack_size, data);
}

PURPL_API void purpl_thread_detach(purpl_thread_t thread)
{
	SDL_DetachThread(thread);
}

PURPL_API s64 purpl_thread_get_id(purpl_thread_t thread)
{
	if (!thread)
		return 0;

	return SDL_GetThreadID(thread);
}

PURPL_API char *purpl_thread_get_name(purpl_thread_t thread)
{
	const char *name = SDL_GetThreadName(thread);

	// Assume no name means the main thread
	return purpl_strdup(name ? name : "main");
}

PURPL_API s32 purpl_thread_wait(purpl_thread_t thread)
{
	s32 ret = 0;

	SDL_WaitThread(thread, &ret);

	return ret;
}

