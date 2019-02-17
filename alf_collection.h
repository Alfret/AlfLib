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
typedef AlfBool(*PFN_AlfCollectionEqual)(
	const void* object0, 
	const void* object1);

// -------------------------------------------------------------------------- //

/** Prototype of a function to compare two objects in a collection. The value 
 * that is returned signifies the relative value of one object to the other. A
 * value of 1 means that the first object is greater than the second, -1 means
 * the opposite and 0 means that both values are equal.
 * \param object0 First object in comparison.
 * \param object1 Second object in comparison.
 * \return 1, -1 or 0 depending on the comparison result.
 */
typedef int32_t(*PFN_AlfCollectionCompare)(
	const void* object0, 
	const void* object1);

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
 * \param[in] desc List descriptor.
 * \return Created list or NULL on failure.
 */
AlfList* alfCreateList(const AlfListDesc* desc);

// -------------------------------------------------------------------------- //

/** Create a list without a specified descriptor. The list will have the default
 * capacity and destructor set.
 * \return Created list or NULL on failure.
 */
AlfList* alfCreateListSimple(void);

// -------------------------------------------------------------------------- //

/** Destroy a list. This will call the destructor for all remaining items in the
 * list.
 * \brief Destroy list.
 * \param[in] list List to destroy.
 */
void alfDestroyList(AlfList* list);

// -------------------------------------------------------------------------- //

/** Add an object to the end of a list.
 * \brief Add object to end of list.
 * \param[in] list List to add object to.
 * \param[in] object Object to add.
 */
void alfListAdd(AlfList* list, void* object);

// -------------------------------------------------------------------------- //

/** Add an object to the beginning of a list.
 * \brief Add object to start of list.
 * \param[in] list List to add object to.
 * \param[in] object Object to add.
 */
void alfListPrepend(AlfList* list, void* object);

// -------------------------------------------------------------------------- //

/** Add an object to the specified index in a list. If the index is larger than
 * the highest index the object will be added to the end of the list. If the
 * index is 0 the object is added to the beginning of the list.
 * \brief Insert object into list.
 * \param[in] list List to insert object into.
 * \param[in] object Object to insert into list.
 * \param[in] index Index to insert object at.
 */
void alfListInsert(AlfList* list, void* object, uint64_t index);

// -------------------------------------------------------------------------- //

/** Remove an object from a list and return it. If the index is out of bounds
 * the function will return null.
 * \brief Remove and return object in list.
 * \param[in] list List to remove object from.
 * \param[in] index Index to return object at.
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
 * \param[in] list List to remove object from.
 * \param[in] object Object to remove.
 * \return Removed object or NULL if no object were found to match.
 */
void* alfListRemoveObject(AlfList* list, const void* object);

// -------------------------------------------------------------------------- //

/** Removes and returns the first object in the list that compares equal to the
 * specied object according to the specified equality function.
 * \brief Removes and return object in list.
 * \param[in] list List to remove object from.
 * \param[in] object Object to remove.
 * \param[in] equalsFunction Function to compare objects in list with specified
 * object.
 * \return Removed object or NULL if no object was removed.
 */
void* alfListRemoveObjectEqual(
	AlfList* list, 
	const void* object, 
	PFN_AlfCollectionEqual equalsFunction);

// -------------------------------------------------------------------------- //

/** Returns the object at the specified index in a list. The index will be
 * asserted to be valid.
 * \pre The index must be a valid index, meaning it must point to an object
 * in the list.
 * \brief Returns item at index in list.
 * \param[in] list List to get item from.
 * \param[in] index Index in list to get item at.
 * \return Item at index.
 */
void* alfListGet(AlfList* list, uint64_t index);

// -------------------------------------------------------------------------- //

/** Returns the object in the list that compares equal to the specified object
 * according to the specified equality function.
 * \brief Returns object in list.
 * \param[in] list List to get object from.
 * \param[in] object Object to get from list.
 * \param[in] equalsFunction Function to compare objects in list with the specified
 * object.
 * \return Object in list.
 */
void* alfListGetObject(
	AlfList* list, 
	void* object, 
	PFN_AlfCollectionEqual equalsFunction);

