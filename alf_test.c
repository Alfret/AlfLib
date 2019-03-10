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

#include "alf_test.h"

// ========================================================================== //
// Header includes
// ========================================================================== //

// Standard headers
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>

// Windows headers
#if defined(_WIN32)
#	include <Windows.h>
#endif

// MacOS headers
#if defined(__APPLE__)
#	include <mach/mach_time.h>
#endif

// Linux headers
#if defined(__linux__)
#	include <time.h>
#endif

// ========================================================================== //
// Color themes declarations
// ========================================================================== //

// Template for color themes
#if 0


#endif

// Color reset macro
#if !defined(ALF_THEME_NONE)
#	define ALF_TEST_CC_RESET "\033[0m"
#else
#	define ALF_TEST_CC_RESET ""
#endif

// Color macros for different themes and support
#if defined(ALF_TEST_NO_TRUECOLOR) || defined(ALF_THEME_NONE)
#	if defined(ALF_THEME_NONE)
#		define ALF_TEST_CC_SUITE ""
#		define ALF_TEST_CC_NAME ""
#		define ALF_TEST_CC_FILE ""
#		define ALF_TEST_CC_LINE ""
#		define ALF_TEST_CC_TIME ""
#		define ALF_TEST_CC_PASS ""
#		define ALF_TEST_CC_FAIL ""
#		define ALF_TEST_CC_TYPE ""
#	elif defined(ALF_TEST_THEME_PALE)
#		define ALF_TEST_CC_SUITE "\033[38;5;93m"
#		define ALF_TEST_CC_NAME "\033[38;5;63m"
#		define ALF_TEST_CC_FILE "\033[38;5;69m"
#		define ALF_TEST_CC_LINE "\033[38;5;26m"
#		define ALF_TEST_CC_TIME "\033[38;5;220m"
#		define ALF_TEST_CC_PASS "\033[38;5;85m"
#		define ALF_TEST_CC_FAIL "\033[38;5;197m"
#		define ALF_TEST_CC_TYPE "\033[38;5;171m"
#	elif defined(ALF_TEST_THEME_AUTUMN_FRUIT)
#		define ALF_TEST_CC_SUITE ""
#		define ALF_TEST_CC_NAME ""
#		define ALF_TEST_CC_FILE ""
#		define ALF_TEST_CC_LINE ""
#		define ALF_TEST_CC_TIME ""
#		define ALF_TEST_CC_PASS ""
#		define ALF_TEST_CC_FAIL ""
#		define ALF_TEST_CC_TYPE ""
#	else // Default non-truecolor theme
#		define ALF_TEST_CC_SUITE ""
#		define ALF_TEST_CC_NAME ""
#		define ALF_TEST_CC_FILE ""
#		define ALF_TEST_CC_LINE ""
#		define ALF_TEST_CC_TIME ""
#		define ALF_TEST_CC_PASS ""
#		define ALF_TEST_CC_FAIL ""
#		define ALF_TEST_CC_TYPE ""
#	endif
#else
#	define ALF_TEST_CC(r, g, b) "\033[38;2;" #r ";" #g ";" #b "m"
#	if defined(ALF_TEST_THEME_NONE)
#		define ALF_TEST_CC_SUITE ""
#		define ALF_TEST_CC_NAME ""
#		define ALF_TEST_CC_FILE ""
#		define ALF_TEST_CC_LINE ""
#		define ALF_TEST_CC_TIME ""
#		define ALF_TEST_CC_PASS ""
#		define ALF_TEST_CC_FAIL ""
#		define ALF_TEST_CC_TYPE ""
#	elif defined(ALF_TEST_THEME_PALE)
#		define ALF_TEST_CC_SUITE ALF_TEST_CC(137, 93, 226)
#		define ALF_TEST_CC_NAME ALF_TEST_CC(91, 138, 224)
#		define ALF_TEST_CC_FILE ALF_TEST_CC(120, 159, 230)
#		define ALF_TEST_CC_LINE ALF_TEST_CC(66, 120, 220)
#		define ALF_TEST_CC_TIME ALF_TEST_CC(255, 196, 88)
#		define ALF_TEST_CC_PASS ALF_TEST_CC(77, 225, 169)
#		define ALF_TEST_CC_FAIL ALF_TEST_CC(255, 71, 57)
#		define ALF_TEST_CC_TYPE ALF_TEST_CC(189, 99, 197)
#	elif defined(ALF_TEST_THEME_AUTUMN_FRUIT)
#		define ALF_TEST_CC_SUITE ALF_TEST_CC(111, 88, 201)
#		define ALF_TEST_CC_NAME ALF_TEST_CC(87, 74, 226)
#		define ALF_TEST_CC_FILE ALF_TEST_CC(101, 69, 151)
#		define ALF_TEST_CC_LINE ALF_TEST_CC(232, 142, 237)
#		define ALF_TEST_CC_TIME ALF_TEST_CC(34, 42, 104)
#		define ALF_TEST_CC_PASS ALF_TEST_CC(189, 228, 167)
#		define ALF_TEST_CC_FAIL ALF_TEST_CC(216, 30, 91)
#		define ALF_TEST_CC_TYPE ALF_TEST_CC(255, 199, 134)
#	else // Default truecolor theme
#		define ALF_TEST_CC_SUITE ALF_TEST_CC(111, 88, 201)
#		define ALF_TEST_CC_NAME ALF_TEST_CC(34, 116, 165)
#		define ALF_TEST_CC_FILE ALF_TEST_CC(241, 196, 15)
#		define ALF_TEST_CC_LINE ALF_TEST_CC(247, 92, 3)
#		define ALF_TEST_CC_TIME ALF_TEST_CC(27, 153, 139)
#		define ALF_TEST_CC_PASS ALF_TEST_CC(0, 204, 102)
#		define ALF_TEST_CC_FAIL ALF_TEST_CC(217, 3, 104)
#		define ALF_TEST_CC_TYPE ALF_TEST_CC(244, 128, 194)
#	endif
#endif // defined(ALF_TEST_NO_TRUECOLOR)

