// Parallelism stuff. Basically just wrappers around the SDL stuff.
//
// Note that the SDL atomic functions and thread local storage aren't included because I have no use for them and the
// documentation for the atomic stuff says "Seriously, here be dragons!". You can still use them by just linking your
// program to SDL though (use ${SDL2_LIBRARIES} in your CMakeLists.txt). For more detailed documentation, see
// https://wiki.libsdl.org/CategoryThread and https://wiki.libsdl.org/CategoryMutex
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

PURPL_API s32 purpl_thread_wait(purpl_thread_t thread)
{
	s32 ret = 0;

	SDL_WaitThread(thread, &ret);

	return ret;
}

PURPL_API s64 purpl_thread_get_id(purpl_thread_t thread)
{
	return SDL_GetThreadID(thread);
}

PURPL_API purpl_mutex_t purpl_mutex_create(void)
{
	return SDL_CreateMutex();
}

PURPL_API void purpl_mutex_destroy(purpl_mutex_t mutex)
{
	SDL_DestroyMutex(mutex);
}

PURPL_API bool purpl_mutex_lock(purpl_mutex_t mutex)
{
	return SDL_LockMutex(mutex) == 0;
}

PURPL_API u8 purpl_mutex_lock_noblock(purpl_mutex_t mutex)
{
	return SDL_TryLockMutex(mutex);
}

PURPL_API bool purpl_mutex_unlock(purpl_mutex_t mutex)
{
	return SDL_UnlockMutex(mutex) == 0;
}

PURPL_API purpl_semaphore_t purpl_semaphore_create(u32 value)
{
	return SDL_CreateSemaphore(value);
}

PURPL_API void purpl_semaphore_destroy(purpl_semaphore_t semaphore)
{
	SDL_DestroySemaphore(semaphore);
}

PURPL_API s32 purpl_semaphore_post(purpl_semaphore_t semaphore)
{
	return SDL_SemPost(semaphore) == 0;
}

PURPL_API bool purpl_semaphore_wait(purpl_semaphore_t semaphore)
{
	return SDL_SemWait(semaphore) == 0;
}

PURPL_API s8 purpl_semaphore_wait_noblock(purpl_semaphore_t semaphore)
{
	return SDL_SemTryWait(semaphore);
}

PURPL_API s8 purpl_semaphore_wait_timeout(purpl_semaphore_t semaphore, u32 timeout)
{
	return SDL_SemWaitTimeout(semaphore, timeout);
}

PURPL_API u32 purpl_semaphore_get(purpl_semaphore_t semaphore)
{
	return SDL_SemValue(semaphore);
}

PURPL_API purpl_condition_t purpl_condition_create(void)
{
	return SDL_CreateCond();
}

PURPL_API void purpl_condition_destroy(purpl_condition_t cond)
{
	SDL_DestroyCond(cond);
}

PURPL_API bool purpl_condition_broadcast(purpl_condition_t cond)
{
	return SDL_CondBroadcast(cond) == 0;
}

PURPL_API bool purpl_condition_signal(purpl_condition_t cond)
{
	return SDL_CondSignal(cond) == 0;
}

PURPL_API s8 purpl_condition_wait_timeout(purpl_condition_t cond, purpl_mutex_t mutex, u32 timeout)
{
	return SDL_CondWaitTimeout(cond, mutex, timeout);
}