// -------------------------------------------------------------------------- //

/** Resize a list. This changes the current size of the list and also reserves
 * space if size exceeds the capacity.
 * \brief Resize list.
 * \param[in] list List to resize.
 * \param[in] size Size of list.
 */
void alfListResize(AlfList* list, uint64_t size);

// -------------------------------------------------------------------------- //

/** Reserve space in a list. If the capacity is less than the current capacity
 * then nothing will happen. To shrink capacity instead call icListShrink. The
 * size if the list remains unchanged.
 * \brief Reserve space in list.
 * \param[in] list List to reserve space in.
 * \param[in] capacity Capacity to reserve.
 */
void alfListReserve(AlfList* list, uint64_t capacity);

// -------------------------------------------------------------------------- //

/** Shrink list to fit the specified number of objects. If the list is shrunk
 * below the current size, each object past the end will be deleted using the
 * specified destructor function (If not specified they will not be deleted).
 * \brief Shrink list.
 * \param[in] list List to shrink.
 * \param[in] capacity Capacity to shrink to.
 */
void alfListShrink(AlfList* list, uint64_t capacity);

// -------------------------------------------------------------------------- //

/** Shrink the size of a list to fit all the objects in it without wasting any
 * memory.
 * \brief Shrink list to fit all objects in it.
 * \param[in] list List to shrink.
 */
void alfListShrinkToFit(AlfList* list);

// -------------------------------------------------------------------------- //

/** Returns the size of a list in number of objects.
 * \brief Returns list size.
 * \param[in] list List to get size of.
 * \return Size of list.
 */
uint64_t alfGetListSize(AlfList* list);

// -------------------------------------------------------------------------- //

/** Returns a pointer to the underlying data buffer of a list.
 * \brief Returns data pointer of list.
 * \param[in] list List to get data pointer for.
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
 * \param[in] desc Array-list descriptor.
 * \return Created list or NULL on failure.
 */
AlfArrayList* alfCreateArrayList(const AlfArrayListDesc* desc);

// -------------------------------------------------------------------------- //

/** Create an array-list for objects of the specified size and with the
 * specified destructor. The destructor may be NULL, in which case a default
 * noop destructor is used.
 * \brief Create array-list from object size and destructor.
 * \param[in] objectSize Size of each object in the list.
 * \param[in] cleaner Cleaner for objects in list. May be NULL.
 * \return Created list or NULL on failure.
 */
AlfArrayList* alfCreateArrayListForObjectSize(
	uint32_t objectSize, 
	PFN_AlfCollectionCleaner cleaner);

// -------------------------------------------------------------------------- //

/** Destroy an array-list for calling the destructor for each element and then
 * freeing the list itself.
 * \brief Destroy array-list.
 * \param[in] list List to destroy.
 */
void alfDestroyArrayList(AlfArrayList* list);

// -------------------------------------------------------------------------- //

/** Add an object to the end of an array-list.
 * \brief Add object to end of array-list.
 * \param[in] list List to add to.
 * \param[in] object Object to add.
 */
void alfArrayListAdd(AlfArrayList* list, const void* object);

// -------------------------------------------------------------------------- //

/** Add an object to the beginning of an array-list.
 * \brief Add object to start of array-list.
 * \param[in] list List to add to.
 * \param[in] object Object to add.
 */
void alfArrayListPrepend(AlfArrayList* list, const void* object);

// -------------------------------------------------------------------------- //

/** Insert an item into an array-list at the specified index.
 * \note If index is out of bounds (> size-1) the object will be appended.
 * \brief Insert item in array-list.
 * \param[in] list Array-list to insert into.
 * \param[in] object Object to insert into list.
 * \param[in] index Index to insert at.
 */
void alfArrayListInsert(AlfArrayList* list, const void* object, uint64_t index);

// -------------------------------------------------------------------------- //

/** Remove an item from the specified index in an array-list.
 * \pre Index must not be out of bounds.
 * \brief Remove item from array-list.
 * \param[in] list List to remove from.
 * \param[in] index Index to remove from.
 */
void alfArrayListRemove(AlfArrayList* list, uint64_t index);

