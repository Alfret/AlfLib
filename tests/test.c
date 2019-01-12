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

#define ALF_TEST_IMPL
#include "alf_test.h"

#include "alf_unicode.h"
#include "alf_thread.h"
#include "alf_collection.h"

// ========================================================================== //
// Unicode Tests
// ========================================================================== //

void test_alfUTF8Insert(AlfTestState* state)
{
	// Setup input strings
	const char* input0 = u8"måndag";

	// Insert
	char* output0 = alfUTF8Insert(input0, 3, 0, u8"ads");
	alfCheckStrEqM(state, output0, u8"månadsdag", 
		"Add letters in word, no delete");

	// Delete
	char* output1 = alfUTF8Insert(input0, 3, 3, u8"");
	alfCheckStrEqM(state, output1, u8"mån", 
		"Only delete letters, no adding");

	// Replace
	char* output2 = alfUTF8Insert(input0, 0, 3, u8"annan");
	alfCheckStrEqM(state, output2, u8"annandag", 
		"Replace letters, delete some and add some");
}

// -------------------------------------------------------------------------- //

void test_alfUTF8Substring(AlfTestState* state)
{
	// Setup input strings
	const char* input0 = u8"måndag";

	// Check strings that starts at beginning
	char* output0 = alfUTF8Substring(input0, 0, 0); // Empty word
	alfCheckStrEqM(state, output0, u8"", 
		"Empty substring at beginning of string");
	char* output1 = alfUTF8Substring(input0, 0, 1); // First letter
	alfCheckStrEqM(state, output1, u8"m", 
		"First letter at beginning of string");
	char* output2 = alfUTF8Substring(input0, 0, 2); // First 2 letters
	alfCheckStrEqM(state, output2, u8"må", 
		"First two letters at beginning of string");
	char* output3 = alfUTF8Substring(input0, 0, 5); // All except 1 letter
	alfCheckStrEqM(state, output3, u8"månda", 
		"Entire string expect last letter");
	char* output4 = alfUTF8Substring(input0, 0, 6); // All
	alfCheckStrEqM(state, output4, u8"måndag", 
		"Entire string");

	// Check strings that ends at the end
	char* output5 = alfUTF8Substring(input0, 5, 0); // Empty word
	alfCheckStrEqM(state, output5, u8"", 
		"Empty substring at beginning of string");
	char* output6 = alfUTF8Substring(input0, 5, 1); // First letter
	alfCheckStrEqM(state, output6, u8"g", 
		"First letter at beginning of string");
	char* output7 = alfUTF8Substring(input0, 4, 2); // First 2 letters
	alfCheckStrEqM(state, output7, u8"ag", 
		"First two letters at beginning of string");
	char* output8 = alfUTF8Substring(input0, 1, 5); // All except 1 letter
	alfCheckStrEqM(state, output8, u8"åndag", 
		"Entire string expect last letter");

	// Invalid index
	char* output9 = alfUTF8Substring(input0, 100, 1);
	alfCheckNullM(state, output9, "From index 100, count of 1");

	// Invalid count
	char* output10 = alfUTF8Substring(input0, 0, 100);
	alfCheckNullM(state, output10, "From beginning, count of 100");
}

// ========================================================================== //
// Thread Tests
// ========================================================================== //

int32_t threadStartFunction0(void* args) { return (int32_t)(int64_t)args; }

// -------------------------------------------------------------------------- //

int32_t threadStartFunction1(void* args)
{
	return (int32_t)strlen(alfGetThreadName());
}

// -------------------------------------------------------------------------- //

void test_alfCreateThread(AlfTestState* state)
{
	// Make sure code is passed through
	const int32_t code = 56;
	AlfThread* thread = 
		alfCreateThread(threadStartFunction0, (void*)(int64_t)(code));
	int32_t exitCode = alfJoinThread(thread);
	alfCheckTrueM(state, exitCode == code, 
		"Exit code from thread should match input");

	// Create named
	const char* name = "test_thread";
	thread = alfCreateThreadNamed(
		threadStartFunction1, 
		(void*)(int64_t)(code), 
		name
	);
	exitCode = alfJoinThread(thread);
	alfCheckTrueM(state, exitCode == strlen(name),
		"Exit code from thread should match length of the name set for the "
		"thread");
}

// ========================================================================== //
// Collection Tests
// ========================================================================== //

void test_alfCreateHashTable(AlfTestState* state)
{
	// Setup test data
	const char* keys[] = {
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
	uint32_t values[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16,17,18,19,
		20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,
		40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,
		60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79
	};
	const uint32_t keyCount = sizeof(keys) / sizeof(keys[0]);
	const uint32_t valueCount = sizeof(values) / sizeof(values[0]);

	// Test insertion
	AlfHashTable* table = alfCreateHashTableSimple(sizeof(uint32_t), NULL);
	for (uint32_t i = 0; i < keyCount; i++)
	{
		const AlfBool success = alfHashTableInsert(table, keys[i], &values[i]);
		alfCheckTrueM(state, success, 
			"Check that insertion of all values work");
	}

	// Check that count is correct
	alfCheckTrueM(state, alfHashTableGetSize(table) == keyCount, 
		"Check that hash-table size is correct after insertions");

	// Check that value are in list
	for (uint32_t i = 0; i < keyCount; i++)
	{
		uint32_t* value = alfHashTableGet(table, keys[i]);
		alfCheckNotNullM(state, value,
			"Check that keys are actually in list");
		alfCheckTrueM(state, *value == values[i],
			"Check that value returned from 'get' is correct");
	}

	// Destroy table
	alfDestroyHashTable(table);
}

// ========================================================================== //
// Main Function
// ========================================================================== //

int main()
{
	// Startup
	alfThreadStartup();

	// Setup suite: Unicode
	AlfTest testsUnicode[] = {
		{ "Insert", test_alfUTF8Insert },
		{ "Substring", test_alfUTF8Substring }
	};
	AlfTestSuite* suiteUnicode =
		alfCreateTestSuite("Unicode", testsUnicode, 2);

	// Setup suite: Thread
	AlfTest testsThread[] = {
		{ "Create Thread", test_alfCreateThread }
	};
	AlfTestSuite* suiteThread =
		alfCreateTestSuite("Thread", testsThread, 1);

	// Setup suite: Collection
	AlfTest testsCollection[] = {
		{ "Create Hash Table", test_alfCreateHashTable }
	};
	AlfTestSuite* suiteCollection =
		alfCreateTestSuite("Collection", testsCollection, 1);

	// Run suites
	AlfTestSuite* suites[] = {
		suiteUnicode,
		suiteThread,
		suiteCollection
	};
	const AlfBool failures = alfRunSuites(suites, 3);
	alfDestroyTestSuite(suiteCollection);
	alfDestroyTestSuite(suiteThread);
	alfDestroyTestSuite(suiteUnicode);

	// Shutdown
	alfThreadShutdown();
	return failures;
}