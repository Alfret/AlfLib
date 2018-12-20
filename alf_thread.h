// MIT License
//
// Copyright (c) 2018 Filip Björklund
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef ALF_THREAD_H
#define ALF_THREAD_H

#if defined(__cplusplus)
extern "C" {
#endif

// ========================================================================== //
// Header includes
// ========================================================================== //

// Standard headers
#include <stdint.h>

// ========================================================================== //
// Type definitions
// ========================================================================== //

/** Boolean type **/
typedef uint8_t AlfBool;

// -------------------------------------------------------------------------- //

/** Prototype for a function that can be used as the starting-point of a newly 
 * spawned thread. 
 * \brief Thread start function.
 * \param argument Argument to the function.
 */
typedef uint32_t(*PFN_ThreadFunction)(void* argument);

// -------------------------------------------------------------------------- //

/** Predicate function that is used for condition variables.
 * \brief Predicate function.
 * \param argument Argument to predicate function.
 */
typedef AlfBool(*PFN_AlfPredicate)(void* argument);

// ========================================================================== //
// Macro declarations
// ========================================================================== //

// True value
#ifndef ALF_TRUE
	/** Value for a boolean representing true **/
#	define ALF_TRUE ((AlfBool)1)
#endif

// False value
#ifndef ALF_FALSE
	/** Value for a boolean representing false **/
#	define ALF_FALSE ((AlfBool)0)
#endif

// Constant for immediate timeout
#define ALF_THREAD_IMMEDIATELY (0)

// Allocation macros
#if !defined(ALF_THREAD_ALLOC)
#	include <stdlib.h>
	/** Allocation function. Size represents the amount of memory to allocate, in bytes **/
#	define ALF_THREAD_ALLOC(size) malloc(size)
#endif

// Allocation macros
#if !defined(ALF_THREAD_FREE)
#	include <stdlib.h>
	/** Free the memory of an allocation **/
#	define ALF_THREAD_FREE(mem) free(mem)
#endif

// Assert macros
#if !defined(ALF_THREAD_ASSERT)
#	include <assert.h>
	/** Internal assertion macro **/
#	define ALF_THREAD_ASSERT(cond, msg) assert(cond && msg)
#endif 

// Default thread name
#define ALF_DEFAULT_THREAD_NAME "Unknown"

// ========================================================================== //
// Structures
// ========================================================================== //

/** \enum AlfThreadResult
 * \author Filip Björklund
 * \date 03 oktober 2018 - 16:22
 * \brief
 * \details
 */
typedef enum AlfThreadResult
{
	/** Success **/
	ALF_THREAD_SUCCESS,
	/** Unknown error **/
	ALF_THREAD_UNKNOWN_ERROR,

	/** Invalid arguments **/
	ALF_THREAD_INVALID_ARGUMENTS,
	/** Invalid handle value **/
	ALF_THREAD_INVALID_HANDLE,
	/** Access denied **/
	ALF_THRAD_ACCESS_DENIED,
	/** Timeout **/
	ALF_THREAD_TIMEOUT,
	/** Caller is not owner of object **/
	ALF_THREAD_NOT_OWNER,
	/** Object is still in use when being destroyed **/
	ALF_THREAD_STILL_IN_USE
} AlfThreadResult;

// -------------------------------------------------------------------------- //

/** \enum AlfThreadPriority
 * \author Filip Björklund
 * \date 03 oktober 2018 - 16:23
 * \brief
 * \details
 */
typedef enum AlfThreadPriority
{
	/** Lowest thread priority **/
	ALF_THREAD_PRIORITY_LOWEST,
	/** Low thread priority **/
	ALF_THREAD_PRIORITY_LOW,
	/** Normal thread priority **/
	ALF_THREAD_PRIORITY_NORMAL,
	/** High thread priority **/
	ALF_THREAD_PRIORITY_HIGH,
	/** Highest thread priority **/
	ALF_THREAD_PRIORITY_HIGHEST,
	/** Critical thread priority **/
	ALF_THREAD_PRIORITY_CRITICAL
} AlfThreadPriority;

// -------------------------------------------------------------------------- //

/** \struct AlfThread
 * \author Filip Björklund
 * \date 03 oktober 2018 - 16:11
 * \brief Thread handle.
 * \details
 * Represents a handle to a single thread of execution on the system.
 */
typedef struct tag_AlfThread AlfThread;

// -------------------------------------------------------------------------- //

/** \struct AlfSemaphore
 * \author Filip Björklund
 * \date 03 oktober 2018 - 16:25
 * \brief Semaphore.
 * \details
 * 
 */
typedef struct tag_AlfSemaphore AlfSemaphore;

// -------------------------------------------------------------------------- //

/** \struct AlfMutex
 * \author Filip Björklund
 * \date 03 oktober 2018 - 16:25
 * \brief Mutex.
 * \details
 * 
 */
typedef struct tag_AlfMutex AlfMutex;

// -------------------------------------------------------------------------- //

/** \struct AlfConditionVariable
 * \author Filip Björklund
 * \date 03 oktober 2018 - 16:25
 * \brief Condition variable.
 * \details
 */
typedef struct tag_AlfConditionVariable AlfConditionVariable;

// -------------------------------------------------------------------------- //

/** \struct AlfReadWriteLock
 * \author Filip Björklund
 * \date 03 oktober 2018 - 16:25
 * \brief Read/Write lock
 * \details
 * Represents a lock that can be locked in both read and write mode. When it's 
 * locked in read mode all other threads that try to acquire it in read mode 
 * will succeed, and writers will fail. When, however, it's locked in write mode
 * it will only allow the thread that acquired it to write. Other threads trying
 * to acquire either read or write at the same time will fail.
 */
typedef struct tag_AlfReadWriteLock AlfReadWriteLock;

// -------------------------------------------------------------------------- //

/** \struct AlfTLSHandle
 * \author Filip Björklund
 * \date 03 oktober 2018 - 16:26
 * \brief Thread-local storage handle.
 * \details
 * Handle to thread-local storage. This is used as a key to store and retrieve
 * data that is local to the calling thread.
 */
typedef struct tag_AlfTLSHandle AlfTLSHandle;

// ========================================================================== //
// Functions
// ========================================================================== //

/** Function that must be called before the AlfThread library is used. This sets
 * up all the required data that is used by the library to keep track of thread
 * handles.
 * \brief Startup AlfThread.
 */
void alfThreadStartup(void);

// -------------------------------------------------------------------------- //

/** Function that can be called after the user is done using the AlfThread 
 * library. This will free up any data used by the library and also delete any 
 * handles to threads that might still be held onto by the library.
 * \brief Shutdown AlfThread.
 */
void alfThreadShutdown(void);

// ========================================================================== //
// Thread functions
// ========================================================================== //

/** Create a thread that executes the specified function with the specified 
 * arguments. The name of the thread is set to a the default name ("Unknown").
 * \note Handles to threads are not freed by the user but rather when the thread
 * is either joined or detached. Any dangling thread handles will also be freed
 * when the shutdown function is called.
 * \brief Create thread.
 * \param function Function to run on thread.
 * \param argument Argument passed to function.
 * \return Handle to thread.
 */
AlfThread* alfCreateThread(PFN_ThreadFunction function, void* argument);

// -------------------------------------------------------------------------- //

/** Create a named thread that executes the specified function with the 
 * specified arguments. The name must be valid UTF-8.
 * \note Handles to threads are not freed by the user but rather when the thread
 * is either joined or detached. Any dangling thread handles will also be freed
 * when the shutdown function is called.
 * \note As there is a limitation on thread name lengths in pthread, only the
 * first 15 characters will be displayed in a debugger. However the full name is
 * still retrieved from a call to AlfGetThreadName.
 * \brief Create thread.
 * \param function Function to run on thread.
 * \param argument Argument passed to thread function.
 * \param name Name of the thread.
 * \return Handle to thread.
 */
AlfThread* alfCreateThreadNamed(PFN_ThreadFunction function, void* argument, char* name);

// -------------------------------------------------------------------------- //

/** Returns the handle to the calling thread.
 * \brief Returns handle of calling thread.
 * \return Handle for the calling thread.
 */
AlfThread* alfThisThread(void);

// -------------------------------------------------------------------------- //

/** Wait for a thread. This will block until the thread represented by the 
 * handle is has finished executing its thread function.
 * \note Joining the same thread multiple times will result in undefined 
 * behaviour.
 * \brief Wait for thread.
 * \param thread Thread to wait for.
 * \return Exit code returned from the thread function.
 * \note Using a handle to a thread that has been joined is undefined behaviour 
 * and may also possibly end in a crash.
 */
uint32_t alfJoinThread(AlfThread* thread);

// -------------------------------------------------------------------------- //

/** Try to join a thread. If the thread has not yet exited then the function 
 * returns false. Else the thread will be joined and the function returns true.
 * \note Joining the same thread multiple times results in undefined behaviour.
 * \note Using a handle to a thread that has been joined is undefined behaviour
 * and may also possibly end in a crash.
 * \brief Wait for thread.
 * \param thread Thread to try to join.
 * \param exitCodeOut exit code from thread function, not set if function 
 * returns false.
 * \return True if the thread could be joined else false.
 */
AlfBool alfJoinThreadTry(AlfThread* thread, uint32_t* exitCodeOut);

// -------------------------------------------------------------------------- //

/** Detach a thread. This will let the thread continue running without having to
 * be joined at a later time. All resources used by the thread are releases when
 * the thread exits.
 * \note Using a handle to a thread that has been detached results in undefined
 * behaviour.
 * \note A thread that has been detached cannot be joined.
 * \brief Detach thread.
 * \param thread Handle to the thread that is to be detached.
 * \return Result:
 * - ALF_THREAD_SUCCESS: Successfully detached thread.
 * - ALF_THREAD_UNKNOWN_ERROR: Unknown error.
 * - ALF_THREAD_INVALID_HANDLE: Invalid thread handle.
 */
AlfThreadResult alfDetachThread(AlfThread* thread);

// -------------------------------------------------------------------------- //

/** Kill a executing thread abruptly.
 * \brief Kill thread.
 * \param thread Thread to kill.
 */
void alfKillThread(AlfThread* thread);

// -------------------------------------------------------------------------- //

/** Stop the execution of the calling thread. The exit code returned from the 
 * thread is that of the argument to this function.
 * \brief Exit thread.
 * \param exitCode Exit code from thread.
 */
void alfExitThread(uint32_t exitCode);

// -------------------------------------------------------------------------- //

/** Yield the execution of the calling thread. This means that another thread
 * might be scheduled (if available).
 * \brief Yield calling thread.
 */
void alfYieldThread(void);

// -------------------------------------------------------------------------- //

/** Set the priority of a thread. This priority will affect how the operationg
 * system schedules the threads.
 * \brief Set thread priority.
 * \param thread Thread to set priority of.
 * \param priority Prioriy to set.
 * \return Result:
 * - ALF_THREAD_SUCCESS: Successfully set thread priority.
 * - ALF_THREAD_UNKNOWN_ERROR: Unknown error.
 * - ALF_THREAD_INVALID_HANDLE: Invalid thread handle.
 * - ALF_THREAD_INVALID_PRIORITY: Invalid priority.
 * - ALF_THREAD_ACCESS_DENIED: Caller is not allowed to set thread priority.
 */
AlfThreadResult alfSetThreadPriority(AlfThread* thread, AlfThreadPriority priority);

// -------------------------------------------------------------------------- //

/** Sleep the calling thread for the specified number of milliseconds. The
 * precision of the sleep depends on the platform and implementation.
 * \brief Sleep the calling thread for a specified number of milliseconds.
 * \param milliseconds Number of milliseconds to sleep thread for.
 */
void alfSleepThread(uint64_t milliseconds);

// -------------------------------------------------------------------------- //

/** Returns the id of the specified thread.
 * \brief Returns id of specified thread.
 * \param thread Thread to get ID of.
 * \return ID of the thread.
 */
uint32_t alfGetThreadID(AlfThread* thread);

// -------------------------------------------------------------------------- //

/** Returns the name of the calling thread. This name is owned by the thread and
 * must not be freed by the user.
 * \brief Returns name of thread.
 * \return Name of thread.
 */
const char* alfGetThreadName(void);

// -------------------------------------------------------------------------- //

/** Sets the name of the calling thread. The string will be copied and stored by
 * the thread internally.
 * \brief Set thread name.
 * \param name Name to set.
 * \return Result:
 * - ALF_PTHREAD_SUCCESS: Successfully set thread name.
 * - ALF_PTHREAD_UNKNOWN_ERROR: Unknown error.
 */
AlfThreadResult alfSetThreadName(const char* name);

// ========================================================================== //
// Semaphore functions
// ========================================================================== //

/** Create a semaphore that is initialized with the specified value.
 * \brief Create semaphore.
 * \param initialValue Initial semaphore value.
 * \return Created semaphore.
 */
AlfSemaphore* alfCreateSemaphore(uint64_t initialValue);

// -------------------------------------------------------------------------- //

/** Delete a semaphore.
 * \note There must be no threads waiting on the semaphore when it's being 
 * deleted. Or else they will never be able to proceed.
 * \brief Delete semaphore
 * \param semaphore Semaphore to delete.
 */
void alfDeleteSemaphore(AlfSemaphore* semaphore);

// -------------------------------------------------------------------------- //

/** Acquire a semaphore. This will block until the semaphore value is at least
 * one, then it will be decremented and the function will return.
 * \brief Acquire Semaphore.
 * \param semaphore Semaphore to acquire.
 * \return Result:
 * - ALF_THREAD_SUCCESS: Successfully acquired semaphore.
 * - ALF_THREAD_UNKNOWN_ERROR: Unknown error.
 * - ALF_THREAD_TIMEOUT: Timed out.
 * - ALF_THREAD_INVALID_HANDLE: Invalid semaphore handle.
 */
AlfThreadResult alfAcquireSemaphore(AlfSemaphore* semaphore);

// -------------------------------------------------------------------------- //

/** Try to acquire a semaphore. This will block either until the semaphore is
 * released (i.e. value is greater than 0) or until the timeout expires.
 * \brief Acquire semaphore with timeout.
 * \param semaphore Semaphore to acquire.
 * \param milliseconds Milliseconds until timing out.
 * \return Result:
 * - ALF_THREAD_SUCCESS: Successfully acquired semaphore.
 * - ALF_THREAD_UNKNOWN_ERROR: Unknown error.
 * - ALF_THREAD_TIMEOUT: Timed out. 
 * - ALF_THREAD_INVALID_HANDLE: Invalid semaphore handle.
 */
AlfThreadResult alfAcquireSemaphoreTimed(AlfSemaphore* semaphore, uint64_t milliseconds);

// -------------------------------------------------------------------------- //

/** Checks if the semaphore can be acquired and acquires it if possible. If the
 * semaphore cannot immediately be acquired then the function returns false and
 * the semaphore is not acquired.
 * \note The result of calling this function is the same as calling 
 * alfAcquireSemaphoreTimed with a timeout of 0 (ALF_THREAD_IMMEDIATELY).
 * \brief Try to acquire semaphore.
 * \param semaphore Semaphore to try to acquire.
 * \return True if the semaphore was acquired else false.
 */
AlfBool alfAcquireSemaphoreTry(AlfSemaphore* semaphore);

// -------------------------------------------------------------------------- //
/** Release semaphore. This will increment the semaphore value by one and then
 * return.
 * \brief Release semaphore.
 * \param semaphore Semaphore to release.
 * \return Result:
 * - ALF_THREAD_SUCCESS: Successfully released semaphore.
 * - ALF_THREAD_UNKNOWN_ERROR: Unknown error.
 * - ALF_THREAD_INVALID_HANDLE: Invalid semaphore handle.
 * - ALF_SEMAPHORE_OVERFLOW: Maximum semaphore value reached.
 */
AlfThreadResult alfReleaseSemaphore(AlfSemaphore* semaphore);

// ========================================================================== //
// Mutex functions
// ========================================================================== //

/** Create a mutex. The parameter is used to determine if the mutex should be 
 * created with support for recursive locking.
 * \brief Create mutex.
 * \param recursive Whether the mutex can be recursively locked.
 * \return Created mutex.
 */
AlfMutex* alfCreateMutex(AlfBool recursive);

// -------------------------------------------------------------------------- //

/** Delete a mutex. 
 * \note The user is responsible for making sure that the mutex is not held by
 * any thread when it is destroyd, as that results in undefined behaviour.
 * \note This function won't fail on Windows.
 * \brief Delete mutex.
 * \param mutex Mutex to destroy.
 * \return Result:
 * - ALF_THREAD_SUCCESS: Successfully destroyed mutex.
 * - ALF_THREAD_UNKNOWN_ERROR: Unknown error.
 * - ALF_THREAD_INVALID_HANDLE: Invalid mutex handle.
 * - ALF_THREAD_STILL_IN_USE: Mutex is still locked by a thread.
 */
AlfThreadResult alfDeleteMutex(AlfMutex* mutex);

// -------------------------------------------------------------------------- //
/** Acquire a mutex. This function blocks until any other thread that holds the
 * mutex releases it.
 * \brief Acquire mutex.
 * \param mutex Mutex to acquire.
 * \return Result:
 * - ALF_THREAD_SUCCESS: Successfully acquired mutex.
 * - ALF_THREAD_UNKNOWN_ERROR: Unknown error.
 * - ALF_THREAD_INVALID_HANDLE: Invalid mutex handle.
 * - ALF_THREAD_DEADLOCK: A non-recursive mutex was locked recursively.
 */
AlfThreadResult alfAcquireMutex(AlfMutex* mutex);

// -------------------------------------------------------------------------- //

/** Try to acquire a mutex. If the mutex is not locked by another thread it will
 * be locked and the function returns true. If the mutex on the other hand is 
 * locked by another thread then this function returns false.
 * \brief Try to acquire mutex.
 * \param mutex Mutex to acquire.
 * \return True if the mutex was acquired else false.
 */
AlfBool alfAcquireMutexTry(AlfMutex* mutex);

// -------------------------------------------------------------------------- //

/** Release mutex that is held by the calling thread. Calling this when the 
 * calling thread does not already hold the mutex will result in no operation.
 * \brief Release mutex.
 * \param mutex Mutex to release.
 * \return Result:
 * - ALF_THREAD_SUCCESS: Successfully released mutex.
 * - ALF_THREAD_UNKNOWN_ERROR: Unknown error.
 * - ALF_THREAD_INVALID_HANDLE: Invalid mutex handle.
 * - ALF_THREAD_ALREADY_LOCKED: Mutex is not locked.
 */
AlfThreadResult alfReleaseMutex(AlfMutex* mutex);

// ========================================================================== //
// Condition variable functions
// ========================================================================== //

/** Create a condition variable.
 * \brief Construct condition variable.
 * \return Created condition variable.
 */
AlfConditionVariable* alfCreateConditionVariable(void);

// -------------------------------------------------------------------------- //

/** Delete condition variable.
 * \note It's up to the user to make sure that there are no threads still 
 * waiting on the condition variable when it's destroyed.
 * \note This function won't fail on Windows.
 * \brief Delete condition variable.
 * \param conditionVariable Condition variable to destroy.
 * \return Result:
 * - ALF_THREAD_SUCCESS: Successfully destroyed condition variable.
 * - ALF_THREAD_UNKNOWN_ERROR: Unknown error.
 * - ALF_THREAD_INVALID_HANDLE: Invalid condition variable handle.
 * - ALF_THREAD_IN_USE: Condition variable is still in use.
 */
AlfThreadResult alfDeleteConditionVariable(AlfConditionVariable* conditionVariable);

// -------------------------------------------------------------------------- //

/** Wait for condition variable to be notified. This function may return early
 * due to so-called spurious waking. The predicate version of this function can
 * be used to handle this.
 * \brief Wait for condition variable.
 * \param conditionVariable Condition variable to wait for.
 * \param mutex Mutex that is unlocked during the wait.
 * \return Error codes:
 * - ALF_THREAD_SUCCESS: Successfully waited for condition variable.
 * - ALF_THREAD_UNKNOWN_ERROR: Unknown error.
 * - ALF_THREAD_INVALID_HANDLE: Invalid condition variable or mutex handle.
 * - ALF_THREAD_TIMEOUT: Wait timed out.
 * - ALF_THREAD_NOT_OWNER: The mutex is not owned by the calling
 * thread.
 */
AlfThreadResult alfWaitConditionVariable(AlfConditionVariable* conditionVariable, AlfMutex* mutex);

// -------------------------------------------------------------------------- //

/** Wait for a condition variable to be notified and the predicate to hold. If
 * the condition variable is randomly notified without the condition holding
 * then the thread will go back to waiting on the condition. Condition variables
 * may be randomly awoken due to so-called spurious wakes, this function is used
 * to avoid this.
 * \brief Wait for condition variable and predicate.
 * \param conditionVariable Condition variable to wait for.
 * \param mutex Mutex to unlock during wait.
 * \param predicate Predicate that must hold when the condition is signaled.
 * \param predicateArgument Argument to the predicate function.
 */
void alfWaitConditionVariablePredicate(AlfConditionVariable* conditionVariable, AlfMutex* mutex, PFN_AlfPredicate predicate, void* predicateArgument);

// -------------------------------------------------------------------------- //

/** Notify one thread that is waiting on condition variable.
 * \brief Notify one waiting thread on condition variable.
 * \param conditionVariable Condition variable to notify threads waiting on.
 */
void alfNotifyConditionVariable(AlfConditionVariable* conditionVariable);

// -------------------------------------------------------------------------- //

/** Notify all threads that are waiting on condition variable.
 * \brief Notify all waiting threads on condition variable.
 * \param conditionVariable Condition variable to notify threads waiting on.
 */
void alfNotifyAllConditionVariables(AlfConditionVariable* conditionVariable);

// ========================================================================== //
// Read/Write lock functions
// ========================================================================== //


// ========================================================================== //
// TLS functions
// ========================================================================== //

/** Acquire a handle that is used to store thread-local data.
 * \note If there are not TLS handles available then the function returns NULL.
 * \brief Acquire TLS handle.
 * \return TLS handle or NULL on failure.
 */
AlfTLSHandle* alfGetTLS();

// -------------------------------------------------------------------------- //

/** Return a handle to a thread-local data store to the OS.
 * \note The user must not try to use the handle to store or load data after the
 * handle has been destroyed.
 * \brief Return TLS handle..
 * \param handle TLS Handle to return.
 */
void alfReturnTLS(AlfTLSHandle* handle);

// -------------------------------------------------------------------------- //

/** Store data in a thread-local store that corresponds to a TLS handle.
 * \brief Store TLS data.
 * \param handle Handle to TLS store.
 * \param data Data to store.
 */
void alfStoreTLS(AlfTLSHandle* handle, void* data);

// -------------------------------------------------------------------------- //
/** Load data from a thread-local store that corresponds to a TLS handle.
 * \brief Load TLS data.
 * \param handle Handle TLS store.
 * \return Loaded data.
 */
void* alfLoadTLS(AlfTLSHandle* handle);

// ========================================================================== //
// Implementation
// ========================================================================== //

#if defined(ALF_THREAD_IMPL)

// ========================================================================== //
// Private header includes
// ========================================================================== //

// Platform detection
#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW64__)
#	define ALF_THREAD_TARGET_WINDOWS
#	define NOMINMAX
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	include <process.h>
#elif defined(__linux__)
#	define ALF_THREAD_TARGET_LINUX
#	define ALF_THREAD_PTHREAD
#	include <semaphore.h>
#	include <zconf.h>
#	include <errno.h>
#elif defined (__APPLE__)
#	define ALF_THREAD_TARGET_MACOS
#	define ALF_THREAD_PTHREAD
#	include <dispatch/dispatch.h>
#	include <errno.h>
#else
UNSUPPORTED_PLATFORM
#endif

// Pthread header
#if defined(ALF_THREAD_PTHREAD)
#	include <pthread.h>
#endif

// ========================================================================== //
// Structures
// ========================================================================== //

typedef struct tag_AlfThread
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	/** Thread handle**/
	HANDLE handle;
#elif defined(ALF_THREAD_PTHREAD)
	/** Thread handle**/
	pthread_t handle;
#endif
	/** Thread id **/
	uint32_t id;
	/** Name of thread **/
	char* name;
	/** Whether the thread has been detached **/
	AlfBool detached;
} tag_AlfThread;