// Library name color
#if !defined(ALF_THEME_NONE)
#	define ALF_TEST_CC_LOGO "\033[38;5;112m"
#	define ALF_TEST_CC_C "\033[38;5;45m"
#endif

// ========================================================================== //
// Macro Declarations
// ========================================================================== //

/** Size of format buffer **/
#define ALF_TEST_FORMAT_BUFFER_SIZE 1024

// ========================================================================== //
// Global Variables
// ========================================================================== //


// -------------------------------------------------------------------------- //

/** Buffer that is used for temporary storage when formatting string **/
static char sFormatBuffer[ALF_TEST_FORMAT_BUFFER_SIZE];

// ========================================================================== //
// Private structures
// ========================================================================== //

/** Internal test structure **/
typedef struct AlfTestInternal
{
	/** Name of test **/
	char* name;
	/** Test function **/
	PFN_AlfTest TestFunction;
} AlfTestInternal;

// -------------------------------------------------------------------------- //

/** Private test-state structure **/
typedef struct tag_AlfTestState
{
	/** Suite that state represents **/
	AlfTestSuite* suite;
	/** Total check count **/
	AlfTestInt count;
	/** Failed check count **/
	AlfTestInt failCount;
} tag_AlfTestState;

// -------------------------------------------------------------------------- //

/** Private test-suite structure **/
typedef struct tag_AlfTestSuite
{
	/** Setup function **/
	PFN_AlfSuiteSetup Setup;
	/** Teardown function **/
	PFN_AlfSuiteTeardown Teardown;

	/** Name of test suite **/
	char* name;
	/** User data **/
	void* userData;

	/** State for use during tests **/
	AlfTestState state;

	/** Test count **/
	AlfTestInt testCount;
	/** Tests**/
	AlfTestInternal* tests;
} tag_AlfTestSuite;

// ========================================================================== //
// Private function implementations
// ========================================================================== //

/** Setup console for platforms that does not support escape sequences out of
 * the box **/