// -------------------------------------------------------------------------- //

/** Remove an item from the specified index in an array-list. The object is 
 * written to the output parameter 'object' before being removed from the list.
 * \pre Index must not be out of bounds.
 * \brief Remove item from array-list.
 * \param[in] list List to remove from.
 * \param[in] index Index to remove from.
 * \param[in,out] object Removed object is writte to this buffer if non-NULL.
 */
void alfArrayListRemoveGet(AlfArrayList* list, uint64_t index, void* object); 

// -------------------------------------------------------------------------- //

/** Remove an object from an array-list that compares equal to the specified
 * object according to the equality function.
 * \note If no item compares equal to the specified object then nothing will
 * happen.
 * \brief Remove object from array-list.
 * \param[in] list List to remove object from.
 * \param[in] object Object to remove.
 * \param[in] equalsFunction Equality function for comparison of objects.
 */
void alfArrayListRemoveObject(
	AlfArrayList* list,
	void* object,
	PFN_AlfCollectionEqual equalsFunction);

// -------------------------------------------------------------------------- //

/** Returns the object at the specified index in an array-list.
 * \pre Index must not be out of bounds.
 * \brief Returns object at index in array-list.
 * \param[in] list List to get object from.
 * \param[in] index Index to retrieve object at.
 * \return Object at index.
 */
void* alfArrayListGet(const AlfArrayList* list, uint64_t index);

// -------------------------------------------------------------------------- //

/** Resize an array-list so that it reflects any changes made to the underlying
 * buffer. This function will reserve more capacity if the list is too small.
 * \brief Resize array-list.
 * \param[in] list List to resize.
 * \param[in] size Size to resize to.
 */
void alfArrayListResize(AlfArrayList* list, uint64_t size);

// -------------------------------------------------------------------------- //

/** Reserve more space in an array-list so that it has the capacity to hold the
 * specified number of objects.
 * \brief Reserve space in array-list.
 * \param[in] list Array-list to reserve capacity in.
 * \param[in] capacity Capacity to reserve.
 */
void alfArrayListReserve(AlfArrayList* list, uint64_t capacity);

// -------------------------------------------------------------------------- //

/** Shrink an array-list to the specified capacity. Any object that is outside
 * the array-list after resizing will be destructed using the specified
 * destructor of the list.
 * \brief Shrink array-list.
 * \param[in] list Array-list to shrink.
 * \param[in] capacity Capacity to shrink array-list to.
 */
void alfArrayListShrink(AlfArrayList* list, uint64_t capacity);

// -------------------------------------------------------------------------- //

/** Shrink an array-list to fit only the current number of objects in it. This
 * will lower the memory overhead to a minimum.
 * \note This should not be called if the list is known to be changing
 * frequently as it will trigger reallocations.
 * \brief Shrink array-list to fit.
 * \param[in] list Array-list to shrink.
 */
void alfArrayListShrinkToFit(AlfArrayList* list);

// -------------------------------------------------------------------------- //

/** Returns the size of an array-list in number of objects stored in it.
 * \brief Returns array-list size.
 * \param[in] list Array-list to get size of.
 * \return Size of array-list.
 */
uint64_t alfGetArrayListSize(const AlfArrayList* list);

// -------------------------------------------------------------------------- //

/** Returns the pointer to the underlying data buffer of an array-list.
 * \brief Returns array-list data pointer.
 * \param[in] list Array-list to get data pointer from.
 * \return Data pointer.
 */
const uint8_t* alfArrayListGetData(const AlfArrayList* list);

// -------------------------------------------------------------------------- //

/** Sort an array list in ascending order. The 'compareFunction' is used to 
 * compare two objects to see which should appear before the other. Ties are 
 * broken arbitrarily.
 * \brief Sort array list.
 * \param list List to sort.
 * \param compareFunction Comparison function. 
 */
void alfArrayListSort(
	AlfArrayList* list, 
	PFN_AlfCollectionCompare compareFunction);

// ========================================================================== //
// Stack Structures
// ========================================================================== //