// -------------------------------------------------------------------------- //

typedef struct tag_AlfSemaphore
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	HANDLE handle;
#elif defined(ALF_THREAD_TARGET_LINUX)
	sem_t handle;
#elif defined(ALF_THREAD_TARGET_MACOS)
	dispatch_semaphore_t handle;
#endif
} tag_AlfSemaphore;

// -------------------------------------------------------------------------- //

typedef struct tag_AlfMutex
{
	/** Whether mutex supports recursive locking **/
	AlfBool recursive;
#if defined(ALF_THREAD_TARGET_WINDOWS)
	union
	{
		/** Windows slim reader-writer lock handle for non-recursive mutexes **/
		SRWLOCK srwlock;
		/** Windows critical section for recursive mutexes **/
		CRITICAL_SECTION criticalSection;
	};
#elif defined(ALF_THREAD_PTHREAD)
	/** Pthread mutex handle **/
	pthread_mutex_t handle;
	/** Pthread mutex attributes **/
	pthread_mutexattr_t attributes;
#endif
} tag_AlfMutex;

// -------------------------------------------------------------------------- //

typedef struct tag_AlfConditionVariable
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	/** Windows condition variable handle **/
	CONDITION_VARIABLE handle;
#elif defined(ALF_THREAD_PTHREAD)
	/** Pthread condition variable handle **/
	pthread_cond_t handle;
