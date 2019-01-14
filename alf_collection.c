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

#include "alf_collection.h"

// ========================================================================== //
// Header Includes
// ========================================================================== //

// Standard headers
#include <string.h>
#include <memory.h>

// ========================================================================== //
// Macro Declarations
// ========================================================================== //

/** Default list and array-list capacity **/
#define ALF_LIST_DEFAULT_CAPACITY 10

// -------------------------------------------------------------------------- //

/** Macro that returns minimum of two number **/
#define ALF_COLLECTION_MIN(a, b) a < b ? a : b

// ========================================================================== //
// Private Functions
// ========================================================================== //

/** Default destructor function **/
void alfDefaultDestructor(void* object) { }

// -------------------------------------------------------------------------- //

/** Default cleaner function **/
void alfDefaultCleaner(const void* object) { }

// ========================================================================== //
// List Structures
// ========================================================================== //

typedef struct tag_AlfList
{
	/** Capacity **/
	uint64_t capacity;
	/** Size **/
	uint64_t size;

	/** Data buffer **/
	void** buffer;

	/** Destructor **/
	PFN_AlfCollectionDestructor destructor;
} tag_AlfList;

// ========================================================================== //
// List Functions
// ========================================================================== //

AlfList* alfCreateList(const AlfListDesc* desc)
{
	AlfList* list = ALF_COLLECTION_ALLOC(sizeof(AlfList));
	if (!list) { return NULL; }

	list->capacity =
		desc->capacity ? desc->capacity : ALF_LIST_DEFAULT_CAPACITY;
	list->size = 0;
	list->destructor = 
		desc->destructor ? desc->destructor : alfDefaultDestructor;

	list->buffer = 
		(void**)ALF_COLLECTION_ALLOC(sizeof(void*) * list->capacity);
	if (!list->buffer)
	{
		ALF_COLLECTION_FREE(list);
		return NULL;
	}

	return list;
}

// -------------------------------------------------------------------------- //

AlfList* alfCreateListSimple()
{
	AlfListDesc desc = { 0 };
	return alfCreateList(&desc);
}

// -------------------------------------------------------------------------- //

void alfDestroyList(AlfList* list)
{
	for (uint64_t i = 0; i < list->size; i++)
	{
		list->destructor(list->buffer[i]);
	}

	ALF_COLLECTION_FREE(list->buffer);
	ALF_COLLECTION_FREE(list);
}

// -------------------------------------------------------------------------- //

void alfListAdd(AlfList* list, void* object)
{
	if (list->size >= list->capacity)
	{
		alfListReserve(list, list->capacity * 2);
	}
	list->buffer[list->size++] = object;
}

// -------------------------------------------------------------------------- //

void alfListPrepend(AlfList* list, void* object)
{
	if (list->size >= list->capacity)
	{
		alfListReserve(list, list->capacity * 2);
	}
	memmove(list->buffer + 1, list->buffer, sizeof(void*) * list->size);
	list->buffer[0] = object;
	list->size++;
}

// -------------------------------------------------------------------------- //

void alfListInsert(AlfList* list, void* object, uint64_t index)
{
	if (index == 0)
	{
		alfListPrepend(list, object);
		return;
	}

	if (index >= list->size)
	{
		alfListAdd(list, object);
	}

	if (list->size >= list->capacity)
	{
		alfListReserve(list, list->capacity * 2);
	}

	memmove(
		list->buffer + index + 1, 
		list->buffer + index, 
		sizeof(void*) * (list->size - index)
	);
	list->buffer[index] = object;
	list->size++;
}

// -------------------------------------------------------------------------- //

void* alfListRemove(AlfList* list, uint64_t index)
{
	if (index >= list->size) { return NULL; }
	void* object = alfListGet(list, index);

	memmove(
		list->buffer + index, 
		list->buffer + index + 1, 
		sizeof(void*) * (list->size - index - 1)
	);
	list->size--;
	return object;
}

// -------------------------------------------------------------------------- //

void* alfListRemoveObject(AlfList* list, const void* object)
{
	for (uint64_t i = 0; i < list->size; i++)
	{
		void* listObject = list->buffer[i];
		if (listObject == object)
		{
			return alfListRemove(list, i);
		}
	}
	return NULL;
}

// -------------------------------------------------------------------------- //