/** \struct AlfStackDesc
 * \author Filip Björklund
 * \date 12 januari 2019 - 22:58
 * \brief Stack descriptor.
 * \details
 * Structure that represents a descriptor for stack creation.
 */
typedef struct AlfStackDesc 
{
	/** Initial capacity **/
	uint32_t capacity;
	/** Size of objects in stack **/
	uint32_t objectSize;
	/** Object cleaner **/
	PFN_AlfCollectionCleaner objectCleaner;
} AlfStackDesc;

// -------------------------------------------------------------------------- //

/** \struct AlfStack
 * \author Filip Björklund
 * \date 12 januari 2019 - 22:57
 * \brief Stack.
 * \details
 * Structure that represents a stack collection. Objects can be pushed onto the
 * top of the stack and then popped back of. When popping an item from the stack
 * the latest object pushed onto the stack will be returned.
 */
typedef struct tag_AlfStack AlfStack;

// ========================================================================== //
// Stack Functions
// ========================================================================== //

/** Create a stack from a stack descriptor.
 * \brief Create stack.
 * \param[in] desc Stack descriptor.
 * \return Created stack or NULL on failure.
 */
AlfStack* alfCreateStack(const AlfStackDesc* desc);

// -------------------------------------------------------------------------- //

/** Destroy a stack. The cleaner set during creation will be called for each of
 * the objects that still remain in the stack.
 * \brief Destroy stack.
 * \param[in] stack Stack to destroy.
 */
void alfDestroyStack(AlfStack* stack);

// -------------------------------------------------------------------------- //

/** Push an object onto a stack.
 * \brief Push object onto stack
 * \param[in] stack Stack to push object onto.
 * \param[in] object Object to push onto stack.
 * \return True if object could successfully be pushed onto the stack otherwise
 * false.
 */
AlfBool alfStackPush(AlfStack* stack, const void* object);

// -------------------------------------------------------------------------- //

/** Pop an object from a stack.
 * \brief Pop object from stack.
 * \param[in] stack Stack to pop object from.
 * \param[in,out] objectOut Object that was popped from the stack. This will only be set
 * if the function returns true to signal a successful pop.
 * \return True if item was popped from stack otherwise false.
 */
AlfBool alfStackPop(AlfStack* stack, void *objectOut);

// -------------------------------------------------------------------------- //

/** Resize stack to hold a specified number of objects.
 * \brief Resize stack.
 * \param[in] stack Stack to resize.
 * \param[in] size Size to resize stack to.
 * \return True if stack could be resized otherwise false.
 */
AlfBool alfStackResize(AlfStack* stack, uint32_t size);

// -------------------------------------------------------------------------- //

/** Returns the number of items that are currently present in stack.
 * \brief Returns stack size.
 * \param[in] stack Stack to get size of.
 * \return Stack size.
 */
uint32_t alfStackGetSize(AlfStack* stack);

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

/** Create a hash table from a descriptor. Most of the callback in the 
 * descriptor must be set for the hash table to work correctly. See the 
 * descriptor documentation for more information.
 * \note To create a hash table setup for using strings as keys, use the 
 * creation function alfCreateHashTableSimple instead.
 * \brief Create hash table.
 * \param[in] desc Hash table descriptor.
 * \return Created hash table or NULL on failure.
 */
AlfHashTable* alfCreateHashTable(const AlfHashTableDesc* desc);

// -------------------------------------------------------------------------- //

/** Create a hash table for use with string keys. This is supported for the 
 * reason that string keys are one of the most common.
 * \brief Create hash table for string keys.
 * \param[in] valueSize Size of values in bytes.
 * \param[in] valueCleaner Cleaner for values in table.
 * \return Created hash table or NULL on failure.
 */
AlfHashTable* alfCreateHashTableSimple(
	uint32_t valueSize,
	PFN_AlfCollectionCleaner valueCleaner);

// -------------------------------------------------------------------------- //

/** Destroy a hash table and also all the remaining entries in it. The 
 * destructors and cleanup functions set during creation of the has table is 
 * used for this.
 * \brief Destroy hash table.
 * \param[in] table Hash table to destroy.
 */
void alfDestroyHashTable(AlfHashTable* table);