#endif
} tag_AlfConditionVariable;

// -------------------------------------------------------------------------- //

typedef struct tag_AlfReadWriteLock
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	/** Windows slim read/write lock handle **/
	SRWLOCK handle;
#elif defined(ALF_THREAD_PTHREAD)
	/** Pthread read/write lock handle **/
	pthread_rwlock handle;
#endif
} tag_AlfReadWriteLock;

// -------------------------------------------------------------------------- //

typedef struct tag_AlfTLSHandle
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	/** Windows TLS handle **/
	DWORD handle;
#elif defined(ALF_THREAD_PTHREAD)
	/** Pthread TLS handle **/
	pthread_key_t handle;
#endif
} tag_AlfTLSHandle;

// ========================================================================== //
// Private structures
// ========================================================================== //

/** List node structure **/
typedef struct AlfNode
{
	/** Data pointer **/
	void* data;
	/** Pointer to next node **/
	struct AlfNode* next;
} AlfNode;

// -------------------------------------------------------------------------- //

/** Global data for AlfThread library **/
typedef struct AlfGlobalData
{
	/** Mutex for global data locking **/
	AlfMutex* mutex;
	/** TLS handle for thread handle data **/
	AlfTLSHandle* handleTLS;

	/** Handle to node of first external thread **/
	AlfNode* externalThreads;
} AlfGlobalData;