void* alfListRemoveObjectEqual(AlfList* list, const void* object, PFN_AlfCollectionEqual equalsFunction)
{
	for (uint64_t i = 0; i < list->size; i++)
	{
		void* listObject = list->buffer[i];
		if (equalsFunction(listObject, object))
		{
			return alfListRemove(list, i);
		}
	}
	return NULL;
}

// -------------------------------------------------------------------------- //

void* alfListGet(AlfList* list, uint64_t index)
{
	ALF_COLLECTION_ASSERT(
		index >= 0 && index < list->size,
		"Index out of bounds: %u (0 - %u)", 
		index, list->size
	);
	return list->buffer[index];
}

// -------------------------------------------------------------------------- //

void* alfListGetObject(
	AlfList* list, 
	void* object, 
	PFN_AlfCollectionEqual equalsFunction)
{
	for (uint64_t i = 0; i < list->size; i++)
	{
		void* listObject = list->buffer[i];
		if (equalsFunction(listObject, object))
		{
			return alfListGet(list, i);
		}
	}
	return NULL;
}

// -------------------------------------------------------------------------- //

void alfListResize(AlfList* list, uint64_t size)
{
	if (size > list->capacity)
	{
		alfListReserve(list, size);
	}
	list->size = size;
}

// -------------------------------------------------------------------------- //

void alfListReserve(AlfList* list, uint64_t capacity)
{
	if (capacity < list->capacity) { return; }

	void** buffer = (void**)ALF_COLLECTION_ALLOC(
		sizeof(void*) * capacity
	);
	memcpy(buffer, list->buffer, sizeof(void*) * list->capacity);
	ALF_COLLECTION_FREE(list->buffer);
	list->capacity = capacity;
	list->buffer = buffer;
}

// -------------------------------------------------------------------------- //

void alfListShrink(AlfList* list, uint64_t capacity)
{
	if (capacity > list->capacity) { return; }

	void** buffer = (void**)ALF_COLLECTION_ALLOC(
		sizeof(void*) * capacity
	);
	memcpy(buffer, list->buffer, sizeof(void*) * capacity);
	for (uint64_t i = capacity; i < list->size; i++)
	{
		list->destructor(list->buffer[i]);
	}
	ALF_COLLECTION_FREE(list->buffer);
	list->capacity = capacity;
	list->buffer = buffer;
	list->size = ALF_COLLECTION_MIN(list->capacity, list->size);
}

// -------------------------------------------------------------------------- //

void alfListShrinkToFit(AlfList* list)
{
	alfListShrink(list, list->size);
}

// -------------------------------------------------------------------------- //

uint64_t alfGetListSize(AlfList* list)
{
	return list->size;
}

// -------------------------------------------------------------------------- //

void* alfGetListData(AlfList* list)
{
	return list->buffer;
}

// ========================================================================== //
// ArrayList Structures
// ========================================================================== //

typedef struct tag_AlfArrayList
{
	/** Size of each object in list **/
	uint32_t objectSize;
	/** Initial capacity **/
	uint64_t capacity;
	/** List size **/
	uint64_t size;

	/** Buffer **/
	uint8_t* buffer;

	/** Destructor function **/
	PFN_AlfCollectionCleaner cleaner;
} tag_AlfArrayList;

// ========================================================================== //
// ArrayList Private Functions
// ========================================================================== //

AlfBool alfSetupArrayList(AlfArrayList* list, const AlfArrayListDesc* desc)
{
	list->objectSize = desc->objectSize;
	list->capacity =
		desc->capacity ? desc->capacity : ALF_LIST_DEFAULT_CAPACITY;
	list->size = 0;
	list->cleaner = desc->cleaner ? desc->cleaner : alfDefaultCleaner;

	list->buffer = ALF_COLLECTION_ALLOC(list->capacity * list->objectSize);
	if (!list->buffer)
	{
		ALF_COLLECTION_FREE(list);
		return ALF_FALSE;
	}
	return ALF_TRUE;
}

// -------------------------------------------------------------------------- //

void alfCleanupArrayList(AlfArrayList* list)
{
	for (uint64_t i = 0; i < list->size; i++)
	{
		list->cleaner(alfArrayListGet(list, i));
	}
	ALF_COLLECTION_FREE(list->buffer);
}

// ========================================================================== //
// ArrayList Functions
// ========================================================================== //