// -------------------------------------------------------------------------- //

/** Insert a value into a hash table with the specified key.
 * \brief Insert value into hash table.
 * \param[in] table Hash table to insert into.
 * \param[in] key Key to insert value with.
 * \param[in] value Value to insert.
 * \return True if insertion succeeded, otherwise false.
 */
AlfBool alfHashTableInsert(
	AlfHashTable* table, 
	const void* key, 
	const void* value);

// -------------------------------------------------------------------------- //

/** Returns the value in the list that corresponds to a specified key. If no 
 * value exists with the key then the function returns NULL.
 * \brief Returns value from hash table.
 * \param[in] table Hash table to get value from.
 * \param[in] key Key to lookup value with.
 * \return Value that was found for the key or NULL if the key was not found.
 */
void* alfHashTableGet(AlfHashTable* table, const void* key);

// -------------------------------------------------------------------------- //

/** Remove a value that is stored with a specified key from a hash table.
 * \brief Remove value from hash table.
 * \param[in] table Hash table to remove value from.
 * \param[in] key Key to lookup the value to remove.
 * \param[in,out] valueOut Removed value, this is only valid if the function
 * also returns true.
 * \return True if the value could be removed otherwise false.
 */
AlfBool alfHashTableRemove(
	AlfHashTable* table, 
	const void* key, 
	void* valueOut);

// -------------------------------------------------------------------------- //

/** Returns whether or not a hash table contains a value stored with the 
 * specified key.
 * \brief Returns whether hash table contains key.
 * \param[in] table Hash table to check if contains key.
 * \param[in] key Key to check if table contains.
 * \return True if the hash table contains the specified key otherwise false.
 */
AlfBool alfHasKey(AlfHashTable* table, const void* key);

// -------------------------------------------------------------------------- //
	
/** Resize a hash table to the specified size. 
 * \note This will move all values from the old table, which can be a costly 
 * operation. Therefore call this as infrequently as possible, or make a large
 * upfront resize.
 * \note The size must be a power of two.
 * \brief Resize hash table.
 * \param[in] table Hash table to resize.
 * \param[in] size Size to resize to. Must be a power of two.
 */
void alfHashTableResize(AlfHashTable* table, uint32_t size);

// -------------------------------------------------------------------------- //

/** Iterate a hash table and call the specfied callback for each key-value pair
 * in the table. The order of the pairs are not specified.
 * \brief Iterate hash table entries.
 * \param[in] table Hash table to iterate.
 * \param[in] iterateFunction Function to call for each entry during iteration.
 * \return True if the iteration completed otherwise false. The iteration stops
 * if the iterator function for an object returned false.
 */
AlfBool alfHashTableIterate(
	AlfHashTable* table, 
	PFN_AlfHashTableIterate iterateFunction);

// -------------------------------------------------------------------------- //

/** Set the maximum load factor of a hash table. This value determines how 
 * filled the hash table will become before it's automatically resized. A value
 * of 1.0 means filled. Insertion an item into a fill table will always result 
 * in failure, meaning that this value cannot exceed 1.0.
 * \brief Set maximum load factor of hash table.
 * \param[in] table Hash table to set maximum load factor of.
 * \param[in] loadFactor Load factor to set.
 */
void alfHashTableSetMaxLoadFactor(AlfHashTable* table, float loadFactor);

// -------------------------------------------------------------------------- //

/** Returns the current load factor of a hash table. This represents how filled
 * the hash table is. 
 * \brief Returns hash table load factor.
 * \param[in] table Hash table to get load factor of.
 * \return Load factor of hash table.
 */
float alfHashTableGetLoadFactor(AlfHashTable* table);

// -------------------------------------------------------------------------- //

/** Returns the size of a hash table. This represents the current number of 
 * entries stored in the table.
 * \brief Returns size of hash table.
 * \param[in] table Hash table to get size of.
 * \return Size of hash table.
 */
uint64_t alfHashTableGetSize(AlfHashTable* table);

// ========================================================================== //
// End of Header
// ========================================================================== //

#if defined(__cplusplus)
}
#endif

#endif // ALF_COLLECTION_H