// -------------------------------------------------------------------------- //

/** Internal thread data struct. Passed to thread functions **/
typedef struct
{
	/** Userdefined thread function **/
	PFN_ThreadFunction function;
	/** Argument to thread function **/
	void* argument;
	/** Name of thread **/
	const char* name;
	/** Thread handle pointer to store in tls **/
	AlfThread* handle;

	/** Semaphore to release when all data from this struct has been retrieved **/
	AlfSemaphore* semaphore;
	/** Whether the thread started correctly **/
	AlfBool success;
} AlfThreadData;

// ========================================================================== //
// Global data
// ========================================================================== //

/** Global data **/
static AlfGlobalData gData;

// ========================================================================== //
// Private functions
// ========================================================================== //

/** Free a thread handle **/
static void _alfFreeThreadHandle(AlfThread* thread)
{
	if (thread->name)
	{
		ALF_THREAD_FREE(thread->name);
	}
	ALF_THREAD_FREE(thread);
}

// -------------------------------------------------------------------------- //

/** Add a thread that was not started by AlfThread to the list of handled threads **/
static void _alfAddExternalThread(AlfThread* thread)
{
	// Acquire global data mutex
	alfAcquireMutex(gData.mutex);

	// Find last node
	AlfNode** current = &gData.externalThreads;
	while (*current)
	{
		// Go to next node
		current = &(*current)->next;
	}

	// Append new node
	AlfNode* node = ALF_THREAD_ALLOC(sizeof(AlfNode));
	node->next = NULL;
	node->data = thread;
	*current = node;

	// Release global data mutex
	alfReleaseMutex(gData.mutex);
}

// ========================================================================== //
// Windows-specific functions
// ========================================================================== //

#if defined(ALF_THREAD_TARGET_WINDOWS)

/** Prototype for GetThreadDescription function **/
typedef HRESULT(*PFN_GetThreadDescription)(HANDLE thread_handle, PWSTR* thread_description);

// -------------------------------------------------------------------------- //

/** Prototype for SetThreadDescription function **/
typedef HRESULT(*PFN_SetThreadDescription)(HANDLE thread_handle, PCWSTR thread_description);

// -------------------------------------------------------------------------- //

static wchar_t* _alfUTF8ToUTF16(const char* string)
{
	// if the string is NULL then return
	if (!string) { return NULL; }

	// Get the byte count
	const uint64_t byteCount = strlen(string);
	if (byteCount < 1) { return NULL; }

	// Get the length of the converted string
	const int32_t convertedLength = MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		string,
		(int32_t)byteCount,
		NULL,
		0
	);

	// Assert that the string can be converted
	ALF_THREAD_ASSERT(convertedLength > 0, "Invalid UTF-8 string");

	// Allocate the utf16 string
	wchar_t* buffer = ALF_THREAD_ALLOC(sizeof(wchar_t) * (convertedLength + 1));
	buffer[convertedLength] = 0;

	// Convert the string
	MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		string,
		(int32_t)byteCount,
		buffer,
		convertedLength
	);

	// Return the utf16 string
	return buffer;
}

// -------------------------------------------------------------------------- //

static char* _alfUTF16ToUTF8(const wchar_t* string)
{
	// if the string is NULL then return
	if (!string) { return NULL; }

	// Get the byte count
	const uint64_t byteCount = wcslen(string);
	if (byteCount < 1) { return NULL; }

	// Get the length of the converted string
	const int32_t convertedLength = WideCharToMultiByte(
		CP_UTF8,
		WC_ERR_INVALID_CHARS,
		string,
		(int32_t)byteCount,
		NULL,
		0,
		NULL,
		NULL
	);

	// Assert that the string can be converted
	ALF_THREAD_ASSERT(convertedLength > 0, "Invalid UTF-16 string");

	// Allocate the utf16 string
	char* buffer = ALF_THREAD_ALLOC(sizeof(char) * (convertedLength + 1));
	buffer[convertedLength] = 0;

	// Convert the string
	WideCharToMultiByte(
		CP_UTF8,
		WC_ERR_INVALID_CHARS,
		string,
		(int32_t)byteCount,
		buffer,
		convertedLength,
		NULL,
		NULL
	);

	// Return the utf16 string
	return buffer;
}

// -------------------------------------------------------------------------- //

static uint32_t _alfWin32ThreadStart(void* argument)
{
	// Cast the argument to thread data and retrieve functiona and argument
	const AlfThreadData* data = (AlfThreadData*)argument;
	const PFN_ThreadFunction function = data->function;
	void* user_argument = data->argument;

	// Store thread handle
	alfStoreTLS(gData.handleTLS, data->handle);

	// Set the thread name
	alfSetThreadName(data->name);

	// Release the semaphore
	alfReleaseSemaphore(data->semaphore);

	// Call the function
	const uint32_t result = function(user_argument);

	// Cleanup thread if detached
	AlfThread* thread = alfThisThread();
	if (thread->detached)
	{
		_alfFreeThreadHandle(thread);
	}

	// Return result and end thread
	return result;
}
#endif // defined(ALF_THREAD_TARGET_WINDOWS)