AlfArrayList* alfCreateArrayList(const AlfArrayListDesc* desc)
{
	ALF_COLLECTION_ASSERT(
		desc->objectSize != 0, 
		"Size of objects in array-list must be greater than zero"
	);

	AlfArrayList* list = ALF_COLLECTION_ALLOC(sizeof(AlfArrayList));
	if (!list) { return NULL; }
	if (!alfSetupArrayList(list, desc))
	{
		return NULL;
	}
	return list;
}

// -------------------------------------------------------------------------- //

AlfArrayList* alfCreateArrayListForObjectSize(uint32_t objectSize, PFN_AlfCollectionCleaner cleaner)
{
	AlfArrayListDesc desc = { 0 };
	desc.objectSize = objectSize;
	desc.cleaner = cleaner;
	desc.capacity = ALF_LIST_DEFAULT_CAPACITY;
	return alfCreateArrayList(&desc);
}

// -------------------------------------------------------------------------- //

void alfDestroyArrayList(AlfArrayList* list)
{
	alfCleanupArrayList(list);
	ALF_COLLECTION_FREE(list);
}

// -------------------------------------------------------------------------- //

void alfArrayListAdd(AlfArrayList* list, const void* object)
{
	if (list->size >= list->capacity)
	{
		alfArrayListReserve(list, list->capacity * 2);
	}

	memcpy(
		list->buffer + (list->size++ * list->objectSize), 
		object, 
		list->objectSize
	);
}

// -------------------------------------------------------------------------- //

void alfArrayListPrepend(AlfArrayList* list, const void* object)
{
	if (list->size >= list->capacity)
	{
		alfArrayListReserve(list, list->capacity * 2);
	}

	memmove(
		list->buffer + list->objectSize, 
		list->buffer, 
		list->size * list->objectSize
	);
	memcpy(list->buffer, object, list->objectSize);
	list->size++;
}

// -------------------------------------------------------------------------- //

void alfArrayListInsert(AlfArrayList* list, const void* object, uint64_t index)
{
	if (index == 0)
	{
		alfArrayListPrepend(list, object);
		return;
	}

	if (index >= list->size)
	{
		alfArrayListAdd(list, object);
	}

	if (list->size >= list->capacity)
	{
		alfArrayListReserve(list, list->capacity * 2);
	}

	memmove(
		list->buffer + (index + 1) * list->objectSize, 
		list->buffer + index * list->objectSize, 
		(list->size - index) * list->objectSize
	);
	memcpy(list->buffer + (index * list->objectSize), object, list->objectSize);
	list->size++;
}

// -------------------------------------------------------------------------- //

void alfArrayListRemove(AlfArrayList* list, uint64_t index)
{
	if (index >= list->size) { return; }

	memmove(
		list->buffer + index * list->objectSize,
		list->buffer + (index + 1) * list->objectSize,
		(list->size - index - 1) * list->objectSize
	);
	list->size--;
}

// -------------------------------------------------------------------------- //

void alfArrayListRemoveGet(AlfArrayList* list, uint64_t index, void* object)
{
	if (index >= list->size) { return; }

	if (object)
	{
		memcpy(
			object, 
			list->buffer + index * list->objectSize, 
			list->objectSize
		);
	}
	memmove(
		list->buffer + index * list->objectSize,
		list->buffer + (index + 1) * list->objectSize,
		(list->size - index - 1) * list->objectSize
	);
	list->size--;
}

// -------------------------------------------------------------------------- //

void alfArrayListRemoveObject(AlfArrayList* list, void* object, PFN_AlfCollectionEqual equalsFunction)
{
	for (uint64_t i = 0; i < list->size; i++)
	{
		void* listObject = alfArrayListGet(list, i);
		if (equalsFunction(listObject, object))
		{
			alfArrayListRemove(list, i);
		}
	}
}

// -------------------------------------------------------------------------- //

void* alfArrayListGet(AlfArrayList* list, uint64_t index)
{
	ALF_COLLECTION_ASSERT(
		index >= 0 && index < list->size, 
		"Index out of bounds: %u (0 - %u)", 
		index, list->size
	);

	return list->buffer + (index * list->objectSize);
}

// -------------------------------------------------------------------------- //

void alfArrayListResize(AlfArrayList* list, uint64_t size)
{
	if (size > list->capacity)
	{
		alfArrayListReserve(list, size);
	}
	list->size = size;
}

// -------------------------------------------------------------------------- //

void alfArrayListReserve(AlfArrayList* list, uint64_t capacity)
{
	if (capacity < list->capacity) { return; }

	uint8_t* buffer = ALF_COLLECTION_ALLOC(capacity * list->objectSize);
	memcpy(buffer, list->buffer, list->size * list->objectSize);
	ALF_COLLECTION_FREE(list->buffer);
	list->capacity = capacity;
	list->buffer = buffer;
}

