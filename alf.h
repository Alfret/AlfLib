// MIT License
//
// Copyright (c) 2018-2019 Filip Björklund
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

#ifndef ALF_H
#define ALF_H

#if defined(__cplusplus)
extern "C" {
#endif

// ========================================================================== //
// Macro Declarations
// ========================================================================== //

#if defined(_WIN32)
#	define ALF_TARGET_WINDOWS
#	if defined(ALF_SHARED_LIB)
#		if defined(ALF_SHARED_LIB_EXPORT)
#			define ALF_API __declspec(dllexport)
#		else
#			define ALF_API __declspec(dllimport)
#		endif
#	else
#		define ALF_API
#	endif
#elif defined(__linux__)
#	define ALF_TARGET_LINUX
#	define ALF_API
#elif defined(__APPLE__)
#	include <TargetConditionals.h>
#	if TARGET_IPHONE_SIMULATOR
#		define ALF_TARGET_IOS_SIMULATOR
#	elif TARGET_OS_IPHONE
#		define ALF_TARGET_IOS
#	elif TARGET_OS_MAC
#		define ALF_TARGET_MACOS
#	else
#		error "Unsupported Apple OS"
#	endif
#	define ALF_API
#endif

// ========================================================================== //
// Header Includes
// ========================================================================== //

// Standard headers
#include <stdint.h>

// ========================================================================== //
// Types and Constants
// ========================================================================== //

/** Boolean type **/
typedef uint32_t AlfBool;

// -------------------------------------------------------------------------- //

/** Boolean value for true (1) **/
#define ALF_TRUE ((AlfBool)1)

// -------------------------------------------------------------------------- //

/** Boolean value for false (0) **/
#define ALF_FALSE ((AlfBool)0)

// -------------------------------------------------------------------------- //

/** NULL value **/
#define ALF_NULL ((void*)0)

// ========================================================================== //
// Allocation Functions
// ========================================================================== //

/** Allocate memory of the specified size and alignment.
 * \note Size can be 0, in which case NULL is returned.
 * \pre Alignment must be a exponent of two.
 * \brief Allocate memory.
 * \param size Size of memory to allocate.
 * \param alignment Alignment to allocate with.
 * \return Allocated memory.
 */
ALF_API void* alfAlloc(uint64_t size, uint32_t alignment);

// -------------------------------------------------------------------------- //

/** Allocate memory of the specified size and alignment. The memory is then
 * cleared to contain all zeros.
 * \note Size can be 0, in which case NULL is returned.
 * \pre Alignment must be a exponent of two.
 * \brief Allocate memory and clear it.
 * \param size Size of memory to allocate.
 * \param alignment Alignment to allocate with.
 * \return Allocated memory.
 */
ALF_API void* alfAllocClear(uint64_t size, uint32_t alignment);

// -------------------------------------------------------------------------- //

/** Reallocate memory with a new size and alignment. The reallocated memory is 
 * guaranteed to contains the exact same data as the memory that was 
 * reallocated.
 * \note Currently the implementation of this function is naïve, it always
 * allocates memory and copies from the old to the new allocation.
 * \brief Reallocate memory.
 * \param memory Memory to reallocate.
 * \param size Size of new memory block.
 * \param alignment Alignment of reallocated memory.
 * \return Reallocated memory.
 */
ALF_API void* alfRealloc(void* memory, uint64_t size, uint32_t alignment);

// -------------------------------------------------------------------------- //

/** Free memory that was previously allocated with the lnAlloc function.
 * \note Memory can be NULL, in which case nothing happens.
 * \brief Free memory.
 * \param memory Memory to free.
 */
ALF_API void alfFree(void* memory);

// -------------------------------------------------------------------------- //

/** Returns the size of an allocation.
 * \brief Returns allocation size.
 * \param memory Allocation to get size of.
 * \return Size of allocation.
 */
ALF_API uint64_t alfGetAllocSize(void* memory);

// -------------------------------------------------------------------------- //

/** Returns the total amount of memory currently allocated by the program.
 * \brief Returns memory usage.
 * \return Memory usage in bytes.
 */
ALF_API uint64_t alfGetTotalMemoryUsage();

// ========================================================================== //
// Memory Functions
// ========================================================================== //

/** Copy memory from the source memory location to the destination.
 * \brief Copy memory.
 * \param destination Destination of copy.
 * \param source Source of copy.
 * \param size Size of memory to copy.
 */
ALF_API void alfCopyMemory(
	void* destination, 
	const void* source, 
	uint64_t size);

// -------------------------------------------------------------------------- //

/** Clear the data at the memory location specified. The size specifies how many
 * bytes to clear from the specified location.
 * \brief Clear memory.
 * \param memory Location to perform clear at.
 * \param size Size of memory to clear, from location.
 */
ALF_API void alfClearMemory(void* memory, uint64_t size);

// -------------------------------------------------------------------------- //

/** Move memory from the source memory location to the destination. The regions
 * may overlap.
 * \brief Move memory.
 * \param destination Destination of move.
 * \param source Source of move.
 * \param size Size of memory to move.
 */
ALF_API void alfMoveMemory(
	void* destination, 
	const void* source, 
	uint64_t size);

// -------------------------------------------------------------------------- //

/** Compare two regions of memory to see if the contain the same data.
 * \brief Compare memory regions for equality.
 * \param memory0 First region.
 * \param memory1 Second region.
 * \param size Size of regions to compare. This size must not be larger than the
 * largest of the two regions.
 * \return True if the contents in the memory at the specified regions are equal
 * else false.
 */
ALF_API AlfBool alfEqualMemory(
	const void* memory0, 
	const void* memory1, 
	uint64_t size);

// ========================================================================== //
// Assert Macro Declarations
// ========================================================================== //

/** **/
#define ALF_ASSERT(condition, format, ...)

// ========================================================================== //
// Assert Functions
// ========================================================================== //



// ========================================================================== //
// Dialog Functions
// ========================================================================== //



// ========================================================================== //
// End of Header
// ========================================================================== //

#if defined(__cplusplus)
}
#endif

#endif // ALF_H