// ========================================================================== //
// Pthread-specific functions
// ========================================================================== //

/** Thread start function **/
#if defined(ALF_THREAD_PTHREAD)

void _alfMillisecondsToTimespec(uint32_t ms, struct timespec* time)
{
	time->tv_sec = ms / 1000;
	time->tv_nsec = (ms % 1000) * 1000000;
}

// -------------------------------------------------------------------------- //

static void* _alfPthreadThreadStart(void* argument)
{
	// Cast the argument to thread data and retrieve functiona and argument
	const AlfThreadData* data = (AlfThreadData*)argument;
	const PFN_ThreadFunction function = data->function;
	void* user_argument = data->argument;

	// Store thread handle
	alfStoreTLS(gData.handleTLS, data->handle);

	// Set the thread name
	alfSetThreadName(data->name);

	// Release the semaphore
	alfReleaseSemaphore(data->semaphore);

	// Call the function
	const uint32_t result = function(user_argument);

	// Cleanup thread if detached
	AlfThread* thread = alfThisThread();
	if (thread->detached)
	{
		_alfFreeThreadHandle(thread);
	}

	// Return result and end thread
	return result;
}
#endif // defined(ALF_THREAD_TARGET_WINDOWS)
// ========================================================================== //
// Thread functions
// ========================================================================== //

inline void alfThreadStartup(void)
{
	gData.handleTLS = alfGetTLS();
	gData.mutex = alfCreateMutex(ALF_FALSE);
}

// -------------------------------------------------------------------------- //

inline void alfThreadShutdown()
{
	// Free all external node handles
	AlfNode* current = gData.externalThreads;
	while (current)
	{
		// Store old node and step to next
		AlfNode* old = current;
		current = current->next;

		// Cleanup the thread handle
		_alfFreeThreadHandle((AlfThread*)old->data);

		// Free old node
		ALF_THREAD_FREE(old);
	}

	// Deallocate tls handles
	alfReturnTLS(gData.handleTLS);

	// Delete mutex
	alfDeleteMutex(gData.mutex);
}

// -------------------------------------------------------------------------- //

inline AlfThread* alfCreateThread(PFN_ThreadFunction function, void* argument)
{
	return alfCreateThreadNamed(function, argument, NULL);
}

// -------------------------------------------------------------------------- //

inline AlfThread* alfCreateThreadNamed(PFN_ThreadFunction function, void* argument, char* name)
{
	// Allocate thread object
	AlfThread* thread = ALF_THREAD_ALLOC(sizeof(AlfThread));
	if (!thread) { return NULL; }

	// Clear thread handle
	memset(thread, 0, sizeof(AlfThread));

	// Setup thread data
	AlfThreadData data;
	data.function = function;
	data.argument = argument;
	data.name = name;
	data.handle = thread;
	data.semaphore = alfCreateSemaphore(0);
	data.success = ALF_TRUE;

	// Check if the semaphore is valid
	if (!data.semaphore)
	{
		ALF_THREAD_FREE(thread);
		return NULL;
	}

#if defined(ALF_THREAD_TARGET_WINDOWS)
	// Start the thread
	uint32_t id;
	const HANDLE handle = (HANDLE)_beginthreadex(
		NULL,
		0,
		_alfWin32ThreadStart,
		(void*)&data,
		0,
		&id
	);

	// Check if handle is valid
	if (handle == INVALID_HANDLE_VALUE)
	{
		// Free thread object and return
		ALF_THREAD_FREE(thread);
		return NULL;
	}
#elif defined(ALF_THREAD_PTHREAD)
	// Start thread
	pthread_t handle;
	const int result = pthread_create(
		&handle,
		NULL,
		_alfPthreadThreadStart,
		(void*)&data
	);

	// Check result
	if (result != 0)
	{
		// Free thread object and return
		ALF_THREAD_FREE(thread);
		return NULL;
	}

	// Get id
	uint32_t id = (uint32_t)(u_int64_t)handle;
#endif

	// Wait for and then destroy semaphore
	alfAcquireSemaphore(data.semaphore);
	alfDeleteSemaphore(data.semaphore);

	// Assign handle and id
	thread->handle = handle;
	thread->id = id;

	// Setup other thread properties
	thread->detached = ALF_FALSE;

	// Check for errors during thread initialization
	if (!data.success)
	{
		// Join the thread
		alfJoinThread(thread);
	}

	// Return the thread
	return thread;
}

// -------------------------------------------------------------------------- //

inline AlfThread* alfThisThread()
{
	// Get thread
	AlfThread* thread = (AlfThread*)alfLoadTLS(gData.handleTLS);

	// Check if handle is valid
	if (!thread)
	{
		// Allocate a new handle
		thread = ALF_THREAD_ALLOC(sizeof(AlfThread));
		if (!thread) { return NULL; }

		// Set thread-handle data
#if defined(ALF_THREAD_TARGET_WINDOWS)
		thread->handle = (HANDLE)GetCurrentThread();
		thread->id = (uint32_t)GetCurrentThreadId();
		thread->name = NULL;
		thread->detached = ALF_FALSE;
#elif defined(ALF_THREAD_PTHREAD)
		thread->handle = pthread_self();
		thread->id = (uint32_t)(uint64_t)thread->handle;
		thread->name = NULL;
		thread->detached = ALF_FALSE;
#endif

		// Setup thread handle
		alfStoreTLS(gData.handleTLS, thread);

		// Add external thread handle
		_alfAddExternalThread(thread);
	}

	// Return the thread object
	return thread;
}

// -------------------------------------------------------------------------- //

inline uint32_t alfJoinThread(AlfThread* thread)
{
	// Exit code
	uint32_t exitCode;

#if defined(ALF_THREAD_TARGET_WINDOWS)
	// Wait for thread
	WaitForSingleObject(thread->handle, INFINITE);
	DWORD _exitCode;
	GetExitCodeThread(thread->handle, &_exitCode);
	exitCode = _exitCode;

	// Close handle to the thread
	CloseHandle(thread->handle);
#elif defined(ALF_THREAD_PTHREAD)
	// Join thread
	void* result;
	pthread_join(thread->handle, &result);
	exitCode = (uint32_t)((uint64_t)result);
#endif

	// Free the thread handle
	_alfFreeThreadHandle(thread);

	// Return exit code
	return exitCode;
}

// -------------------------------------------------------------------------- //

inline AlfBool alfJoinThreadTry(AlfThread* thread, uint32_t* exitCodeOut)
{
	// Exit code
	uint32_t exitCode;

#if defined(ALF_THREAD_TARGET_WINDOWS)
	// Wait for thread
	const DWORD result = WaitForSingleObject(thread->handle, 0);
	ALF_THREAD_ASSERT(result != WAIT_FAILED && result != WAIT_ABANDONED, "Failed to try to join thread");
	if (result == WAIT_TIMEOUT)
	{
		*exitCodeOut = 0;
		return ALF_FALSE;
	}
	DWORD _exitCode;
	GetExitCodeThread(thread->handle, &_exitCode);
	exitCode = _exitCode;
	CloseHandle(thread->handle);
#elif defined(ALF_THREAD_PTHREAD)
	// Join thread
	void* result;
	int result = pthread_tryjoin_np(thread->handle, &result);
	if (result == EBUSY)
	{
		*exitCodeOut = 0;
		return ALF_FALSE;
	}
	exitCode = (uint32_t)((uint64_t)result);
#endif

	// Free the thread handle
	_alfFreeThreadHandle(thread);

	// Return exit code
	*exitCodeOut = exitCode;
	return ALF_TRUE;
}

// -------------------------------------------------------------------------- //

inline AlfThreadResult alfDetachThread(AlfThread* thread)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	// Close handle to detach the thread
	const BOOL result = CloseHandle(thread->handle);
	if (!result)
	{
		return ALF_THREAD_UNKNOWN_ERROR;
	}