// -------------------------------------------------------------------------- //

void alfArrayListShrink(AlfArrayList* list, uint64_t capacity)
{
	if (capacity > list->capacity) { return; }

	uint8_t* buffer = ALF_COLLECTION_ALLOC(capacity * list->objectSize);
	memcpy(buffer, list->buffer, capacity * list->objectSize);
	for (uint64_t i = capacity; i < list->size; i++)
	{
		list->cleaner(alfArrayListGet(list, i));
	}
	ALF_COLLECTION_FREE(list->buffer);
	list->capacity = capacity;
	list->buffer = buffer;
	list->size = ALF_COLLECTION_MIN(list->capacity, list->size);
}

// -------------------------------------------------------------------------- //

void alfArrayListShrinkToFit(AlfArrayList* list)
{
	alfArrayListShrink(list, list->size);
}

// -------------------------------------------------------------------------- //

uint64_t alfGetArrayListSize(AlfArrayList* list)
{
	return list->size;
}

// -------------------------------------------------------------------------- //

uint8_t* alfArrayListGetData(AlfArrayList* list)
{
	return list->buffer;
}

// ========================================================================== //
// Stack Structures
// ========================================================================== //

typedef struct tag_AlfStack
{
	/** Capacity **/
	uint32_t capacity;
	/** Stack size**/
	uint32_t size;
	/** Data buffer **/
	uint8_t* buffer;

	/** Object size**/
	uint32_t objectSize;

	/** Object cleaner **/
	PFN_AlfCollectionCleaner objectCleaner;
} tag_AlfStack;

// ========================================================================== //
// Stack Functions
// ========================================================================== //

AlfStack* alfCreateStack(const AlfStackDesc* desc)
{
	ALF_COLLECTION_ASSERT(
		desc->objectSize != 0,
		"Size of objects in stack must be greater than zero"
	);

	AlfStack* stack = ALF_COLLECTION_ALLOC(sizeof(AlfStack));
	if (!stack) { return NULL; }

	stack->capacity = desc->capacity;
	stack->size = 0;
	stack->objectSize = desc->objectSize;
	stack->objectCleaner = desc->objectCleaner;
	stack->buffer = ALF_COLLECTION_ALLOC(stack->objectSize * stack->capacity);
	if (!stack->buffer)
	{
		ALF_COLLECTION_FREE(stack);
		return NULL;
	}

	return stack;
}

// -------------------------------------------------------------------------- //

void alfDestroyStack(AlfStack* stack)
{
	for (uint32_t i = 0; i < stack->size; i++)
	{
		void* object = stack->buffer + (stack->objectSize * i);
		stack->objectCleaner(object);
	}
	ALF_COLLECTION_FREE(stack->buffer);
	ALF_COLLECTION_FREE(stack);
}

// -------------------------------------------------------------------------- //

AlfBool alfStackPush(AlfStack* stack, const void* object)
{
	if (stack->capacity <= stack->size)
	{
		const AlfBool success = alfStackResize(stack, stack->size * 2);
		if (!success) { return ALF_FALSE; }
	}
	memcpy(
		stack->buffer + (stack->objectSize * stack->size++), 
		object, 
		stack->objectSize
	);
	return ALF_TRUE;
}

// -------------------------------------------------------------------------- //

AlfBool alfStackPop(AlfStack* stack, void* objectOut)
{
	if (stack->size < 1) { return ALF_FALSE; }
	memcpy(
		objectOut,
		stack->buffer + (stack->objectSize * --stack->size),
		stack->objectSize
	);
	return ALF_TRUE;
}

// -------------------------------------------------------------------------- //

AlfBool alfStackResize(AlfStack* stack, uint32_t size)
{
	// Allocate block and copy data
	uint8_t* buffer = ALF_COLLECTION_ALLOC(size * stack->objectSize);
	if (!buffer) { return ALF_FALSE; }
	const uint32_t sizeAfter = ALF_COLLECTION_MIN(size, stack->size);
	memcpy(buffer, stack->buffer, sizeAfter * stack->objectSize);

	// Clean object that are after end of old buffer
	const int32_t freeCount = stack->size - sizeAfter;
	for (int32_t i = 0; i < freeCount; i++)
	{
		void* object = stack->buffer + (stack->objectSize * (sizeAfter + i));
		stack->objectCleaner(object);
	}

	stack->size = sizeAfter;
	return ALF_TRUE;
}

