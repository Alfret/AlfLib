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

#include "alf.h"

// ========================================================================== //
// Header Includes
// ========================================================================== //

// Standard headers
#include <stdlib.h>

// ========================================================================== //
// Allocation Global variables
// ========================================================================== //

/** Current total memory usage **/
static uint64_t gMemoryUsage;

// -------------------------------------------------------------------------- //

/** High-water mark for memory usage **/
static uint64_t gMemoryHighWatermark;

// ========================================================================== //
// Allocation Structures
// ========================================================================== //

/** Allocation header **/
typedef struct AlfAllocHeader
{
	/** Pointer to original allocated block **/
	void* original;
	/** Size of the allocation **/
	uint64_t size;
} AlfAllocHeader;

// ========================================================================== //
// Allocation Functions
// ========================================================================== //

void* alfAlloc(uint64_t size, uint32_t alignment)
{
	// Assert preconditions
	ALF_ASSERT(LN_IS_POWER_OF_TWO(alignment),
		"Allocation alignment must be a power of two, is: %i", alignment);

	// Don't allocate empty block
	if (size == 0) { return ALF_NULL; }

	// Allocate memory
	const uint64_t overhead = alignment + sizeof(AlfAllocHeader) - 1;
	uint8_t* memory = (uint8_t*)malloc(size + overhead);
	if (!memory) { return ALF_NULL; }

	// Setup data
	AlfAllocHeader* userPointer =
		(AlfAllocHeader*)((uintptr_t)(memory + overhead) & ~(alignment - 1ull));
	(&userPointer[-1])->original = memory;
	(&userPointer[-1])->size = size;

	// Update stats
	gMemoryUsage += size;
	if (gMemoryHighWatermark < gMemoryUsage)
	{
		gMemoryHighWatermark = gMemoryUsage;
	}

	// Return allocation
	return userPointer;
}

// -------------------------------------------------------------------------- //

void* alfAllocClear(uint64_t size, uint32_t alignment)
{
	void* memory = alfAlloc(size, alignment);
	if (memory)
	{
		alfClearMemory(memory, size);
	}
	return memory;
}

// -------------------------------------------------------------------------- //

void* alfRealloc(void* memory, uint64_t size, uint32_t alignment)
{
	// Handle special cases
	if (!memory) { return alfAlloc(size, alignment); }
	if (!size) { alfFree(memory); return ALF_NULL; }

	// Allocate new block and move data over
	const uint64_t oldSize = alfGetAllocSize(memory);
	void* block = alfAlloc(size, alignment);
	alfCopyMemory(block, memory, ALF_MIN(oldSize, size));
	alfFree(memory);
	return block;
}

// -------------------------------------------------------------------------- //

void alfFree(void* memory)
{
	// Just ignore NULL pointers
	if (!memory) { return; }

	// Retrieve original allocation
	AlfAllocHeader* userPointer = (AlfAllocHeader*)memory;
	AlfAllocHeader* original = (AlfAllocHeader*)userPointer[-1].original;

	// Update stats
	const uint64_t size = userPointer[-1].size;
	gMemoryUsage -= size;

	// Free allocation
	free(original);
}

// -------------------------------------------------------------------------- //

uint64_t alfGetAllocSize(void* memory)
{
	return ((AlfAllocHeader*)memory)[-1].size;
}

// -------------------------------------------------------------------------- //

uint64_t alfGetTotalMemoryUsage()
{
	return gMemoryUsage;
}

// ========================================================================== //
// Memory Functions
// ========================================================================== //

void alfCopyMemory(void* destination, const void* source, uint64_t size)
{
	
}

// -------------------------------------------------------------------------- //

void alfClearMemory(void* memory, uint64_t size)
{
	
}

// -------------------------------------------------------------------------- //

void alfMoveMemory(void* destination, const void* source, uint64_t size)
{
	
}

// -------------------------------------------------------------------------- //

AlfBool alfEqualMemory(const void* memory0, const void* memory1, uint64_t size)
{
	
}