#elif defined(ALF_THREAD_PTHREAD)
	// Detach thread
	int result = pthread_detach(thread->handle);
	if (result != 0)
	{
		return ALF_THREAD_UNKNOWN_ERROR;
	}
#endif

	// Set detach status
	thread->detached = ALF_TRUE;

	// Return success
	return ALF_THREAD_SUCCESS;
}

// -------------------------------------------------------------------------- //

inline void alfKillThread(AlfThread* thread)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	const BOOL result = TerminateThread(thread->handle, 0);
	ALF_THREAD_ASSERT(result != 0, "Failed to kill thread");
#elif defined(ALF_THREAD_PTHREAD)
	int result = pthread_kill(thread->handle, 0);
	ALF_THREAD_ASSERT(result == 0, "Failed to kill thread");
#endif
}

// -------------------------------------------------------------------------- //

inline void alfExitThread(uint32_t exitCode)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	// Cleanup thread if detached
	AlfThread* thread = alfThisThread();
	if (thread->detached)
	{
		_alfFreeThreadHandle(thread);
	}

	// Exit thread
	_endthreadex(exitCode);
#elif defined(ALF_THREAD_PTHREAD)
	pthread_exit((void*)((uint64_t)exitCode));
#endif
}

// -------------------------------------------------------------------------- //

inline void alfYieldThread()
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	SwitchToThread();
#elif defined(ALF_THREAD_PTHREAD)
	int result = sched_yield();
	ALF_THREAD_ASSERT(result == 0, "Failed to yield thread");
#endif
}

// -------------------------------------------------------------------------- //

inline AlfThreadResult alfSetThreadPriority(AlfThread* thread, AlfThreadPriority priority)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	// Determine priority
	int32_t prio = THREAD_PRIORITY_NORMAL;
	switch (priority)
	{
		case ALF_THREAD_PRIORITY_LOWEST: { prio = THREAD_PRIORITY_LOWEST; break; }
		case ALF_THREAD_PRIORITY_LOW: { prio = THREAD_PRIORITY_BELOW_NORMAL; break; }
		case ALF_THREAD_PRIORITY_HIGH: { prio = THREAD_PRIORITY_ABOVE_NORMAL; break; }
		case ALF_THREAD_PRIORITY_HIGHEST: { prio = THREAD_PRIORITY_HIGHEST; break; }
		case ALF_THREAD_PRIORITY_CRITICAL: { prio = THREAD_PRIORITY_TIME_CRITICAL; break; }
		case ALF_THREAD_PRIORITY_NORMAL: { break; }
		default: { return ALF_THREAD_INVALID_ARGUMENTS; }
	}

	// Set priority
	const BOOL result = SetThreadPriority(
		thread->handle,
		prio
	);
	if (!result)
	{
		return ALF_THREAD_UNKNOWN_ERROR;
	}
#elif defined(ALF_THREAD_PTHREAD)
	// Determine priority
	int prio = 0;
	switch (priority)
	{
		case ALF_THREAD_PRIORITY_LOWEST: { prio = -2; break; }
		case ALF_THREAD_PRIORITY_LOW: { prio = -1; break; }
		case ALF_THREAD_PRIORITY_HIGH: { prio = 1; break; }
		case ALF_THREAD_PRIORITY_HIGHEST: { prio = 2; break; }
		case ALF_THREAD_PRIORITY_CRITICAL: { prio = 3; break; }
		default: break;
	}
	int result = pthread_setschedprio(thread->handle, prio);
	switch (result)
	{
		case 0: break;
		case EINVAL: return ALF_THREAD_INVALID_ARGUMENTS;
		case EPERM: return ALF_THRAD_ACCESS_DENIED;
		case ESRCH: return ALF_THREAD_INVALID_HANDLE;
		default: return ALF_THREAD_UNKNOWN_ERROR; 
	}
#endif

	// Return success
	return ALF_THREAD_SUCCESS;
}

// -------------------------------------------------------------------------- //

inline void alfSleepThread(uint64_t milliseconds)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	Sleep((DWORD)milliseconds);
#elif defined(ALF_THREAD_PTHREAD)
	struct timespec time;
	_alfMillisecondsToTimespec(milliseconds, &time);
	nanosleep(&time, NULL);
#endif
}

// -------------------------------------------------------------------------- //

inline uint32_t alfGetThreadID(AlfThread* thread)
{
	return thread->id;
}

// -------------------------------------------------------------------------- //

inline const char* alfGetThreadName()
{
	// Get thread handle
	AlfThread* thread = alfThisThread();

	// Return 'Unknown' for threads with no set name
	if (!thread->name)
	{
		alfSetThreadName(ALF_DEFAULT_THREAD_NAME);
	}

	// Return the thread name
	return thread->name;
}

// -------------------------------------------------------------------------- //

inline AlfThreadResult alfSetThreadName(const char* name)
{
	// If name is NULL then set name to Unknown
	if (!name)
	{
		return alfSetThreadName(ALF_DEFAULT_THREAD_NAME);
	}

	// Get thread handle
	AlfThread* thread = alfThisThread();

	// Free old name
	if (thread->name)
	{
		ALF_THREAD_FREE(thread->name);
		thread->name = NULL;
	}

	// Copy name into handle if not NULL
	const size_t length = strlen(name);
	thread->name = (char*)ALF_THREAD_ALLOC(length + 1);
	memcpy(thread->name, name, length + 1);

#if defined(ALF_THREAD_TARGET_WINDOWS)
	// Get the address to 'SetThreadDescription'.
	const PFN_SetThreadDescription pSetThreadDescription = (PFN_SetThreadDescription)GetProcAddress(GetModuleHandleW(L"Kernel32.dll"), "SetThreadDescription");

	// Use the function if available
	if (pSetThreadDescription)
	{
		// Convert name to utf16
		wchar_t* _name = _alfUTF8ToUTF16(name);

		// Set the name of the thread
		const HRESULT result = pSetThreadDescription(GetCurrentThread(), _name);
		if (!SUCCEEDED(result))
		{
			return ALF_THREAD_UNKNOWN_ERROR;
		}

		// Free the converted string
		ALF_THREAD_FREE(_name);
	}
	else
	{
		// TODO(Filip Björklund): Implement using exceptions
	}
#elif defined(ALF_THREAD_PTHREAD)
	// Create temp name if longer than 15 characters
	if (length > 15)
	{
		// Use temp name
		char temp_name[16];
		temp_name[15] = 0;
		memcpy(temp_name, name, 15);
		// TODO(Filip Björklund): Don't cut UTF-8 codepoints in half!

		// Set the thread name
		pthread_setname_np(temp_name);
	}
	else
	{
		// Set the thread name
		pthread_setname_np(name);
	}
#endif

	// Return success
	return ALF_THREAD_SUCCESS;
}

// ========================================================================== //
// Semaphore functions
// ========================================================================== //

inline AlfSemaphore* alfCreateSemaphore(uint64_t initialValue)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	// Create the semaphore handle
	const HANDLE handle = CreateSemaphoreExW(
		NULL,
		(LONG)initialValue,
		(LONG)LONG_MAX,
		NULL,
		0,
		SYNCHRONIZE | SEMAPHORE_MODIFY_STATE
	);
	if (!handle)
	{
		return NULL;
	}
#elif defined(ALF_THREAD_TARGET_LINUX)
	ALF_THREAD_ASSERT(initialValue < SEM_VALUE_MAX, "Initial semaphore value exceeds SEM_VALUE_MAX for pthread implementation");
	sem_t handle;
	const int result = sem_init(&handle, 0, (unsigned int)initialValue);
	if (result != 0)
	{
		return NULL;
	}
#elif defined(ALF_THREAD_TARGET_MACOS)
	dispatch_semaphore_t handle = dispatch_semaphore_create(initialValue);
#endif

	// Allocate the semaphore
	AlfSemaphore* semaphore = ALF_THREAD_ALLOC(sizeof(AlfSemaphore));
	semaphore->handle = handle;

	// Return the semaphore
	return semaphore;
}

// -------------------------------------------------------------------------- //