// -------------------------------------------------------------------------- //

uint32_t alfStackGetSize(AlfStack* stack)
{
	return stack->size;
}

// ========================================================================== //
// HashTable Structures
// ========================================================================== //

/** A single hash table bucket **/
typedef struct AlfHashTableBucket
{
	/** Cached hash value **/
	uint32_t hash;
	/** Key **/
	void* key;
	/** value data **/
	uint8_t value[];
} AlfHashTableBucket;

// -------------------------------------------------------------------------- //

/** Hash table structure **/
typedef struct tag_AlfHashTable
{
	/** Bucket count **/
	uint32_t bucketCount;
	/** Current entry count**/
	uint32_t size;
	/** buckets **/
	uint8_t* buckets;
	/** Size of a bucket in bytes **/
	uint32_t bucketSize;
	/** Maximum load factor **/
	float maxLoadFactor;

	/** Size of value object in bytes **/
	uint32_t valueSize;

	/** Whether automatic shrinking is enabled **/
	AlfBool automaticShrink;

	/** Hash function  **/
	PFN_AlfCollectionHash hashFunction;
	/** Key equality function **/
	PFN_AlfCollectionEqual keyEqual;
	/** Key copy function **/
	PFN_AlfCollectionCopy keyCopy;
	/** Key destructor **/
	PFN_AlfCollectionDestructor keyDestructor;
	/** Value cleaner **/
	PFN_AlfCollectionCleaner valueCleaner;
} tag_AlfHashTable;

// ========================================================================== //
// HashTable Macro Declarations
// ========================================================================== //

/** Default hash-table bucket count **/
#define ALF_DEFAULT_HASH_TABLE_BUCKET_COUNT (1 << 5)

// -------------------------------------------------------------------------- //

/** Default load factor to trigger resize of hash-table **/
#define ALF_DEFAULT_HASH_TABLE_LOAD_FACTOR_TRIGGER_RESIZE 0.8f

// -------------------------------------------------------------------------- //

/** Macro to check if power of two **/
#define ALF_IS_POWER_OF_TWO(num) (num && !(num & (num - 1)))

// -------------------------------------------------------------------------- //

/** Macro to perform modulo calculation where y is a power of two **/
#define ALF_MOD_POWER_OF_TWO(x, y) ((x) & ((y) - 1))

// ========================================================================== //
// HashTable Private String Functions
// ========================================================================== //

/** String copy **/
void* alfStringCopyFunction(const void* object)
{
	const char* str = (const void*)object;
	const uint64_t size = strlen(str);
	char* buffer = ALF_COLLECTION_ALLOC(size + 1);
	buffer[size] = 0;
	memcpy(buffer, str, size);
	return buffer;
}

// -------------------------------------------------------------------------- //

/** String destructor **/
void alfStringDestructorFunction(void* object)
{
	char* str = (char*)object;
	ALF_COLLECTION_FREE(str);
}

// -------------------------------------------------------------------------- //

/** String comparison **/
AlfBool alfStringEqualFunction(const void* object0, const void* object1)
{
	const char* str0 = (const char*)object0;
	const char* str1 = (const char*)object1;
	return strcmp(str0, str1) == 0;
}

// -------------------------------------------------------------------------- //

/** FNV-32 hash for strings **/
uint32_t alfStringHashFunction(const void* object)
{
	const char* str = (const char*)object;
	uint32_t index = 0;
	uint32_t hash = 0x811c9dc5ul;
	while (str[index])
	{
		hash = hash * 16777619ul;
		hash = hash ^ str[index++];
	}
	return hash;
}

// ========================================================================== //
// HashTable Private Functions
// ========================================================================== //

/** Returns pointer to bucket at specified index in bucket array **/
static AlfHashTableBucket* alfHashTableGetBucketAtIndex(
	uint8_t* buckets,
	uint32_t bucketSize,
	uint32_t index)
{
	return (AlfHashTableBucket*)(buckets + (index * bucketSize));
}

// -------------------------------------------------------------------------- //

/** Creates a hash that is valid for the hash table from the hash function that
 * the user specified at hash table creation. This function makes sure that the
 * MSB are always clear (Signifies tombstones) and that 0 is never returned as a
 * valid hash (Signifies empty bucket). **/
