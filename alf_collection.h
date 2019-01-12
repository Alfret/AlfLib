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

#ifndef ALF_COLLECTION_H
#define ALF_COLLECTION_H

#if defined(__cplusplus)
extern "C" {
#endif

// ========================================================================== //
// Header includes
// ========================================================================== //

// Standard headers
#include <stdint.h>

// ========================================================================== //
// Types and Values
// ========================================================================== //

/** Boolean type **/
typedef uint32_t AlfBool;

// -------------------------------------------------------------------------- //

#ifndef ALF_TRUE
	/** Boolean value true **/
#	define ALF_TRUE ((AlfBool)1)
#endif

// -------------------------------------------------------------------------- //

#ifndef ALF_FALSE
	/** Boolean value false **/
#	define ALF_FALSE ((AlfBool)0)
#endif

// -------------------------------------------------------------------------- //

// Allocation macros
#if !defined(ALF_COLLECTION_ALLOC)
#	include <stdlib.h>
	/** Allocation function **/
#	define ALF_COLLECTION_ALLOC(size) malloc(size)
#endif

// -------------------------------------------------------------------------- //

// Allocation macros
#if !defined(ALF_COLLECTION_FREE)
#	include <stdlib.h>
	/** Free the memory of an allocation **/
#	define ALF_COLLECTION_FREE(mem) free(mem)
#endif

// -------------------------------------------------------------------------- //

// Assert macros
#if !defined(ALF_COLLECTION_ASSERT)
#	include <assert.h>
	/** Internal assertion macro **/
#	define ALF_COLLECTION_ASSERT(cond, msg, ...) assert((cond) && msg)
#endif 

// ========================================================================== //
// Type Definitions
// ========================================================================== //

/** Prototype of a function that works as a destructor for objects in a 
 * collection. The calle is responsible for freeing the object and all its data.
 * \param object Object to destroy.
 */
typedef void(*PFN_AlfCollectionDestructor)(void* object);

// -------------------------------------------------------------------------- //

/** Prototype of a function that works as a cleanup function for object in a 
 * collection. The calle must not free the actual object, only cleanup all the
 * data that it owns.
 * \note This is used in place of a destructor when the collection owns the 
 * memory of the object but the user must clean the object.
 * \param object Object to clean.
 */
typedef void(*PFN_AlfCollectionCleaner)(const void* object);

// -------------------------------------------------------------------------- //

/** Prototype of a function to compare objects in a collection for equality.
 * \param object0 First object in comparison.
 * \param object1 Second object in comparison.
 * \return True if the objects are equal otherwise false.
 */
typedef AlfBool(*PFN_AlfCollectionEqual)(const void* object0, const void* object1);

// -------------------------------------------------------------------------- //

/** Prototype of a function to compute the hash for an object.
 * \param object Object to compute hash from.
 */
typedef uint32_t(*PFN_AlfCollectionHash)(const void* object);

// -------------------------------------------------------------------------- //

/** Prototype of a function to create a copy of an object.
 * \param object Object to create copy of.
 */
typedef void*(*PFN_AlfCollectionCopy)(const void* object);

// ========================================================================== //
// Structures
// ========================================================================== //

/** \struct AlfListDesc
 * \author Filip Björklund
 * \date 08 januari 2019 - 23:41
 * \brief List descriptor.
 * \details
 * Structure that represents a descriptor for list creation. This contains 
 * information about the initial capacity of the list as well as a destructor
 * that will be called for any objects that are still in the list when it's 
 * being destroyed.
 * 
 * The initial capacity may be 0, which will set it to the internal default 
 * value.
 * The destructor may be NULL in which case nothing will most likely leak the 
 * memory of the object.
 */
typedef struct AlfListDesc
{
	/** Initial list capacity **/
	uint64_t capacity;
	/** Object destructor **/
	PFN_AlfCollectionDestructor destructor;
} AlfListDesc;

// -------------------------------------------------------------------------- //

/** \struct AlfList
 * \author Filip Björklund
 * \date 08 januari 2019 - 23:38
 * \brief List of pointers.
 * \details
 * Structure that represents a list of pointers to objects. This allows the list
 * to be generic and hold objects that have been created elsewhere.
 */
typedef struct tag_AlfList AlfList;

// ========================================================================== //
// List Functions
// ========================================================================== //

/** Create a list from the given descriptor.
 * \brief Create list.
 * \param desc List descriptor.
 * \return Created list or NULL on failure.
 */
AlfList* alfCreateList(const AlfListDesc* desc);

// -------------------------------------------------------------------------- //

/** Create a list without a specified descriptor. The list will have the default
 * capacity and destructor set.
 * \return Created list or NULL on failure.
 */
AlfList* alfCreateListSimple();

// -------------------------------------------------------------------------- //

/** Destroy a list. This will call the destructor for all remaining items in the
 * list.
 * \brief Destroy list.
 * \param list List to destroy.
 */
void alfDestroyList(AlfList* list);

// -------------------------------------------------------------------------- //

/** Add an object to the end of a list.
 * \brief Add object to end of list.
 * \param list List to add object to.
 * \param object Object to add.
 */
void alfListAdd(AlfList* list, void* object);

// -------------------------------------------------------------------------- //

/** Add an object to the beginning of a list.
 * \brief Add object to start of list.
 * \param list List to add object to.
 * \param object Object to add.
 */
void alfListPrepend(AlfList* list, void* object);

// -------------------------------------------------------------------------- //

/** Add an object to the specified index in a list. If the index is larger than
 * the highest index the object will be added to the end of the list. If the
 * index is 0 the object is added to the beginning of the list.
 * \brief Insert object into list.
 * \param list List to insert object into.
 * \param object Object to insert into list.
 * \param index Index to insert object at.
 */
void alfListInsert(AlfList* list, void* object, uint64_t index);

// -------------------------------------------------------------------------- //

/** Remove an object from a list and return it. If the index is out of bounds
 * the function will return null.
 * \brief Remove and return object in list.
 * \param list List to remove object from.
 * \param index Index to return object at.
 * \return Removed object or null if no object was removed.
 */
void* alfListRemove(AlfList* list, uint64_t index);

// -------------------------------------------------------------------------- //

/** Removes and then returns the first object in the list that matches the
 * object of the second arguments. The comparison is a simple pointer comparison
 * and so only the exact same objects match.
 * \note To check equality on a deeper level the ashListRemoveObjectEqual
 * function should be used instead. This supports an equality function as an
 * extra parameter.
 * \brief Remove and return object in list.
 * \param list List to remove object from.
 * \param object Object to remove.
 * \return Removed object or NULL if no object were found to match.
 */
void* alfListRemoveObject(AlfList* list, const void* object);

// -------------------------------------------------------------------------- //

/** Removes and returns the first object in the list that compares equal to the
 * specied object according to the specified equality function.
 * \brief Removes and return object in list.
 * \param list List to remove object from.
 * \param object Object to remove.
 * \param equalsFunction Function to compare objects in list with specified
 * object.
 * \return Removed object or NULL if no object was removed.
 */
void* alfListRemoveObjectEqual(AlfList* list, const void* object, PFN_AlfCollectionEqual equalsFunction);

// -------------------------------------------------------------------------- //

/** Returns the object at the specified index in a list. The index will be
 * asserted to be valid.
 * \pre The index must be a valid index, meaning it must point to an object
 * in the list.
 * \brief Returns item at index in list.
 * \param list List to get item from.
 * \param index Index in list to get item at.
 * \return Item at index.
 */
void* alfListGet(AlfList* list, uint64_t index);

// -------------------------------------------------------------------------- //

/** Returns the object in the list that compares equal to the specified object
 * according to the specified equality function.
 * \brief Returns object in list.
 * \param list List to get object from.
 * \param object Object to get from list.
 * \param equalsFunction Function to compare objects in list with the specified
 * object.
 * \return Object in list.
 */
void* alfListGetObject(AlfList* list, void* object, PFN_AlfCollectionEqual equalsFunction);

// -------------------------------------------------------------------------- //

/** Resize a list. This changes the current size of the list and also reserves
 * space if size exceeds the capacity.
 * \brief Resize list.
 * \param list List to resize.
 * \param size Size of list.
 */
void alfListResize(AlfList* list, uint64_t size);

// -------------------------------------------------------------------------- //

/** Reserve space in a list. If the capacity is less than the current capacity
 * then nothing will happen. To shrink capacity instead call icListShrink. The
 * size if the list remains unchanged.
 * \brief Reserve space in list.
 * \param list List to reserve space in.
 * \param capacity Capacity to reserve.
 */
void alfListReserve(AlfList* list, uint64_t capacity);

// -------------------------------------------------------------------------- //

/** Shrink list to fit the specified number of objects. If the list is shrunk
 * below the current size, each object past the end will be deleted using the
 * specified destructor function (If not specified they will not be deleted).
 * \brief Shrink list.
 * \param list List to shrink.
 * \param capacity Capacity to shrink to.
 */
void alfListShrink(AlfList* list, uint64_t capacity);

// -------------------------------------------------------------------------- //

/** Shrink the size of a list to fit all the objects in it without wasting any
 * memory.
 * \brief Shrink list to fit all objects in it.
 * \param list List to shrink.
 */
void alfListShrinkToFit(AlfList* list);

// -------------------------------------------------------------------------- //

/** Returns the size of a list in number of objects.
 * \brief Returns list size.
 * \param list List to get size of.
 * \return Size of list.
 */
uint64_t alfGetListSize(AlfList* list);

// -------------------------------------------------------------------------- //

/** Returns a pointer to the underlying data buffer of a list.
 * \brief Returns data pointer of list.
 * \param list List to get data pointer for.
 * \return Data pointer of list.
 */
void* alfGetListData(AlfList* list);

// ========================================================================== //
// ArrayList Structures
// ========================================================================== //

/** \struct AlfArrayListDesc
 * \author Filip Björklund
 * \date 09 januari 2019 - 16:58
 * \brief Array-list descriptor.
 * \details
 * Structure that represents a descriptor for array-list creation.
 */
typedef struct AlfArrayListDesc
{
	/** Size of each object in list **/
	uint32_t objectSize;
	/** Initial capacity **/
	uint64_t capacity;

	/** Function for cleaning objects **/
	PFN_AlfCollectionCleaner cleaner;
} AlfArrayListDesc;

// -------------------------------------------------------------------------- //

/** \struct AlfArrayList
 * \author Filip Björklund
 * \date 09 januari 2019 - 17:00
 * \brief Array-list.
 * \details
 * Represents an array-list where each object is stored contigously in memory.
 * This is implemented differently than the IcList in that each object is
 * directly stored together instead of pointers to each object.
 */
typedef struct tag_AlfArrayList AlfArrayList;

// ========================================================================== //
// ArrayList Functions
// ========================================================================== //

/** Create an array-list from a descriptor.
 * \brief Create array-list.
 * \param desc Array-list descriptor.
 * \return Created list or NULL on failure.
 */
AlfArrayList* alfCreateArrayList(const AlfArrayListDesc* desc);

// -------------------------------------------------------------------------- //

/** Create an array-list for objects of the specified size and with the
 * specified destructor. The destructor may be NULL, in which case a default
 * noop destructor is used.
 * \brief Create array-list from object size and destructor.
 * \param objectSize Size of each object in the list.
 * \param cleaner Cleaner for objects in list. May be NULL.
 * \return Created list or NULL on failure.
 */
AlfArrayList* alfCreateArrayListForObjectSize(
	uint32_t objectSize, 
	PFN_AlfCollectionCleaner cleaner);

// -------------------------------------------------------------------------- //

/** Destroy an array-list for calling the destructor for each element and then
 * freeing the list itself.
 * \brief Destroy array-list.
 * \param list List to destroy.
 */
void alfDestroyArrayList(AlfArrayList* list);

// -------------------------------------------------------------------------- //

/** Add an object to the end of an array-list.
 * \brief Add object to end of array-list.
 * \param list List to add to.
 * \param object Object to add.
 */
void alfArrayListAdd(AlfArrayList* list, const void* object);

// -------------------------------------------------------------------------- //

/** Add an object to the beginning of an array-list.
 * \brief Add object to start of array-list.
 * \param list List to add to.
 * \param object Object to add.
 */
void alfArrayListPrepend(AlfArrayList* list, void* object);

// -------------------------------------------------------------------------- //

/** Insert an item into an array-list at the specified index.
 * \note If index is out of bounds (> size-1) the object will be appended.
 * \brief Insert item in array-list.
 * \param list Array-list to insert into.
 * \param object Object to insert into list.
 * \param index Index to insert at.
 */
void alfArrayListInsert(AlfArrayList* list, void* object, uint64_t index);

// -------------------------------------------------------------------------- //

/** Remove an item from the specified index in an array-list.
 * \pre Index must not be out of bounds.
 * \brief Remove item from array-list.
 * \param list List to remove from.
 * \param index Index to remove from.
 */
void alfArrayListRemove(AlfArrayList* list, uint64_t index);

// -------------------------------------------------------------------------- //

/** Remove an item from the specified index in an array-list. The object is 
 * written to the output parameter 'object' before being removed from the list.
 * \pre Index must not be out of bounds.
 * \brief Remove item from array-list.
 * \param[in] list List to remove from.
 * \param[in] index Index to remove from.
 * \param[in, out] object Removed object is writte to this buffer if non-NULL.
 */
void alfArrayListRemoveGet(AlfArrayList* list, uint64_t index, void* object); 

// -------------------------------------------------------------------------- //

/** Remove an object from an array-list that compares equal to the specified
 * object according to the equality function.
 * \note If no item compares equal to the specified object then nothing will
 * happen.
 * \brief Remove object from array-list.
 * \param list List to remove object from.
 * \param object Object to remove.
 * \param equalsFunction Equality function for comparison of objects.
 */
void alfArrayListRemoveObject(
	AlfArrayList* list,
	void* object,
	PFN_AlfCollectionEqual equalsFunction);

// -------------------------------------------------------------------------- //

/** Returns the object at the specified index in an array-list.
 * \pre Index must not be out of bounds.
 * \brief Returns object at index in array-list.
 * \param list List to get object from.
 * \param index Index to retrieve object at.
 * \return Object at index.
 */
void* alfArrayListGet(AlfArrayList* list, uint64_t index);

// -------------------------------------------------------------------------- //

/** Resize an array-list so that it reflects any changes made to the underlying
 * buffer. This function will reserve more capacity if the list is too small.
 * \brief Resize array-list.
 * \param list List to resize.
 * \param size Size to resize to.
 */
void alfArrayListResize(AlfArrayList* list, uint64_t size);

// -------------------------------------------------------------------------- //

/** Reserve more space in an array-list so that it has the capacity to hold the
 * specified number of objects.
 * \brief Reserve space in array-list.
 * \param list Array-list to reserve capacity in.
 * \param capacity Capacity to reserve.
 */
void alfArrayListReserve(AlfArrayList* list, uint64_t capacity);

// -------------------------------------------------------------------------- //

/** Shrink an array-list to the specified capacity. Any object that is outside
 * the array-list after resizing will be destructed using the specified
 * destructor of the list.
 * \brief Shrink array-list.
 * \param list Array-list to shrink.
 * \param capacity Capacity to shrink array-list to.
 */
void alfArrayListShrink(AlfArrayList* list, uint64_t capacity);

// -------------------------------------------------------------------------- //

/** Shrink an array-list to fit only the current number of objects in it. This
 * will lower the memory overhead to a minimum.
 * \note This should not be called if the list is known to be changing
 * frequently as it will trigger reallocations.
 * \brief Shrink array-list to fit.
 * \param list Array-list to shrink.
 */
void alfArrayListShrinkToFit(AlfArrayList* list);

// -------------------------------------------------------------------------- //

/** Returns the size of an array-list in number of objects stored in it.
 * \brief Returns array-list size.
 * \param list Array-list to get size of.
 * \return Size of array-list.
 */
uint64_t alfGetArrayListSize(AlfArrayList* list);

// -------------------------------------------------------------------------- //

/** Returns the pointer to the underlying data buffer of an array-list.
 * \brief Returns array-list data pointer.
 * \param list Array-list to get data pointer from.
 * \return Data pointer.
 */
uint8_t* alfArrayListGetData(AlfArrayList* list);

// ========================================================================== //
// HashTable Forward Declarations
// ========================================================================== //

typedef struct tag_AlfHashTable AlfHashTable;

// ========================================================================== //
// HashTable Callback Types
// ========================================================================== //

/** Prototype for a function that is used as a callback during hash table 
 * iteration.
 * \param table Hash table that is being iterated
 * \param index Index, in iteration, of key-value pair. This does NOT correspond
 * to any real index in the actual table implementation.
 * \param key Key.
 * \param value Value corresponding to key.
 * \return True should be returned to continue iteration. If false is returned
 * then the iteration stops and the iterate function in turn also returns false.
 */
typedef AlfBool(*PFN_AlfHashTableIterate)(
	AlfHashTable* table,
	uint32_t index,
	const void* key, 
	void* value);

// ========================================================================== //
// HashTable Structures
// ========================================================================== //

/** \struct AlfHashTableDesc
 * \author Filip Björklund
 * \date 09 januari 2019 - 17:43
 * \brief Hash table descriptor.
 * \details
 * Structure that represents a descriptor for hash table creation.
 */
typedef struct AlfHashTableDesc
{
	/** Initial bucket count **/
	uint32_t bucketCount;

	/** Size of value object in bytes **/
	uint32_t valueSize;

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
} AlfHashTableDesc;

// -------------------------------------------------------------------------- //

/** \struct AlfHashTable
 * \author Filip Björklund
 * \date 09 januari 2019 - 17:42
 * \brief Hash table.
 * \details
 * Structure that represents an implementation of a hash table. The hash table
 * is implemented using robin-hood hashing.
 * 
 * Hash tables are used to store key-value pairs. It supports quick insertion 
 * and lookup of values using the corresponding key.
 */
typedef struct tag_AlfHashTable AlfHashTable;

// ========================================================================== //
// HashTable Functions
// ========================================================================== //

AlfHashTable* alfCreateHashTable(const AlfHashTableDesc* desc);

// -------------------------------------------------------------------------- //

/** Create a hash table for use with string keys. This is supported for the 
 * reason that string keys are one of the most common.
 * \brief Create hash table for string keys.
 * \param valueSize Size of values in bytes.
 * \param valueCleaner Cleaner for values in table.
 * \return Created hash table or NULL on failure.
 */
AlfHashTable* alfCreateHashTableSimple(
	uint32_t valueSize,
	PFN_AlfCollectionCleaner valueCleaner);

// -------------------------------------------------------------------------- //

void alfDestroyHashTable(AlfHashTable* table);

// -------------------------------------------------------------------------- //

AlfBool alfHashTableInsert(
	AlfHashTable* table, 
	const void* key, 
	const void* value);

// -------------------------------------------------------------------------- //

void* alfHashTableGet(AlfHashTable* table, const void* key);

// -------------------------------------------------------------------------- //

AlfBool alfHashTableRemove(AlfHashTable* table, const void* key, void* valueOut);

// -------------------------------------------------------------------------- //

AlfBool alfHasKey(AlfHashTable* table, const void* key);

// -------------------------------------------------------------------------- //
	
void alfHashTableResize(AlfHashTable* table, uint32_t size);

// -------------------------------------------------------------------------- //

AlfBool alfHashTableIterate(
	AlfHashTable* table, 
	PFN_AlfHashTableIterate iterateFunction);

// -------------------------------------------------------------------------- //

float alfHashTableGetLoadFactor(AlfHashTable* table);

// -------------------------------------------------------------------------- //

uint64_t alfHashTableGetSize(AlfHashTable* table);

// ========================================================================== //
// End of Header
// ========================================================================== //

#if defined(__cplusplus)
}
#endif

#endif // ALF_COLLECTION_H