inline void alfDeleteSemaphore(AlfSemaphore* semaphore)
{
	// Return immediately if the handle is NULL
	if (!semaphore) { return; }

	// Delete semaphore
#if defined(ALF_THREAD_TARGET_WINDOWS)
	CloseHandle(semaphore->handle);
#elif defined(ALF_THREAD_TARGET_LINUX)
	sem_destroy(&semaphore->handle);
#elif defined(ALF_THREAD_TARGET_MACOS)
	dispatch_release(semaphore->handle);
#endif

	// Free the semaphore object
	ALF_THREAD_FREE(semaphore);
}

// -------------------------------------------------------------------------- //

inline AlfThreadResult alfAcquireSemaphore(AlfSemaphore* semaphore)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	// Wait for semaphore
	const DWORD result = WaitForSingleObjectEx(semaphore->handle, INFINITE, FALSE);
	switch (result)
	{
		case WAIT_OBJECT_0: break;
		case WAIT_TIMEOUT: return ALF_THREAD_TIMEOUT;
		default: return ALF_THREAD_UNKNOWN_ERROR;
	}
#elif defined(ALF_THREAD_TARGET_LINUX)
	// Wait for semaphore
	const int result = sem_wait(&semaphore->handle);
	if (result == -1)
	{
		// Check result
		int error = errno;
		switch (error)
		{
			case EINVAL: return ALF_THREAD_INVALID_HANDLE;
			case ETIMEDOUT: return ALF_THREAD_TIMEOUT;
			default: return ALF_THREAD_UNKNOWN_ERROR;
		}
	}
#elif defined(ALF_THREAD_TARGET_MACOS)
	long result = dispatch_semaphore_wait(semaphore->handle, DISPATCH_TIME_FOREVER);
	if (result != 0)
	{
		return ALF_THREAD_TIMEOUT;
	}
#endif

	// Return success
	return ALF_THREAD_SUCCESS;
}

// -------------------------------------------------------------------------- //

inline AlfThreadResult alfAcquireSemaphoreTimed(AlfSemaphore* semaphore, uint64_t milliseconds)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	// Wait for semaphore
	const DWORD result = WaitForSingleObjectEx(semaphore->handle, (DWORD)milliseconds, FALSE);
	switch (result)
	{
		case WAIT_OBJECT_0: break;
		case WAIT_TIMEOUT: return ALF_THREAD_TIMEOUT;
		default: return ALF_THREAD_UNKNOWN_ERROR;
	}

#elif defined(ALF_THREAD_TARGET_LINUX)
	// Wait for semaphore
	int result;
	if (milliseconds == CHIF_IMMEDIATE)
	{
		result = sem_trywait(&semphore->handle);
	}
	else
	{
		struct timespec timeout;
		_alfMillisecondsToTimespec(milliseconds, &timeout);
		result = sem_timedwait(&semaphore->handle, &timeout);
	}

	// Check specific error if result is -1
	if (result == -1)
	{
		// Check result
		int error = errno;
		switch (error)
		{
			case EINVAL: return ALF_THREAD_INVALID_HANDLE;
			case ETIMEDOUT: return ALF_THREAD_TIMEOUT;
			default: return ALF_THREAD_UNKNOWN_ERROR,
		}
	}
#elif defined(ALF_THREAD_TARGET_MACOS)
	// Create dispatch_time_t structure from milliseconds
	dispatch_time_t timeout = dispatch_time(DISPATCH_TIME_NOW, milliseconds * 1000000);

	// Wait for the semaphore either until released or timeout
	long result = dispatch_semaphore_wait(semaphore->handle, timeout);
	if (result != 0)
	{
		return CHIF_CONCURRENT_TIMEOUT;
	}
#endif

	// Return success
	return ALF_THREAD_SUCCESS;
}

// -------------------------------------------------------------------------- //

inline AlfBool alfAcquireSemaphoreTry(AlfSemaphore* semaphore)
{
	// Do a timed acquire with immediate timeout
	return alfAcquireSemaphoreTimed(semaphore, ALF_THREAD_IMMEDIATELY) == ALF_THREAD_SUCCESS;
}

// -------------------------------------------------------------------------- //

inline AlfThreadResult alfReleaseSemaphore(AlfSemaphore* semaphore)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	// Release semaphore
	const BOOL result = ReleaseSemaphore(semaphore->handle, (LONG)1, NULL);
	if (result == 0)
	{
		// Switch on error codes
		const DWORD error = GetLastError();
		switch (error)
		{
			default: return ALF_THREAD_UNKNOWN_ERROR;
		}
	}
#elif defined(ALF_THREAD_TARGET_LINUX)
	// Post semaphore
	const int result = sem_post(&semaphore->handle);
	ALF_THREAD_ASSERT(result != EOVERFLOW, "Maximum value for semaphore would be exceeded if it's released");
	switch (result)
	{
		case 0: break;
		case EINVAL: return ALF_THREAD_INVALID_HANDLE;
		default: return ALF_THREAD_UNKNOWN_ERROR;
	}
#elif defined(ALF_THREAD_TARGET_MACOS)
	dispatch_semaphore_signal(semaphore->handle);
#endif

	// Return success
	return ALF_THREAD_SUCCESS;
}

// ========================================================================== //
// Mutex functions
// ========================================================================== //

inline AlfMutex* alfCreateMutex(AlfBool recursive)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	tag_AlfMutex handle;
	if (recursive) { InitializeCriticalSection(&handle.criticalSection); }
	else { InitializeSRWLock(&handle.srwlock); }
#elif defined(ALF_THREAD_PTHREAD)
	tag_AlfMutex handle;

	// Initialize mutex attributes
	int result = pthread_mutexattr_init(&handle.attr);
	if (result != 0) { return NULL; }

	// Set attributes
	pthread_mutexattr_settype(
		&handle.attr,
		recursive ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_ERRORCHECK
	);

	// Initialize mutex
	result = pthread_mutex_init(&handle.mutex, &handle.attr);
	ALF_THREAD_ASSERT(result != EINVAL, "Invalid pthread mutex attribute specified during creation")
	if (result != 0) { return NULL; }
#endif

	// Allocate mutex object
	AlfMutex* mutex = ALF_THREAD_ALLOC(sizeof(AlfMutex));
	handle.recursive = recursive;
	*mutex = handle;

	// Return the mutex
	return mutex;
}

// -------------------------------------------------------------------------- //

inline AlfThreadResult alfDeleteMutex(AlfMutex* mutex)
{
	// Return immediately if the handle is NULL
	if (!mutex) { return ALF_THREAD_SUCCESS; }

#if defined(ALF_THREAD_TARGET_WINDOWS)
	if (mutex->recursive)
	{
		DeleteCriticalSection(&mutex->criticalSection);
	}
#elif defined(ALF_THREAD_PTHREAD)
	// Delete handle
	int result = pthread_mutex_destroy(&mutex->handle.mutex);
	switch (result)
	{
		case 0: { break; }
		case EINVAL: { return ALF_THREAD_INVALID_HANDLE; }
		case EBUSY: { return ALF_THREAD_IN_USE; }
		default: { return ALF_THREAD_UNKNOWN_ERROR; }
	}

	// Delete attributes
	result = pthread_mutexattr_destroy(&mutex->handle.attr);
	ALF_THREAD_ASSERT(result != EINVAL, "Invalid pthread mutex attribute object");
#endif

	// Free the mutex object
	ALF_THREAD_FREE(mutex);

	// Return success
	return ALF_THREAD_SUCCESS;
}

// -------------------------------------------------------------------------- //

inline AlfThreadResult alfAcquireMutex(AlfMutex* mutex)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	if (mutex->recursive) { EnterCriticalSection(&mutex->criticalSection); }
	else { AcquireSRWLockExclusive(&mutex->srwlock); }
#elif defined(ALF_THREAD_PTHREAD)
	// Lock mutex
	int result;
	do
	{
		result = pthread_mutex_lock(&mutex->handle.mutex);
	} while (result == EBUSY);

	// Check result
	ALF_THREAD_ASSERT(result != EAGAIN, "Recursive mutex is locked too many times")
	switch (result)
	{
		case 0: { break; }
		case EINVAL: { return ALF_THREAD_INVALID_HANDLE; }
		case EDEADLK: { return ALF_THREAD_DEADLOCK; }
		default: { return ALF_THREAD_UNKNOWN_ERROR; }
	}