static uint32_t alfHashTableGetKeyHash(AlfHashTable* table, const void* key)
{
	// Clear MSB and make sure hash is never 0 (Signifies empty bucket)
	uint32_t hash = table->hashFunction(key);
	hash &= 0x7FFFFFFF;
	return hash ? hash : 1;
}

// -------------------------------------------------------------------------- //

/** Returns whether or not a hash value represents a tombstone **/
static AlfBool alfHashTableIsTombstone(uint32_t hash)
{
	return (hash & 0x80000000) != 0;
}

// -------------------------------------------------------------------------- //

/** Returns the tombstone value from a hash value **/
static uint32_t alfHashTableMarkTombstone(uint32_t hash)
{
	return hash |= 0x80000000;
}

// -------------------------------------------------------------------------- //

/** Sets the value of a bucket by copying all bytes **/
static void alfHashTableSetBucketValue(
	AlfHashTable* table,
	AlfHashTableBucket* bucket,
	const void* value)
{
	memcpy(
		bucket->value, 
		value, 
		table->valueSize
	);
}

// -------------------------------------------------------------------------- //

/** Setup buckets to match the bucket count. Bucket hashes are cleared to 0 **/
static void alfHashTableSetupBuckets(AlfHashTable* table, uint32_t bucketCount)
{
	// Allocate buckets
	table->bucketCount = bucketCount;
	table->buckets = ALF_COLLECTION_ALLOC(
		table->bucketSize * table->bucketCount);

	// Clear buckets
	for (uint32_t i = 0; i < table->bucketCount; i++)
	{
		AlfHashTableBucket* bucket = alfHashTableGetBucketAtIndex(
			table->buckets, table->bucketSize, i);
		bucket->hash = 0;
	}
}

// -------------------------------------------------------------------------- //

/** Returns the distance from the current index to the index that the hash 
 * value corresponds to. This is used to determine the probing distance **/
static uint32_t alfHashTableDistanceFromWantedIndex(
	AlfHashTable* table,
	uint32_t hash,
	uint32_t currentIndex)
{
	const uint32_t otherIndex = ALF_MOD_POWER_OF_TWO(hash, table->bucketCount);
	return ALF_MOD_POWER_OF_TWO(
		currentIndex + table->bucketCount - otherIndex, 
		table->bucketCount
	);
}

// -------------------------------------------------------------------------- //

/** Insert a key-value pair into a hash table. This is part of the private 
 * implementation and does not check load factor. alfHashTableInsert should be
 * used instead (It uses this function). **/
static AlfBool alfHashTableInsertKeyValue(
	AlfHashTable* table,
	void* key,
	const void* inValue)
{
	// Create local versions of key and value
	void* value = alloca(table->valueSize);
	memcpy(value, inValue, table->valueSize);

	// Find index to place bucket data at
	uint32_t hash = alfHashTableGetKeyHash(table, key);
	uint32_t index = ALF_MOD_POWER_OF_TWO(hash, table->bucketCount);
	int32_t distance = 0;
	while (ALF_TRUE)
	{
		// Retrieve bucket
		AlfHashTableBucket* otherBucket = alfHashTableGetBucketAtIndex(
			table->buckets, table->bucketSize, index);

		// If slot is empty then insert
		if (otherBucket->hash == 0)
		{
			otherBucket->hash = hash;
			otherBucket->key = key;
			alfHashTableSetBucketValue(table, otherBucket, value);
			return ALF_TRUE;
		}

		// If element has probed less then insert
		const int32_t slotDistance = alfHashTableDistanceFromWantedIndex(
			table, otherBucket->hash, index);
		if (slotDistance < distance)
		{
			// If tombstone then replace
			if (alfHashTableIsTombstone(otherBucket->hash))
			{
				otherBucket->hash = hash;
				otherBucket->key = key;
				alfHashTableSetBucketValue(table, otherBucket, value);
				return ALF_TRUE;
			}

			// Store bucket entries temporarily
			const uint32_t _hash = otherBucket->hash;
			void* _key = otherBucket->key;
			void* _value = alloca(table->valueSize);
			memcpy(_value, otherBucket->value, table->valueSize);

			// Insert values into bucket
			otherBucket->hash = hash;
			otherBucket->key = key;
			alfHashTableSetBucketValue(table, otherBucket, value);

			// Retrieve value from temp
			hash = _hash;
			key = _key;
			memcpy(value, _value, table->valueSize);

			// Update distance
			distance = slotDistance;
		}

		// Go to next index and increase probing distance
		index = ALF_MOD_POWER_OF_TWO(index + 1, table->bucketCount);
		distance++;
	}
}