static void _alfSetupConsole()
{
#if defined(_WIN32)
	static AlfTestInt isSetup = 0;
	if (!isSetup)
	{
		// Enable virtual terminal processing for Color support
		DWORD mode;
		const HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (outputHandle)
		{
			BOOL success = GetConsoleMode(outputHandle, &mode);
			assert(success && "Failet to get Windows console mode");
			success = SetConsoleMode(outputHandle, 
				mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
			assert(success && "Failed to set Windows console mode");
		}

		// Set console encoding
		SetConsoleOutputCP(CP_UTF8);
		SetConsoleCP(CP_UTF8);

		isSetup = 1;
	}
#endif
}

// -------------------------------------------------------------------------- //

static AlfTestInt _alfStringLength(const char* string)
{
	AlfTestInt length = 0;
	while (string[length++] != 0) {}
	return length - 1;
}

// -------------------------------------------------------------------------- //

/** Return a copy of a nul-terminated c-string **/
static char* _alfTestStringCopy(const char* string)
{
	const size_t length = _alfStringLength(string);
	char* buffer = malloc(length + 1);
	if (!buffer) { return NULL;  }
	memcpy(buffer, string, length + 1);
	return buffer;
}

// -------------------------------------------------------------------------- //

/** Returns a relative time from high-performance timer **/
static AlfTestTime _alfHighPerformanceTimer()
{
#if defined(_WIN32)
	// Only query the performance counter frequency once
	static AlfTestTime frequency = 0;
	if (frequency == 0)
	{
		LARGE_INTEGER f;
		QueryPerformanceFrequency(&f);
		frequency = f.QuadPart;
	}

	// Query the performance counter
	LARGE_INTEGER c;
	QueryPerformanceCounter(&c);
	const AlfTestTime counter = c.QuadPart;

	// Convert counter to nanoseconds and return
	const double s = (double)counter / frequency;
	const AlfTestTime ns = (AlfTestTime)(s * 1e9);
	return ns;
#elif defined(__APPLE__)
	static mach_timebase_info_data_t timebaseInfo;
	if (timebaseInfo.denom == 0)
	{
		mach_timebase_info(&timebaseInfo);
	}
	AlfTestTime time = mach_absolute_time();
	return time * timebaseInfo.numer / timebaseInfo.denom;
#elif defined(__linux__)
        struct timespec time;
        clock_gettime(CLOCK_BOOTTIME, &time);
        const AlfTestTime ns = time.tv_sec * (AlfTestTime)1e9 + time.tv_nsec;
        return ns;
#else
	NOT_IMPLEMENTED
#endif
}

// -------------------------------------------------------------------------- //

/** Default suite setup function that does nothing **/
static void _alfDefaultSuiteSetup(AlfTestSuite* suite) {}

// -------------------------------------------------------------------------- //

/** Default suite teardown function that does nothing **/
static void _alfDefaultSuiteTeardown(AlfTestSuite* suite) {}

// -------------------------------------------------------------------------- //

/** Print about-text **/
static void _alfPrintAbout()
{
#if defined(ALF_TEST_PRINT_ABOUT)
	printf(
		"\n\t" ALF_TEST_CC_LOGO "AlfTest" ALF_TEST_CC_RESET 
		" is a unit testing library for " ALF_TEST_CC_C "C" ALF_TEST_CC_RESET 
		" and " ALF_TEST_CC_C "C++" ALF_TEST_CC_RESET " that is\n"
		"\teasy to embed into a program without the need to link a\n"
		"\tlibrary\n"
		"\n\t" ALF_TEST_CC_LOGO "Version" ALF_TEST_CC_RESET " - 0.1.0\n\n"
	);
#endif
}

// -------------------------------------------------------------------------- //

/** Internal check function **/
static void alfTestCheckInternal(
	AlfTestState* state, 
	AlfTestBool condition,
	const char* message, 
	const char* file, 
	AlfTestInt line,
	const char* explanation)
{
	state->count++;
	state->failCount += (condition ? 0 : 1);
	printf(
		"\t" ALF_TEST_CC_FILE "%s" ALF_TEST_CC_RESET ":" ALF_TEST_CC_LINE "%i" 
		ALF_TEST_CC_RESET ": %s%s" ALF_TEST_CC_RESET " - " ALF_TEST_CC_TYPE "%s" 
		ALF_TEST_CC_RESET "%s%s%s\n", 
		file, (int)line,
		condition ? ALF_TEST_CC_PASS : ALF_TEST_CC_FAIL,
		condition ? "PASS" : "FAIL",
		message,
		explanation ? " - \"" : "",
		explanation ? explanation : "",
		explanation ? "\"" : ""
	);
}

// -------------------------------------------------------------------------- //

static char* alfTestFormatString(const char* format, ...)
{
	va_list arguments;
	va_start(arguments, format);

	// Format string
	va_list copy;
	va_copy(copy, arguments);
	const int32_t writtenBytes = vsnprintf(
		sFormatBuffer, ALF_TEST_FORMAT_BUFFER_SIZE, format, arguments);
	va_end(copy);

	// Copy buffer
	char* result = malloc(writtenBytes + 1ull);
	if (!result) { return NULL; }
	memcpy(result, sFormatBuffer, writtenBytes);
	result[writtenBytes] = 0;

	va_end(arguments);
	return result;
}

// ========================================================================== //
// Function implementations
// ========================================================================== //

AlfTestSuite* alfCreateTestSuite(char* name, AlfTest* tests, AlfTestInt count)
{
	// Do required setup
	_alfSetupConsole();

	// Allocate suite
	AlfTestSuite* suite = malloc(sizeof(AlfTestSuite));
	if (!suite) { return NULL; }

	// Setup suite
	suite->name = _alfTestStringCopy(name);
	suite->Setup = _alfDefaultSuiteSetup;
	suite->Teardown = _alfDefaultSuiteTeardown;
	suite->state = (AlfTestState) { suite, 0, 0 };

	// Setup tests
	suite->testCount = count;
	suite->tests = malloc(sizeof(AlfTestInternal) * suite->testCount);
	if (!suite->tests)
	{
		free(suite);
		return NULL;
	}
	for (AlfTestInt i = 0; i < suite->testCount; i++)
	{
		AlfTestInternal* test = &suite->tests[i];
		test->name = _alfTestStringCopy(tests[i].name);
		test->TestFunction = tests[i].TestFunction;
	}

	return suite;
}

// -------------------------------------------------------------------------- //

void alfDestroyTestSuite(AlfTestSuite* suite)
{
	for (AlfTestInt i = 0; i < suite->testCount; i++)
	{
		free(suite->tests[i].name);
	}
	free(suite->tests);
	free(suite->name);
	free(suite);
}

// -------------------------------------------------------------------------- //

void alfSetSuiteUserData(AlfTestSuite* suite, void* data)
{
	suite->userData = data;
}

// -------------------------------------------------------------------------- //

void* alfGetSuiteUserData(AlfTestSuite* suite)
{
	return suite->userData;
}

// -------------------------------------------------------------------------- //

void* alfGetSuiteUserDataFromState(AlfTestState* state)
{
	return state->suite->userData;
}

// -------------------------------------------------------------------------- //

void alfSetSuiteSetupCallback(AlfTestSuite* suite, PFN_AlfSuiteSetup callback)
{
	suite->Setup = callback;
}

// -------------------------------------------------------------------------- //

void alfSetSuiteTeardownCallback(
	AlfTestSuite* suite, 
	PFN_AlfSuiteTeardown callback)
{
	suite->Teardown = callback;
}

// -------------------------------------------------------------------------- //

void alfClearSuiteSetupCallback(AlfTestSuite* suite)
{
	alfSetSuiteSetupCallback(suite, _alfDefaultSuiteSetup);
}

// -------------------------------------------------------------------------- //

void alfClearSuiteTeardownCallback(AlfTestSuite* suite)
{
	alfSetSuiteTeardownCallback(suite, _alfDefaultSuiteTeardown);
}

// -------------------------------------------------------------------------- //

AlfTestInt alfRunSuite(AlfTestSuite* suite)
{
	AlfTestSuite* suites[1];
	suites[0] = suite;
	return alfRunSuites(suites, 1);
}

// -------------------------------------------------------------------------- //

AlfTestInt alfRunSuites(AlfTestSuite** suites, AlfTestInt suiteCount)
{
	_alfPrintAbout();

	// Run all suites
	AlfTestInt totalCheckCount = 0;
	AlfTestInt failCheckCount = 0;
	AlfTestInt totalTestCount = 0;
	AlfTestInt failTestCount = 0;
	AlfTestInt failSuiteCount = 0;
	const AlfTestTime startTime = _alfHighPerformanceTimer();
	for (AlfTestInt suiteIndex = 0; suiteIndex < suiteCount; suiteIndex++)
	{
		AlfTestSuite* suite = suites[suiteIndex];
		suite->Setup(suite);
		printf(ALF_TEST_CC_SUITE "SUITE" ALF_TEST_CC_RESET " \"%s\"\n", 
			suite->name);

		// Run all tests in suite
		const AlfTestInt suiteFailTestCountBefore = failTestCount;
		const AlfTestTime suiteStartTime = _alfHighPerformanceTimer();
		for (AlfTestInt testIndex = 0; testIndex < suite->testCount; testIndex++)
		{
			// Clear state
			suite->state.count = 0;
			suite->state.failCount = 0;

			// Run test
			AlfTestInternal* test = &suite->tests[testIndex];
			printf("Running " ALF_TEST_CC_NAME "%s" ALF_TEST_CC_RESET ":\n", 
				test->name);
			const AlfTestTime timeBefore = _alfHighPerformanceTimer();
			test->TestFunction(&suite->state);
			const AlfTestTime timeDelta = _alfHighPerformanceTimer() - timeBefore;
			printf("\tTest finished in " ALF_TEST_CC_TIME "%.3f" 
				ALF_TEST_CC_RESET " ms\n", timeDelta / 1000000.0);

			// Update suite state
			totalTestCount++;
			totalCheckCount += suite->state.count;
			failCheckCount += suite->state.failCount;
			if (suite->state.failCount > 0) { failTestCount++; }
		}
		const AlfTestTime suiteElapsedTime = 
			_alfHighPerformanceTimer() - suiteStartTime;
		printf("Suite finished in " ALF_TEST_CC_TIME "%.3f" ALF_TEST_CC_RESET 
			" ms\n\n", suiteElapsedTime / 1000000.0);
		if (failTestCount - suiteFailTestCountBefore > 0) { failSuiteCount++; }
		suite->Teardown(suite);
	}
	const AlfTestTime totalTime = _alfHighPerformanceTimer() - startTime;

	// Print summary and return
	const AlfTestInt passSuiteCount = suiteCount - failSuiteCount;
	const AlfTestInt passCheckCount = totalCheckCount - failCheckCount;
	const AlfTestInt passTestCount = totalTestCount - failTestCount;
	printf(ALF_TEST_CC_SUITE "SUMMARY\n" ALF_TEST_CC_RESET);
	printf("Type\t\tTotal\t\tPass\t\tFail\n");
	printf("Suite\t\t%i\t\t%i\t\t%i\n", 
		(int32_t)suiteCount, (int32_t)passSuiteCount, (int32_t)failSuiteCount);
	printf("Test\t\t%i\t\t%i\t\t%i\n", 
		(int32_t)totalTestCount, (int32_t)passTestCount, (int32_t)failTestCount);
	printf("Check\t\t%i\t\t%i\t\t%i\n", 
		(int32_t)totalCheckCount, (int32_t)passCheckCount, (int32_t)failCheckCount);
	printf("Run completed in " ALF_TEST_CC_TIME "%f" ALF_TEST_CC_RESET " ms\n", 
		totalTime / 1000000.0);

	// Print final result
	if (failTestCount == 0)
	{
		printf(ALF_TEST_CC_PASS "ALL TESTS PASSED " ALF_TEST_CC_RESET "\n");
	}
	else
	{
		printf(ALF_TEST_CC_FAIL "SOME TESTS FAILED" ALF_TEST_CC_RESET "\n");
	}

	// Return fail count
	return failTestCount;
}

// -------------------------------------------------------------------------- //

void alfCheckTrue(
	AlfTestState* state,
	AlfTestBool predicate,
	const char* predicateString,
	const char* file,
	AlfTestInt line,
	AlfTestCheckParameters parameters)
{
	char* message = alfTestFormatString(
		"TRUE(%s)",
		predicateString
	);
	alfTestCheckInternal(
		state,
		predicate,
		message,
		file,
		line,
		parameters.reason
	);
	free(message);
}

// -------------------------------------------------------------------------- //

void alfCheckFalse(
	AlfTestState* state,
	AlfTestBool predicate,
	const char* predicateString,
	const char* file,
	AlfTestInt line,
	AlfTestCheckParameters parameters)
{
	char* message = alfTestFormatString(
		"FALSE(%s)",
		predicateString
	);
	alfTestCheckInternal(
		state,
		predicate,
		message,
		file,
		line,
		parameters.reason
	);
	free(message);
}

// -------------------------------------------------------------------------- //

void alfCheckNotNull(
	AlfTestState* state,
	void* pointer,
	const char* pointerText,
	const char* file,
	AlfTestInt line,
	AlfTestCheckParameters parameters)
{
	char* message = alfTestFormatString(
		"NOT_NULL(%s (0x%p))",
		pointerText, pointer
	);
	alfTestCheckInternal(
		state,
		pointer != NULL,
		message,
		file,
		line,
		parameters.reason
	);
	free(message);
}

// -------------------------------------------------------------------------- //

void alfCheckNull(
	AlfTestState* state,
	void* pointer,
	const char* pointerText,
	const char* file,
	AlfTestInt line,
	AlfTestCheckParameters parameters)
{
	char* message = alfTestFormatString(
		"NULL(%s (0x%p))",
		pointerText, pointer
	);
	alfTestCheckInternal(
		state,
		pointer == NULL,
		message,
		file,
		line,
		parameters.reason
	);
	free(message);
}

// -------------------------------------------------------------------------- //

void alfCheckMemEq(
	AlfTestState* state,
	const void* m0,
	const void* m1,
	const char* var0,
	const char* var1,
	AlfTestSize size,
	const char* file,
	AlfTestInt line,
	AlfTestCheckParameters parameters)
{
	char* message = alfTestFormatString(
		"MEM_EQ(%s == %s)",
		m0,
		m0
	);
	const AlfTestInt predicate =
		(m0 == NULL && m1 == NULL) ||
		(m0 && m1 && memcmp(m0, m1, size) == 0);
	alfTestCheckInternal(
		state,
		predicate,
		message,
		file,
		line,
		parameters.reason
	);
	free(message);
}

// -------------------------------------------------------------------------- //

void alfCheckStrEq(
	AlfTestState* state,
	const char* str0,
	const char* str1,
	const char* var0,
	const char* var1,
	const char* file,
	AlfTestInt line,
	AlfTestCheckParameters parameters)
{
	char* message = alfTestFormatString(
		"STR_EQ(%s == %s)",
		str0,
		str1
	);
	const AlfTestInt predicate =
		(str0 == NULL && str1 == NULL) ||
		(str0 && str1 && strcmp(str0, str1) == 0);
	alfTestCheckInternal(
		state, 
		predicate,
		message, 
		file, 
		line, 
		parameters.reason
	);
	free(message);
}

// -------------------------------------------------------------------------- //

void alfCheckFloatEq(
	AlfTestState* state,
	const float* float0,
	const float* float1,
	const char* var0,
	const char* var1,
	const char* file,
	AlfTestInt line,
	AlfTestCheckParameters parameters)
{
	char* message = alfTestFormatString(
		"FLOAT_EQ(%f == %f)",
		float0,
		float1
	);
	const AlfTestInt predicate =
		(float0 - float1) <= ALF_TEST_FLOAT_EPSILON;
	alfTestCheckInternal(
		state,
		predicate,
		message,
		file,
		line,
		parameters.reason
	);
	free(message);
}

// -------------------------------------------------------------------------- //

void alfCheckDoubleEq(
	AlfTestState* state,
	const double* double0,
	const double* double1,
	const char* var0,
	const char* var1,
	const char* file,
	AlfTestInt line,
	AlfTestCheckParameters parameters)
{
	char* message = alfTestFormatString(
		"DOUBLE_EQ(%f == %f)",
		double0,
		double1
	);
	const AlfTestInt predicate =
		(double0 - double1) <= ALF_TEST_DOUBLE_EPSILON;
	alfTestCheckInternal(
		state,
		predicate,
		message,
		file,
		line,
		parameters.reason
	);
	free(message);
}

// -------------------------------------------------------------------------- //

char* alfLastIndexOf(char* string, char character)
{
	AlfTestInt length = _alfStringLength(string);
	while (length > 0)
	{
		if (string[length] == character)
		{
			return string + length;
		}
		length--;
	}
	return string;
}

// ========================================================================== //
// End of Implementation
// ========================================================================== //
