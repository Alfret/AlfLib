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

// ========================================================================== //
// Header Includes
// ========================================================================== //

// Standard headers
#include <string.h>

// Alf headers
#define ALF_TEST_CATCH2_INTEROP
#include "alf_test.h"
#include "alf_unicode.h"
#include "alf_thread.h"
#include "alf_collection.h"

// ========================================================================== //
// Local Testing Data
// ========================================================================== //

/** List of 80 fruit names **/
static const char* fruitNames[] = {
		"Apple", "Apricot", "Avocado", "Banana", "Bilberry",
		"Blackberry", "Blackcurrant", "Blueberry", "Boysenberry", "Crab Apples",
		"Currant", "Cherry", "Cherimoya", "Chico Fruit", "Cloudberry",
		"Coconut", "Cranberry", "Cucumber", "Damson", "Date",
		"Dragonfruit", "Durian", "Elderberry", "Feijoa", "Fig",
		"Goji Berry", "Gooseberry", "Grape", "Grapefruit", "Guava",
		"Honeyberry", "Huckleberry", "Jabuticaba", "Jackfruit", "Jambul",
		"Japanese Plum", "Jostaberry", "Jujube", "Juniper Berry", "Kiwano",
		"Kiwifruit", "Kumquat", "Lemon", "Lime", "Loquat",
		"Longan", "Lychee", "Mango", "Mangosteen", "Marionberry",
		"Cantaloupe", "Honeydew", "Watermelon", "Mulberry", "Nectarine",
		"Nance", "Olive", "Orange", "Blood Orange", "Clementine",
		"Mandarine", "Tangerine", "Papaya", "Passionfruit", "Peach",
		"Pear", "Persimmon", "Plantain", "Plum", "Pineapple",
		"Pineberry", "Plumcot", "Pomegranate", "Pomelo", "Purple Mangosteen",
		"Quince", "Raspberry", "Redcurrant", "Salal", "Salak"
};
static const uint32_t fruitNamesCount = 80;

// -------------------------------------------------------------------------- //

/** List of numbers 0 through 79 **/
uint32_t numbers0through79[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16,17,18,19,
		20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,
		40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,
		60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79
};

// ========================================================================== //
// Main Function
// ========================================================================== //

ALF_TEST_MAIN()

// ========================================================================== //
// Unicode Tests
// ========================================================================== //

TEST_CASE("Length", "[UTF-8]")
{
	alfCheckTrue( 
		_state_internal_, 
		0, 
		alfUTF8StringLength("") == 0, "alfUTF8StringLength(\"\") == 0", 
		__FILENAME__, 
		__LINE__,
		(AlfTestCheckParameters){ 0 } 
	);




	
	
	CHECK(alfUTF8StringLength("a") == 1);
	CHECK(alfUTF8StringLength("ö") == 1);
	CHECK(alfUTF8StringLength("åäö") == 3);
	CHECK(alfUTF8StringLength("aö") == 2);
	CHECK(alfUTF8StringLength("öa") == 2);
	CHECK(alfUTF8StringLength(NULL) == 0,
		"NULL strings have a length of 0");
}
// -------------------------------------------------------------------------- //

ALF_TEST("Codepoint Width", "[UTF-8]")
{
	ALF_CHECK_TRUE(alfUTF8CodepointWidth('a') == 1);
	ALF_CHECK_TRUE(alfUTF8CodepointWidth(246) == 2); // ö
	ALF_CHECK_TRUE(alfUTF8CodepointWidth(24328) == 3); // 弈
	ALF_CHECK_TRUE(alfUTF8CodepointWidth(129300) == 4); // 🤔
}

// -------------------------------------------------------------------------- //

ALF_TEST("Insert", "[UTF-8]")
{
	// Insert
	char* output0 = alfUTF8Insert("måndag", 3, 0, "ads");
	ALF_CHECK_STR_EQ(output0, "månadsdag",
		"Add letters in word, no delete");

	char* output1 = alfUTF8Insert("", 0, 0, "månad");
	ALF_CHECK_STR_EQ(output1, "månad",
		"Insert into empty string");

	char* output2 = alfUTF8Insert("T", 1, 0, "h");
	ALF_CHECK_STR_EQ(output2, "Th");
}

