// Parallelism stuff. Basically just wrappers around the SDL stuff.
//
// Note that the SDL atomic functions and thread local storage aren't included because I have no use for them and the
// documentation for the atomic stuff says "Seriously, here be dragons!". You can still use them by just linking your
// program to SDL though (use ${SDL2_LIBRARIES} in your CMakeLists.txt). For more detailed documentation, see https://wiki.libsdl.org/CategoryThread and https://wiki.libsdl.org/CategoryMutex
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

#pragma once

#include "purpl/core/coredefs.h"
#include "purpl/core/types.h"

#include "string.h"

/// Opaque thread type
typedef void *purpl_thread_t;

/// Pointer to a function that can be run on a thread
typedef s32 (*purpl_thread_func_t)(void *data);

/// Opaque mutex type
typedef void *purpl_mutex_t;

/// Opaque semaphore type
typedef void *purpl_semaphore_t;

/// Opaque condition variable type
typedef void *purpl_condition_t;

/// Create a thread
///
/// \param func The function to execute on the thread
/// \param name The name of the thread (will be "unnamed" if NULL). The length limit for this is usually pretty short.
/// \param data Arbitrary data for the function, use a struct or an array of void pointers or something, and it will be
///		passed to the function
///
/// \return Returns a new thread or NULL
extern PURPL_API purpl_thread_t purpl_thread_create(purpl_thread_func_t func, const char *name, void *data);

/// Detach a thread (begin the thread and let it run to completion). Use this OR purpl_thread_wait, but not both
///
/// \param thread The thread to detach. Specify NULL for the current thread.
extern PURPL_API void purpl_thread_detach(purpl_thread_t thread);

/// Begin a thread, wait for it to complete, and then clean it up (must be called on all threads you create eventually)
///
/// \param thread The thread to wait for. The thread will cease to exist and become invalid after this function
/// 		  completes. Specify NULL for the current thread.
///
/// \return Returns the return value of the thread
extern PURPL_API s32 purpl_thread_wait(purpl_thread_t thread);

/// Get the ID of the given thread
///
/// \param thread The thread to get the ID of. Specify NULL for the current thread.
///
/// \return Returns the ID of the thread
extern PURPL_API s64 purpl_thread_get_id(purpl_thread_t thread);

/// Create a mutex
///
/// \return Returns a new mutex or NULL
extern PURPL_API purpl_mutex_t purpl_mutex_create(void);

/// Destroy a mutex
///
/// \param mutex The mutex to destroy
extern PURPL_API void purpl_mutex_destroy(purpl_mutex_t mutex);

/// Lock a mutex, block until it's locked or an error occurs
///
/// \param mutex The mutex to lock
///
/// \return Returns true if the mutex could be locked, false if there was an error
extern PURPL_API bool purpl_mutex_lock(purpl_mutex_t mutex);

/// Try to lock a mutex but return if it's not available
///
/// \param mutex The mutex to lock
///
/// \return Returns 0 if the mutex could be locked, 1 if the wait would block, and -1 if there was an error
extern PURPL_API u8 purpl_mutex_lock_noblock(purpl_mutex_t mutex);

/// Unlock a mutex
///
/// \param mutex The mutex to unlock
///
/// \return Returns true if the mutex was unlocked, false if there was an error (including it not being locked to begin with)
extern PURPL_API bool purpl_mutex_unlock(purpl_mutex_t mutex);

/// Create a semaphore
///
/// \param value The initial value for the semaphore
///
/// \return Returns a new semaphore, or NULL
extern PURPL_API purpl_semaphore_t purpl_semaphore_create(u32 value);

/// Destroy a semaphore
///
/// \param semaphore The semaphore to destroy
extern PURPL_API void purpl_semaphore_destroy(purpl_semaphore_t semaphore);

/// Atomically increment a semaphore's value and resume waiting threads
///
/// \param semaphore The semaphore to increment
///
/// \return Returns true on success, false if there was an error
extern PURPL_API s32 purpl_semaphore_post(purpl_semaphore_t semaphore);

/// Wait until a semaphore has a positive value and then decrement it
///
/// \param semaphore The semaphore to wait for
///
/// \return Returns true on success, false if an error occurs
extern PURPL_API bool purpl_semaphore_wait(purpl_semaphore_t semaphore);

/// purpl_semaphore_wait but with a timeout
///
/// \param semaphore The semaphore to wait for
/// \param timeout The maximum number of milliseconds to wait for the semaphore, or UINT32_MAX for no timeout
///
/// \return Returns 0 if successful, 1 if the wait times out, or -1 on an error
extern PURPL_API s8 purpl_semaphore_wait_timeout(purpl_semaphore_t semaphore, u32 timeout);

/// Decrement a semaphore if it has a positive value
///
/// \param semaphore The semaphore to decrement
///
/// \return Returns 0 if successful, 1 if the wait would block, or -1 if an error occurs
extern PURPL_API s8 purpl_semaphore_wait_noblock(purpl_semaphore_t semaphore);

/// Get the value of a semaphore
///
/// \param semaphore The semaphore to get the value of
///
/// \return Returns the value of the semaphore
extern PURPL_API u32 purpl_semaphore_get(purpl_semaphore_t semaphore);

/// Create a condition variable
///
/// \return Returns the condition variable or NULL
extern PURPL_API purpl_condition_t purpl_condition_create(void);

/// Destroy a condition variable
///
/// \param cond The condition variable to destroy
extern PURPL_API void purpl_condition_destroy(purpl_condition_t cond);

/// Resume all threads that are waiting on the condition variable
///
/// \param cond The condition variable to signal
///
/// \return Returns true if successful, false if not
extern PURPL_API bool purpl_condition_broadcast(purpl_condition_t cond);

/// Resume one thread that's waiting on the condition variable
///
/// \param cond The condition variable to signal
///
/// \return Returns true if successful, false if not
extern PURPL_API bool purpl_condition_signal(purpl_condition_t cond);

/// Wait until a condition variable is signaled
///
/// \param cond The condition variable to wait on
/// \param mutex The mutex to use for coordination
///
/// \return Returns true when signaled, or false if an error occurs
extern PURPL_API bool purpl_condition_wait(purpl_condition_t cond, purpl_mutex_t mutex);

/// purpl_condition_wait but with a timeout
///
/// \param cond The condition variable to wait on
/// \param mutex The mutex to use for coordination
/// \param timeout The maximum number of milliseconds to wait for the condition variable, or UINT32_MAX for no timeout,
/// 		   making the call behave like purpl_condition_wait
///
/// \return Returns 0 if the condition variable is signaled, 1 if the wait times out, or -1 if an error happens
extern PURPL_API s8 purpl_condition_wait_timeout(purpl_condition_t cond, purpl_mutex_t mutex, u32 timeout);