// -------------------------------------------------------------------------- //

/** Returns object corresponding to key or NULL if key was not found. If the
 * key was found then the 'indexOut' paramter is also set to the index **/
void* alfHashTableFindIndex(
	AlfHashTable* table, 
	const void* key, 
	uint32_t* indexOut)
{
	const uint32_t hash = alfHashTableGetKeyHash(table, key);
	uint32_t index = ALF_MOD_POWER_OF_TWO(hash, table->bucketCount);
	int32_t distance = 0;
	while (ALF_TRUE)
	{
		// Retrieve bucket
		AlfHashTableBucket* otherBucket = alfHashTableGetBucketAtIndex(
			table->buckets, table->bucketSize, index);

		// Return NULL if bucket is empty
		if (otherBucket->hash == 0)
		{
			return NULL;
		}

		// Value cannot be further away than what the object at the current
		// position is, therefore return NULL immediately.
		const int32_t slotDistance = alfHashTableDistanceFromWantedIndex(
			table, otherBucket->hash, index);
		if (distance > slotDistance)
		{
			return NULL;
		}

		// Check if object is the same
		if (otherBucket->hash == hash &&
			table->keyEqual(key, otherBucket->key))
		{
			*indexOut = index;
			return otherBucket->value;
		}

		index = ALF_MOD_POWER_OF_TWO(index + 1, table->bucketCount);
		distance++;
	}
}

// ========================================================================== //
// HashTable Functions
// ========================================================================== //

AlfHashTable* alfCreateHashTable(const AlfHashTableDesc* desc)
{
	// Assert the preconditions
	ALF_COLLECTION_ASSERT(
		ALF_IS_POWER_OF_TWO(desc->bucketCount),
		"Bucket count of hash table must be a power of two"
	);

	// Allocate and setup table
	AlfHashTable* table = ALF_COLLECTION_ALLOC(sizeof(AlfHashTable));
	if (!table) { return NULL; }
	table->automaticShrink = ALF_FALSE;
	table->valueSize = desc->valueSize;
	table->hashFunction = desc->hashFunction;
	table->keyEqual = desc->keyEqual;
	table->keyCopy = desc->keyCopy;
	table->keyDestructor = 
		desc->keyDestructor ? desc->keyDestructor : alfDefaultDestructor;
	table->valueCleaner = 
		desc->valueCleaner ? desc->valueCleaner : alfDefaultCleaner;

	// Setup buckets
	table->maxLoadFactor = ALF_DEFAULT_HASH_TABLE_LOAD_FACTOR_TRIGGER_RESIZE;
	table->size = 0;
	table->bucketSize = 
		sizeof(AlfHashTableBucket) + table->valueSize;
	alfHashTableSetupBuckets(table, desc->bucketCount);

	return table;
}

// -------------------------------------------------------------------------- //

AlfHashTable* alfCreateHashTableSimple(
	uint32_t valueSize, 
	PFN_AlfCollectionCleaner valueCleaner)
{
	// Setup descriptor for hash table using strings as keys
	AlfHashTableDesc desc;
	desc.bucketCount = ALF_DEFAULT_HASH_TABLE_BUCKET_COUNT;
	desc.valueSize = valueSize;
	desc.hashFunction = alfStringHashFunction;
	desc.keyEqual = alfStringEqualFunction;
	desc.keyCopy = alfStringCopyFunction;
	desc.keyDestructor = alfStringDestructorFunction;
	desc.valueCleaner = valueCleaner;
	return alfCreateHashTable(&desc);
}

// -------------------------------------------------------------------------- //

void alfDestroyHashTable(AlfHashTable* table)
{
	// Cleanup all remaining buckets
	for (uint32_t i = 0; i < table->bucketCount; i++)
	{
		const AlfHashTableBucket* bucket =
			alfHashTableGetBucketAtIndex(table->buckets, table->bucketSize, i);
		if (bucket->hash != 0 && !alfHashTableIsTombstone(bucket->hash))
		{
			table->keyDestructor(bucket->key);
			table->valueCleaner(bucket->value);
		}
	}

	// Free table
	ALF_COLLECTION_FREE(table->buckets);
	ALF_COLLECTION_FREE(table);
}

// -------------------------------------------------------------------------- //