// -------------------------------------------------------------------------- //

ALF_TEST("Delete", "[UTF-8]")
{
	// Setup input strings
	const char* input0 = "måndag";

	// Delete
	char* output0 = alfUTF8Insert(input0, 3, 3, "");
	ALF_CHECK_STR_EQ(output0, "mån",
		"Only delete letters, no adding");

	char* output1 = alfUTF8Insert("", 0, 0, "");
	ALF_CHECK_STR_EQ(output1, "",
		"Only delete letters, no adding");
}


// -------------------------------------------------------------------------- //

ALF_TEST("Replace", "[UTF-8]")
{
	// Setup input strings
	const char* input0 = "måndag";

	// Replace
	char* output0 = alfUTF8Insert(input0, 0, 3, "annan");
	ALF_CHECK_STR_EQ(output0, "annandag",
		"Replace letters, delete some and add some");
}

// -------------------------------------------------------------------------- //

ALF_TEST("Substring", "[UTF-8]")
{
	// Setup input strings
	const char* input0 = "måndag";

	// Check strings that starts at beginning
	char* output0 = alfUTF8Substring(input0, 0, 0); // Empty word
	ALF_CHECK_STR_EQ(output0, "",
		"Empty substring at beginning of string");
	char* output1 = alfUTF8Substring(input0, 0, 1); // First letter
	ALF_CHECK_STR_EQ(output1, "m",
		"First letter at beginning of string");
	char* output2 = alfUTF8Substring(input0, 0, 2); // First 2 letters
	ALF_CHECK_STR_EQ(output2, "må",
		"First two letters at beginning of string");
	char* output3 = alfUTF8Substring(input0, 0, 5); // All except 1 letter
	ALF_CHECK_STR_EQ(output3, "månda",
		"Entire string expect last letter");
	char* output4 = alfUTF8Substring(input0, 0, 6); // All
	ALF_CHECK_STR_EQ(output4, "måndag",
		"Entire string");

	// Check strings that ends at the end
	char* output5 = alfUTF8Substring(input0, 5, 0); // Empty word
	ALF_CHECK_STR_EQ(output5, "",
		"Empty substring at beginning of string");
	char* output6 = alfUTF8Substring(input0, 5, 1); // First letter
	ALF_CHECK_STR_EQ(output6, "g",
		"First letter at beginning of string");
	char* output7 = alfUTF8Substring(input0, 4, 2); // First 2 letters
	ALF_CHECK_STR_EQ(output7, "ag",
		"First two letters at beginning of string");
	char* output8 = alfUTF8Substring(input0, 1, 5); // All except 1 letter
	ALF_CHECK_STR_EQ(output8, "åndag",
		"Entire string expect last letter");

	// Invalid index
	char* output9 = alfUTF8Substring(input0, 100, 1);
	ALF_CHECK_NULL(output9, "From index 100, count of 1");

	// Invalid count
	char* output10 = alfUTF8Substring(input0, 0, 100);
	ALF_CHECK_NULL(output10, "From beginning, count of 100");
}

// -------------------------------------------------------------------------- //

ALF_TEST("From Codepoint List", "[UTF-8]")
{
	// Normal list
	uint32_t input0[] = { 67, 246 };
	char buffer0[10];
	uint32_t size0 = 10;
	AlfBool success = alfUTF8FromCodepointList(input0, 2, &size0, buffer0);
	ALF_CHECK(success, "Encoding normal codepoint list should be correct");
	ALF_CHECK_STR_EQ("Cö", buffer0)

	// Check empty list
	uint32_t input1[] = { 0 };
	char buffer1[10];
	uint32_t size1 = 10;
	success = alfUTF8FromCodepointList(input1, 0, &size1, buffer1);
	ALF_CHECK(success, "Encoding empty codepoint list should be correct");
	ALF_CHECK_STR_EQ("", buffer1)

	// Check complex list
	uint32_t input2[] = { 67, 24328, 75, 97, 128526, 128516, 103, 246, 97 };
	uint32_t size2;
	success = alfUTF8FromCodepointList(input2, 9, &size2, NULL);
	ALF_CHECK(success, "Checking the number of bytes needed to encoding UTF-8 "
		"from codepoint list should work");
	char* buffer2 = malloc(size2);
	success = alfUTF8FromCodepointList(input2, 9, &size2, buffer2);
	ALF_CHECK(success, "Encoding complex codepoint list should be correct");
	ALF_CHECK_STR_EQ("C弈Ka😎😄göa", buffer2)
}