#endif

	// Return success
	return ALF_THREAD_SUCCESS;
}

// -------------------------------------------------------------------------- //

inline AlfBool alfAcquireMutexTry(AlfMutex* mutex)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	if (mutex->recursive)
	{
		const BOOL result = TryEnterCriticalSection(&mutex->criticalSection);
		return result != 0;
	}
	const BOOL result = TryAcquireSRWLockExclusive(&mutex->srwlock);
	return result != 0;
#elif defined(ALF_THREAD_PTHREAD)
	int result = pthread_mutex_trylock(&mutex->handle.mutex);
	return result == 0;
#endif
}

// -------------------------------------------------------------------------- //

inline AlfThreadResult alfReleaseMutex(AlfMutex* mutex)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	if (mutex->recursive) { LeaveCriticalSection(&mutex->criticalSection); }
	else { ReleaseSRWLockExclusive(&mutex->srwlock); }
#elif defined(ALF_THREAD_PTHREAD)
	// Unlock mutex
	int result = pthread_mutex_unlock(&mutex->handle.mutex);
	switch (result)
	{
		case 0: { break; }
		case EINVAL: { return CHIF_CONCURRENT_INVALID_HANDLE; }
		case EBUSY: { return CHIF_CONCURRENT_ALREADY_LOCKED; }
		default: { return CHIF_CONCURRENT_RESULT_UNKNOWN; }
	}
#endif

	// Return success
	return ALF_THREAD_SUCCESS;
}

// ========================================================================== //
// Condition variable functions
// ========================================================================== //

inline AlfConditionVariable* alfCreateConditionVariable()
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	// Initialize condition variable
	CONDITION_VARIABLE handle;
	InitializeConditionVariable(&handle);
#elif defined(ALF_THREAD_PTHREAD)
	// Initialize condition variable
	pthread_cond_t handle;
	int result = pthread_cond_init(&handle, NULL);
	if (result != 0)
	{
		return NULL;
	}
#endif

	// Allocate condition variable handle
	AlfConditionVariable* conditionVariable = ALF_THREAD_ALLOC(sizeof(AlfConditionVariable));
	conditionVariable->handle = handle;
	return conditionVariable;
}

// -------------------------------------------------------------------------- //

inline AlfThreadResult alfDeleteConditionVariable(AlfConditionVariable* conditionVariable)
{
	// Return immediately if the handle is NULL
	if (!conditionVariable) { return ALF_THREAD_SUCCESS; }

#if defined(ALF_THREAD_PTHREAD)
	// Delete condition variable
	int result = pthread_cond_destroy(&condition_variable->handle);
	switch (result)
	{
		case 0: { break; }
		case EINVAL: { return ALF_THREAD_INVALID_HANDLE; }
		case EBUSY: { return ALF_THREAD_IN_USE; }
		default: { return ALF_THREAD_UNKNOWN_ERROR; }
	}
#endif

	// Free condition variable object
	ALF_THREAD_FREE(conditionVariable);

	// Return success
	return ALF_THREAD_SUCCESS;
}

// -------------------------------------------------------------------------- //

inline AlfThreadResult alfWaitConditionVariable(AlfConditionVariable* conditionVariable, AlfMutex* mutex)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	// Sleep condition variable
	BOOL result;
	if (mutex->recursive) { result = SleepConditionVariableCS(&conditionVariable->handle, &mutex->criticalSection, INFINITE); }
	else { result = SleepConditionVariableSRW(&conditionVariable->handle, &mutex->srwlock, INFINITE, 0); }

	// Check result
	if (result == 0)
	{
		// Switch on error
		const DWORD error = GetLastError();
		switch (error)
		{
			case ERROR_TIMEOUT: { return ALF_THREAD_TIMEOUT; }
			default: { return ALF_THREAD_UNKNOWN_ERROR; }
		}
	}
#elif defined(ALF_THREAD_PTHREAD)
	// Wait for condition variable
	int result = pthread_cond_wait(&condition_variable->handle, &mutex->handle.mutex);
	switch (result)
	{
		case EINVAL: { return ALF_THREAD_INVALID_HANDLE; }
		case EPERM: { return ALF_THREAD_NOT_OWNER; }
	}
#endif

	// Return success
	return ALF_THREAD_SUCCESS;
}

// -------------------------------------------------------------------------- //

inline void alfWaitConditionVariablePredicate(AlfConditionVariable* conditionVariable, AlfMutex* mutex, PFN_AlfPredicate predicate, void* predicateArgument)
{
	while (!predicate(predicateArgument))
	{
		alfWaitConditionVariable(conditionVariable, mutex);
	}
}

// -------------------------------------------------------------------------- //

inline void alfNotifyConditionVariable(AlfConditionVariable* conditionVariable)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	// Wake one condition variable
	WakeConditionVariable(&conditionVariable->handle);
#elif defined(ALF_THREAD_PTHREAD)
	// Signal one condition variable
	int result = pthread_cond_signal(&condition_variable->handle);
	CHIF_ASSERT(result != EINVAL, "Invalid condition variable object");
#endif
}

// -------------------------------------------------------------------------- //

inline void alfNotifyAllConditionVariables(AlfConditionVariable* conditionVariable)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
		// Wake all condition variables
		WakeAllConditionVariable(&conditionVariable->handle);
#elif defined(ALF_THREAD_PTHREAD)
		// Broadcast to all condition variables
		int result = pthread_cond_broadcast(&condition_variable->handle);
		ALF_THREAD_ASSERT(result != EINVAL, "Invalid condition variable object");
#endif
}

// ========================================================================== //
// Read/Write lock functions
// ========================================================================== //



// ========================================================================== //
// TLS functions
// ========================================================================== //

inline AlfTLSHandle* alfGetTLS()
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	// Allocate tls
	const DWORD tls = TlsAlloc();
	if (tls == TLS_OUT_OF_INDEXES)
	{
		return NULL;
	}
#elif defined(ALF_THREAD_PTHREAD)
	// Create tls key
	pthread_key_t tls;
	int result = pthread_key_create(&tls, NULL);
	if (result != 0)
	{
		return NULL;
	}
#endif

	// Allocate handle
	AlfTLSHandle* handle = ALF_THREAD_ALLOC(sizeof(AlfTLSHandle));
	if (!handle) { return NULL; }
	handle->handle = tls;
	return handle;
}

// -------------------------------------------------------------------------- //

inline void alfReturnTLS(AlfTLSHandle* handle)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	const BOOL result = TlsFree(handle->handle);
	ALF_THREAD_ASSERT(result != 0, "Failed to return TLS handle");
#elif defined(ALF_THREAD_PTHREAD)
	int result = pthread_key_delete(handle->handle);
	ALF_THREAD_ASSERT(result == 0, "Failed to return TLS handle");
#endif

	// Free handle
	ALF_THREAD_FREE(handle);
}

// -------------------------------------------------------------------------- //

inline void alfStoreTLS(AlfTLSHandle* handle, void* data)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	const BOOL result = TlsSetValue(handle->handle, data);
	ALF_THREAD_ASSERT(result != 0, "Failed to store data in TLS");
#elif defined(ALF_THREAD_PTHREAD)
	pthread_setspecific(tls.handle, data);
#endif
}

// -------------------------------------------------------------------------- //

inline void* alfLoadTLS(AlfTLSHandle* handle)
{
#if defined(ALF_THREAD_TARGET_WINDOWS)
	return TlsGetValue(handle->handle);
#elif defined(ALF_THREAD_PTHREAD)
	return pthread_getspecific(handle->handle);
#endif
}

// ========================================================================== //
// End of implementation
// ========================================================================== //

#endif // defined(ALF_THREAD_IMPL

// ========================================================================== //
// End of header
// ========================================================================== //

#if defined(__cplusplus)
}
#endif

#endif // ALF_THREAD_H