AlfBool alfHashTableInsert(
	AlfHashTable* table, 
	const void* key, 
	const void* value)
{
	// Resize if load factor exceeds certain value
	const float loadFactor = alfHashTableGetLoadFactor(table);
	if (loadFactor >= table->maxLoadFactor)
	{
		alfHashTableResize(table, table->bucketCount << 1);
	}

	// Insert
	void* keyCopy = table->keyCopy(key);
	const AlfBool success =  
		alfHashTableInsertKeyValue(table, keyCopy, value);
	if (!success)
	{
		table->keyDestructor(keyCopy);
		return ALF_FALSE;
	}
	table->size++;
	return success;
}

// -------------------------------------------------------------------------- //

void* alfHashTableGet(AlfHashTable* table, const void* key)
{
	uint32_t index;
	return alfHashTableFindIndex(table, key, &index);
}

// -------------------------------------------------------------------------- //

AlfBool alfHashTableRemove(AlfHashTable* table, const void* key, void* valueOut)
{
	// Find value and index, return immediately if value was not found
	uint32_t index;
	void* value =  alfHashTableFindIndex(table, key, &index);
	if (!value)
	{
		return ALF_FALSE;
	}

	// Retrieve bucket, reset hash and write value to user buffer
	AlfHashTableBucket* bucket = 
		alfHashTableGetBucketAtIndex(table->buckets, table->bucketSize, index);
	bucket->hash = alfHashTableMarkTombstone(bucket->hash);
	table->keyDestructor(bucket->key);
	if (valueOut) 
	{
		memcpy(valueOut, value, table->valueSize);
	}
	table->size--;
	return ALF_TRUE;
}

// -------------------------------------------------------------------------- //

AlfBool alfHasKey(AlfHashTable* table, const void* key)
{
	uint32_t index;
	return alfHashTableFindIndex(table, key, &index) != NULL;
}

// -------------------------------------------------------------------------- //

void alfHashTableResize(AlfHashTable* table, uint32_t size)
{
	// Assert the preconditions
	ALF_COLLECTION_ASSERT(
		ALF_IS_POWER_OF_TWO(size),
		"Hash table can only be resized to power of two sizes"
	);

	// Store old and setup new
	const uint32_t oldBucketCount = table->bucketCount;
	uint8_t* oldBuckets = table->buckets;
	alfHashTableSetupBuckets(table, size);

	// Copy values from old
	uint32_t moveSizeLeft = table->size;
	for (uint32_t i = 0; i < oldBucketCount && moveSizeLeft > 0; i++)
	{
		AlfHashTableBucket* oldBucket = alfHashTableGetBucketAtIndex(
			oldBuckets, table->bucketSize, i);
		const uint32_t hash = oldBucket->hash;
		if (hash != 0 && !alfHashTableIsTombstone(hash))
		{
			void* key = oldBucket->key;
			void* value = oldBucket->value;
			alfHashTableInsertKeyValue(table, key, value);
			moveSizeLeft--;
		}
	}

	// Cleanup old
	ALF_COLLECTION_FREE(oldBuckets);
}

// -------------------------------------------------------------------------- //

AlfBool alfHashTableIterate(AlfHashTable* table, PFN_AlfHashTableIterate iterateFunction)
{
	uint32_t index = 0;
	for (uint32_t i = 0; i < table->bucketCount; i++)
	{
		// Retrieve bucket
		const AlfHashTableBucket* bucket = 
			alfHashTableGetBucketAtIndex(table->buckets, table->bucketSize, i);

		// Call the iterator function for key-value pair if hash is valid
		if (bucket->hash != 0 && !alfHashTableIsTombstone(bucket->hash))
		{
			const AlfBool cont = iterateFunction(
				table, index, bucket->key, (void*)bucket->value);
			if (!cont) { return ALF_FALSE; }
			index++;
		}
	}
	return ALF_TRUE;
}

// -------------------------------------------------------------------------- //

void alfHashTableSetMaxLoadFactor(AlfHashTable* table, float loadFactor)
{
	table->maxLoadFactor = loadFactor;
}

// -------------------------------------------------------------------------- //

float alfHashTableGetLoadFactor(AlfHashTable* table)
{
	return (float)table->size / (float)table->bucketCount;
}

// -------------------------------------------------------------------------- //

uint64_t alfHashTableGetSize(AlfHashTable* table)
{
	return table->size;
}

// ========================================================================== //
// End of Implementation
// ========================================================================== //