// ========================================================================== //
// Collection Tests
// ========================================================================== //

ALF_TEST("Create", "[Hash Table]")
{
	// Test insertion
	AlfHashTable* table = alfCreateHashTableSimple(sizeof(uint32_t), NULL);

	// Check that count is correct
	ALF_CHECK_TRUE(alfHashTableGetSize(table) == 0,
		"Check that hash-table size is 0 after creation");
	ALF_CHECK_TRUE(alfHashTableGetLoadFactor(table) == 0,
		"Check that hash-table load factor is 0 after creation");

	// Destroy table
	alfDestroyHashTable(table);
}

// -------------------------------------------------------------------------- //

ALF_TEST("Insert", "[Hash Table]")
{
	// Test insertion
	AlfHashTable* table = alfCreateHashTableSimple(sizeof(uint32_t), NULL);
	for (uint32_t i = 0; i < fruitNamesCount; i++)
	{
		const AlfBool success =
			alfHashTableInsert(table, fruitNames[i], &numbers0through79[i]);
		ALF_CHECK_TRUE(success, "Check that insertion of value succeeded");
	}

	// Check that count is correct
	ALF_CHECK_TRUE(alfHashTableGetSize(table) == fruitNamesCount,
		"Check that hash-table size is correct after insertions");

	// Destroy table
	alfDestroyHashTable(table);
}

// -------------------------------------------------------------------------- //

ALF_TEST("Get", "[Hash Table]")
{
	// Create table and insert values for retrieval
	AlfHashTable* table = alfCreateHashTableSimple(sizeof(uint32_t), NULL);
	for (uint32_t i = 0; i < fruitNamesCount; i++)
	{
		alfHashTableInsert(table, fruitNames[i], &numbers0through79[i]);
	}

	// Check that value are in list
	for (uint32_t i = 0; i < fruitNamesCount; i++)
	{
		uint32_t* value = alfHashTableGet(table, fruitNames[i]);
		ALF_CHECK_NOT_NULL(value,
			"Check that key is actually in list");
		ALF_CHECK_TRUE(*value == numbers0through79[i],
			"Check that value returned from 'get' matches the value set");
	}

	// Destroy table
	alfDestroyHashTable(table);
}

// -------------------------------------------------------------------------- //

ALF_TEST("Remove", "[Hash Table]")
{
	// Create table and insert values for retrieval
	AlfHashTable* table = alfCreateHashTableSimple(sizeof(uint32_t), NULL);
	for (uint32_t i = 0; i < fruitNamesCount; i++)
	{
		alfHashTableInsert(table, fruitNames[i], &numbers0through79[i]);
	}

	// Remove values on even indices
	for (uint32_t i = 0; i < fruitNamesCount; i++)
	{
		if (i % 2 == 0)
		{
			alfHashTableRemove(table, fruitNames[i], NULL);
		}
	}

	// Check that value are in list
	for (uint32_t i = 0; i < fruitNamesCount; i++)
	{
		uint32_t* value = alfHashTableGet(table, fruitNames[i]);
		if (i % 2 == 0)
		{
			ALF_CHECK_NULL(value,
				"Removed value should not be retrievable from table");
		}
		else
		{
			ALF_CHECK_NOT_NULL(value,
				"Check that key is actually in list");
			if (value)
			{
				ALF_CHECK_TRUE(*value == numbers0through79[i],
					"Check that value returned from 'get' matches the value set");
			}
		}
	}

	// Destroy table
	alfDestroyHashTable(table);
}