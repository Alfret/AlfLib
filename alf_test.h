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

#ifndef ALF_TEST_H
#define ALF_TEST_H

#if defined(__cplusplus)
extern "C" {
#endif

// ========================================================================== //
// Usage
// ========================================================================== //

/* INTRODUCTION:
 * This library is made to be easy to use, this means that there is no linking 
 * involed. 
 * 
 * FEATURES:
 * The AlfTest supports an array of features that are useful primarily for unit
 * testing but also in the future for other types of testing.
 * 
 * - Unit testing
 * - Suite based testing
 * - Wide array of checking (assert) functions
 * - Timing data for suites, tests and overall
 * - Colorized output with multiple predefined or custom themes
 * - No dependency on standard library if configurated
 * 
 * USAGE:
 * To use it simply drop the "alf_test.h" file into your project. Then in ONLY 
 * ONE source file define the macro "ALF_TEST_IMPL". This will create the 
 * implementation in that file. Doing this in multiple files will result in 
 * multiply defined symbols. To use the header from other files simply include
 * the header without defining the macro.
 *
 * CONFIG:
 * There are also some optional macros that can be defined to alter the 
 * behaviour of the implementation.
 * - ALF_TEST_CUSTOM_ALLOC: Defining this macro signals the implementation that
 *		the user wants to supply its own allocation functions. The user then 
 *		needs to define the macro ALF_TEST_ALLOC and ALF_TEST_FREE. The alloc
 *		function takes the size of the allocation being made while the free 
 *		function takes the memory being freed.
 *		If this macro is not defined then malloc and free will be used for the
 *		implementation.
 * - ALF_TEST_ASSERT: Defining this macro signals the implementation that the 
 *		user wants to supply its own assertion function for internal assertions.
 *		Note that this is not the assertions used for the actual testing. The 
 *		ALF_TEST_ASSERT macro takes two arguments, the first being the condition
 *		and the second a message.
 *		If this macro is not defined then the assert function from the standard
 *		library will be used. The message is separated from the condition with a
 *		logical and (&&).
 *	- ALF_TEST_PRINTF: Defining this macro signals the implementation that the
 *		user wants to supply its own printf-like function that is used for 
 *		printing the output of the tests. This macro expects a format string and
 *		a list of arguments to the format.
 *	- ALF_TEST_NO_TRUECOLOR: Defining this macro signals the implementation that
 *		it should not use truecolor ansi escape sequences for colorization of 
 *		the printed information. This will then fallback to using 8-bit 
 *		escape-sequence colors instead. The macro ALF_THEME_NONE can be used to
 *		disable colorization altogether.
 * - ALF_THEME_NONE: Defining this macro signals the implementation to not use
 *		any colorization at all for the printed output. It will instead be 
 *		printed to the standard output without any formatting.
 * - ALF_THEME_*: This is a family of macro that can be defined to alter the 
 *		color theme of the implementation. The themes might look different when
 *		truecolor support has been disabled as all colors are not available 
 *		then.
 *		The star in the macro is supposed to be one of the following themes:
 *		- PALE: Pale colors.
 *		- AUTUMN_FRUIT: Purple and orange colors.
 *		If no theme macro is defined then the default color theme is used. 
 * - ALF_THEME_PRINT_ABOUT: Defining this macro will have the implementation 
 *		print information about the library before running any tests. This is 
 *		off by default as to not clutter the information being printed.
 *
 * The user can also create custom themes. This is done by modifying the 
 * implementation slightly to include a custom theme. The user should use the 
 * template that is found in the theme declaration.
 *
 * THEORY:
 * The first thing to do when setting up tests is to define the function that 
 * should be run as part of the test. These function has to have the prototype:
 * "void name(AlfTestState* state)", where the name can be anything.
 * These test are then collected in an array and specified as the argument, 
 * together with the count, during creation of the suite.
 * 
 * Suites are created with "alfCreateTestSuite". This function takes the name of
 * the suite, the array of tests and a count for the number of tests in the 
 * array. When the user is done with the suite it should call 
 * "alfDestroyTestSuite" to free all memory that it's using. 
 * 
 * The user may optionally call some functions for setting user data or callback
 * for the suite before running any tests. The user data is retrievable from any
 * test and can therefore be used to store data that a test must be able to use.
 * The callbacks that can be set is a setup and a teardown function that is 
 * called before and after the suite is run.
 * 
 * When the user has created a suite it's time to one of the multiple run 
 * functions available. For running a single suite "alfRunSuite" should be used.
 * For instead running multiple suites at the same time "alfRunSuites" should be 
 * used. Of course "alfRunSuites" can also be used to run a single suite, 
 * however the information presented in the summary will slightly differ.
 *
 * During the tests the user calls one of the many "check" functions (in other
 * implementations called 'assert') to check/assert that some condition is true.
 * The possible alternatives are:
 * - alfCheckTrue: For checking that a condition is true.
 * - alfCheckFalse: For checking that a condition is false.
 * - alfCheckNotNull: For checking that a pointer is not NULL.
 * - alfCheckNull: For checking that a pointer is NULL.
 * - alfCheckMemEq: For checking that two regions of memory contains the same 
 *		data.
 * - alfCheckStrEq: For checking that two nul-terminated c-strings are equal.
 * - alfCheckFloatEq: For checking that two single-precision floating-point 
 *		numbers are equal. This takes into account the machine epsilon.
 * - alfCheckDoubleEq: For checking that two double-precision floating-point 
 *		numbers are equal. This takes into account the machine epsilon.
 *
 * Each of the check functions also has a corresponding version with 'M' 
 * appended to the name. These alternative functions also accepts a final 
 * argument that is supposed to represent the explantion for or the reason 
 * behind the test. This can be used to describe WHY the test is done and/or 
 * WHAT it accomplishes.
 *
 * EXAMPLE:
 * Here follows a simple example of how to setup a suite to run 3 different 
 * tests. The number of fails from running the test is then returned. The test
 * also sets the theme to "pale".
 *
 * !Note that the test in the example will fail because 3.14 != 3.15.
 *
 * -------------------------------------------------------------------------- *
 * 
 * #define ALF_TEST_IMPL
 * #define ALF_TEST_THEME_PALE
 * #include "alf_test.h"
 * 
 * void test1(AlfTestState* state)
 * {
 *     alfCheckFalse(state, 2 == 3);	
 * 	   alfCheckTrueM(state, 3 == 3, "3 must equal 3");
 * }
 * 
 * void test2(AlfTestState* state)
 * {
 *     alfCheckStrEq(state, "a string", "a string");
 *     alfCheckNotNull(state, (void*)1245);
 * }
 * 
 * void test3(AlfTestState* state)
 * {
 * 	   alfCheckNull(state, NULL);
 * 	   alfCheckFloatEq(state, 3.14f, 3.15f);
 * }
 *
 * int main()
 * {
 *     AlfTest tests[3];
 *     tests[0] = (AlfTest) { .name = "Test1", .TestFunction = test1 };
 *     tests[1] = (AlfTest) { .name = "Test2", .TestFunction = test2 };
 *     tests[2] = (AlfTest) { .name = "Test3", .TestFunction = test3 };
 *     AlfTestSuite* suite = alfCreateTestSuite("Demo Suite", tests, 3);
 * 
 *     const uint32_t fails = alfRunSuite(suite);
 *     alfDestroyTestSuite(suite);
 * 	   return fails;
 * }
 *
 * -------------------------------------------------------------------------- *
 * 
 * END:
 * For more information visit the github repository: TODO(Filip Björklund)
 */

// ========================================================================== //
// Type definitions
// ========================================================================== //

/** Test function **/
typedef void(*PFN_Test)(struct tag_AlfTestState* state);

// -------------------------------------------------------------------------- //

/** Suite setup function **/
typedef void(*PFN_SuiteSetup)(struct tag_AlfTestSuite* suite);

// -------------------------------------------------------------------------- //

/** Suite teardown function **/
typedef PFN_SuiteSetup PFN_SuiteTeardown;

// ========================================================================== //
// Macro declarations
// ========================================================================== //

// Allocation macros
#if !defined(ALF_TEST_CUSTOM_ALLOC)
#	include <stdlib.h>
#	define ALF_TEST_ALLOC(size) malloc(size)
#	define ALF_TEST_FREE(mem) free(mem)
#endif

// Assert macros
#if !defined(ALF_TEST_ASSERT)
#	include <assert.h>
	/** Internal assertion macro. NOT to be used for asserting in unit tests. Use alfCheck instead **/
#	define ALF_TEST_ASSERT(cond, msg) assert(cond && msg)
#endif 

// Print macro
#if !defined(ALF_TEST_PRINTF)
#	include <stdio.h>
#	define ALF_TEST_PRINTF(format, ...) printf(format, ##__VA_ARGS__)
#endif

// Filename macro
#if defined(_WIN32)
	/** Macro for name of current file **/
#	define __FILENAME__ (alfLastIndexOf(__FILE__, '\\') ? alfLastIndexOf(__FILE__, '\\') + 1 : __FILE__)
#else
	/** Macro for name of current file **/
#	define __FILENAME__ (alfLastIndexOf(__FILE__, '/') ? alfLastIndexOf(__FILE__, '/') + 1 : __FILE__)
#endif

/** Float epsilon **/
#define ALF_FLOAT_EPSILON (1.19e-07)
/** Double epsilon **/
#define ALF_DOUBLE_EPSILON (2.22e-16)

// ========================================================================== //
// Macros for checking
// ========================================================================== //

/** Check that condition is true **/
#define alfCheckTrue(state, condition) \
	alfCheck(state, condition, "TRUE(" #condition ")", __FILENAME__, __LINE__)
/** Check that condition is false **/
#define alfCheckFalse(state, condition) \
	alfCheck(state, !(condition), "FALSE(" #condition ")", __FILENAME__, __LINE__)
/** Check that condition is true. With explanation **/
#define alfCheckTrueM(state, condition, explanation) \
	alfCheckM(state, condition, "TRUE(" #condition ")", __FILENAME__, __LINE__, explanation)
/** Check that condition is false. With explanation **/
#define alfCheckFalseM(state, condition, explanation) \
	alfCheckM(state, !(condition), "FALSE(" #condition ")", __FILENAME__, __LINE__, explanation)

/** Check that a pointer is not NULL **/
#define alfCheckNotNull(state, pointer)	\
	alfCheck(state, (pointer) != NULL, "NOTNULL(" #pointer ")", __FILENAME__, __LINE__)
/** Check that a pointer is not NULL **/
#define alfCheckNotNullM(state, pointer, explanation)	\
	alfCheckM(state, (pointer) != NULL, "NOTNULL(" #pointer ")", __FILENAME__, __LINE__, explanation)
/** Check that a pointer is NULL **/
#define alfCheckNull(state, pointer)	\
	alfCheck(state, (pointer) == NULL, "NULL(" #pointer ")", __FILENAME__, __LINE__)
/** Check that a pointer is NULL **/
#define alfCheckNullM(state, pointer, explanation)	\
	alfCheckM(state, (pointer) == NULL, "NULL(" #pointer ")", __FILENAME__, __LINE__, explanation)

/** Check that two memory regions contains the same data **/
#define alfCheckMemEq(state, m0, m1, size)	\
	alfCheck(state, memcmp(m0, m1, size) == 0, "MEMEQ(" #m0 " == " #m1 ", size: " #size ")", __FILENAME__, __LINE__)
/** Check that two memory regions contains the same data. With explanation **/
#define alfCheckMemEqM(state, m0, m1, size, explanation)	\
	alfCheck(state, memcmp(m0, m1, size) == 0, "MEMEQ(" #m0 " == " #m1 ", size: " #size ")", __FILENAME__, __LINE__, explanation)

/** Check that two nul-terminated c-strings are equal **/
#define alfCheckStrEq(state, str0, str1)	\
	alfCheck(state, str0 && str1 && strcmp(str0, str1) == 0, "STREQ(" #str0 " == " #str1 ")", __FILENAME__, __LINE__)
/** Check that two nul-terminated c-strings are equal. With explanation **/
#define alfCheckStrEqM(state, str0, str1, explanation)	\
	alfCheckM(state, str0 && str1 && strcmp(str0, str1) == 0, "STREQ(" #str0 " == " #str1 ")", __FILENAME__, __LINE__, explanation)

/** Check that two single-precision floating-point numbers are equal **/
#define alfCheckFloatEq(state, float0, float1)											\
	alfCheck(																			\
		state,																			\
		(float0 - float1 < ALF_FLOAT_EPSILON) && (float1 - float0 < ALF_FLOAT_EPSILON),	\
		"FLOATEQ(" #float0 " == " #float1 ")",											\
		__FILENAME__, __LINE__															\
	)
/** Check that two single-precision floating-point numbers are equal. With explanation **/
#define alfCheckFloatEqM(state, float0, float1, explanation)								\
	alfCheckM(																			\
		state,																			\
		(float0 - float1 < ALF_FLOAT_EPSILON) && (float1 - float0 < ALF_FLOAT_EPSILON),	\
		"FLOATEQ(" #float0 " == " #float1 ")",											\
		__FILENAME__, __LINE__, explanation												\
	)

/** Check that two double-precision floating-point numbers are equal **/
#define alfCheckDoubleEq(state, double0, double1)												\
	alfCheck(																					\
		state,																					\
		(double0 - double1 < ALF_DOUBLE_EPSILON) && (double1 - double0 < ALF_DOUBLE_EPSILON),	\
		"DOUBLEEQ(" #double0 " == " #double1 ")",												\
		__FILENAME__, __LINE__																	\
	)
/** Check that two double-precision floating-point numbers are equal. With explanation **/
#define alfCheckDoubleEqM(state, double0, double1, explanation)									\
	alfCheckM(																					\
		state,																					\
		(double0 - double1 < ALF_DOUBLE_EPSILON) && (double1 - double0 < ALF_DOUBLE_EPSILON),	\
		"DOUBLEEQ(" #double0 " == " #double1 ")",												\
		__FILENAME__, __LINE__, explanation														\
	)

// ========================================================================== //
// Type definitions
// ========================================================================== //

/** Standard integer type used **/
typedef int INT_TYPE;
/** Integer type used for time **/
typedef unsigned long long TIME_TYPE;

// ========================================================================== //
// Structures
// ========================================================================== //

/** \struct AlfTestSuite
 * \author Filip Björklund
 * \date 29 september 2018 - 13:35
 * \brief Test suite.
 * \details
 * Represents a test suite which is a collection of tests.
 */
typedef struct tag_AlfTestSuite AlfTestSuite;

// -------------------------------------------------------------------------- //

/** \struct AlfTestState
 * \author Filip Björklund
 * \date 29 september 2018 - 13:35
 * \brief Test state.
 * \details
 * Represents the state during testing. An object of this type is the argument
 * of each test function.
 */
typedef struct tag_AlfTestState AlfTestState;

// -------------------------------------------------------------------------- //

/** \struct AlfTest
 * \author Filip Björklund
 * \date 29 september 2018 - 13:44
 * \brief Test.
 * \details
 * Represents a single test to be run as part of a test suite.
 */
typedef struct AlfTest
{
	/** Name of test **/
	char* name;
	/** Test function **/
	PFN_Test TestFunction;
} AlfTest;

// ========================================================================== //
// Functions
// ========================================================================== //

/** Create a test suite with a specified set of tests.
 * \brief Create test suite.
 * \note No transfer of ownership of dynamic resources is done. It's therefore
 * up to the user to free any data that it has allocated.
 * \param name Name of the test suite.
 * \param tests Tests that belong to suite.
 * \param count Number of tests.
 * \return Created test suite.
 */
AlfTestSuite* alfCreateTestSuite(char* name, AlfTest* tests, INT_TYPE count);

// -------------------------------------------------------------------------- //

/** Delete a test suite that was previously created with alfCreateTestSuite.
 * \brief Delete test suite.
 * \param suite Test suite to delete.
 */
void alfDestroyTestSuite(AlfTestSuite* suite);

// -------------------------------------------------------------------------- //

/** Run all the tests of a single test suite.
 * \brief Run test suite.
 * \param suite Suite to run.
 * \return Number of failed tests.
 */
INT_TYPE alfRunSuite(AlfTestSuite* suite);

// -------------------------------------------------------------------------- //

/** Run all the tests for a set of suites. The test are run in the order 
 * specified and a summary is displayed to the user.
 * \brief Run a set of test suites.
 * \param suites Suites to run.
 * \param suiteCount Number of suites.
 * \return Number of failed checks in total in all tests.
 */
INT_TYPE alfRunSuites(AlfTestSuite** suites, INT_TYPE suiteCount);

// -------------------------------------------------------------------------- //

/** Set the user data of a test suite. This can contain any type of data that 
 * the user might want to access during a test.
 * \brief Set suite user data.
 * \param suite Suite to set user data for.
 * \param data User data to set.
 */
void alfSetSuiteUserData(AlfTestSuite* suite, void* data);

// -------------------------------------------------------------------------- //

/** Returns the user data of a test suite.
 * \brief Returns test suite user data.
 * \param suite Suite to return user data of.
 * \return User data. NULL if no data has been set.
 */
void* alfGetSuiteUserData(AlfTestSuite* suite);

// -------------------------------------------------------------------------- //

/** Returns the user data of a test suite from a test state.
 * \brief Returns test suite user data from state.
 * \param state State to retrieve suite user data from.
 * \return User data. NULL if no data has been set.
 */
void* alfGetSuiteUserDataFromState(AlfTestState* state);

// -------------------------------------------------------------------------- //

/** Set the callback that will be called for a suite to setup. This is called 
 * before the suite is run.
 * \note By default a blank function is called for setup.
 * \brief Set suite setup callback.
 * \param suite Suite to set callback for.
 * \param callback Callback to set.
 */
void alfSetSuiteSetupCallback(AlfTestSuite* suite, PFN_SuiteSetup callback);

// -------------------------------------------------------------------------- //

/** Set the callback that will be called for a suite to teardown. This is called
 * after all the tests of the suite has run.
 * \brief Set suite teardowns callback.
 * \param suite Suite to set callback for.
 * \param callback Callback to set.
 */
void alfSetSuiteTeardownCallback(AlfTestSuite* suite, PFN_SuiteTeardown callback);

// -------------------------------------------------------------------------- //

/** Clear the setup callback for a suite.
 * \brief Clear suite setup callback.
 * \param suite Suite to clear callback for.
 */
void alfClearSuiteSetupCallback(AlfTestSuite* suite);

// -------------------------------------------------------------------------- //

/** Clear the teardown callback for a suite.
 * \brief Clear suite teardown callback.
 * \param suite Suite to clear callback for.
 */
void alfClearSuiteTeardownCallback(AlfTestSuite* suite);

// -------------------------------------------------------------------------- //

/** Function to do a check (assertion) during a test. This should however not be
 * used directly. Instead use the check macros.
 * \brief Function for checking during test.
 * \param state Test state.
 * \param condition Condition to check.
 * \param conditionText Condition in text form.
 * \param file File in which check is done.
 * \param line Line in file.
 */
void alfCheck(AlfTestState* state, INT_TYPE condition, char* conditionText, char* file, INT_TYPE line);

// -------------------------------------------------------------------------- //

/** Function to do a check (assertion) during a test. This should however not be
 * used directly. Instead use the check macros. The explanation should be used 
 * to explanation why the test is done.
 * \brief Function for checking during test.
 * \param state Test state.
 * \param condition Condition to check.
 * \param conditionText Condition in text form.
 * \param file File in which check is done.
 * \param line Line in file.
 */
void alfCheckM(AlfTestState* state, INT_TYPE condition, char* conditionText, char* file, INT_TYPE line, char* explanation);

// -------------------------------------------------------------------------- //

char* alfLastIndexOf(char* string, char character);

// ========================================================================== //
// HERE STARTS THE IMPLEMENTATION
// ========================================================================== //

#if defined(ALF_TEST_IMPL)

// ========================================================================== //
// Header includes
// ========================================================================== //

// Windows headers
#if defined(_WIN32)
#	include <Windows.h>
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
#if defined(ALF_TEST_NO_TRUECOLOR)
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
#endif defined(ALF_TEST_NO_TRUECOLOR)

// Library name color
#if !defined(ALF_THEME_NONE)
#	define ALF_TEST_CC_LOGO "\033[38;5;112m"
#	define ALF_TEST_CC_C "\033[38;5;45m"
#endif

// ========================================================================== //
// Private structures
// ========================================================================== //

/** Private test-state structure **/
typedef struct tag_AlfTestState
{
	/** Suite that state represents **/
	AlfTestSuite* suite;
	/** Total check count **/
	INT_TYPE count;
	/** Failed check count **/
	INT_TYPE failCount;
} tag_AlfTestState;

// -------------------------------------------------------------------------- //

/** Private test-suite structure **/
typedef struct tag_AlfTestSuite
{
	/** Setup function **/
	PFN_SuiteSetup Setup;
	/** Teardown function **/
	PFN_SuiteTeardown Teardown;

	/** Name of test suite **/
	char* name;
	/** User data **/
	void* userData;

	/** State for use during tests **/
	AlfTestState state;

	/** Test count **/
	INT_TYPE testCount;
	/** Tests**/
	AlfTest* tests;
} tag_AlfTestSuite;

// ========================================================================== //
// Private function implementations
// ========================================================================== //

/** Setup console for platforms that does not support escape sequences out of 
 * the box
 */
static void _alfSetupConsoleMode()
{
#if defined(_WIN32)
	static INT_TYPE isSetup = 0;
	if (!isSetup)
	{
		// Enable virtual terminal processing for Color support
		DWORD mode;
		const HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (outputHandle)
		{
			BOOL success = GetConsoleMode(outputHandle, &mode);
			ALF_TEST_ASSERT(success, "Failet to get Windows console mode");
			success = SetConsoleMode(outputHandle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
			ALF_TEST_ASSERT(success, "Failed to set Windows console mode");
		}
		isSetup = 1;
	}
#endif
}

// -------------------------------------------------------------------------- //

static INT_TYPE _alfStringLength(const char* string)
{
	INT_TYPE length = 0;
	while (string[length++] != 0) {}
	return length - 1;
}

// -------------------------------------------------------------------------- //

/** Return a copy of a nul-terminated c-string **/
static char* _alfStringCopy(char* string)
{
	const size_t length = _alfStringLength(string);
	char* buffer = ALF_TEST_ALLOC(length + 1);
	memcpy(buffer, string, length + 1);
	return buffer;
}

// -------------------------------------------------------------------------- //

/** Returns a relative time from high-performance timer **/
static TIME_TYPE _alfHighPerformanceTimer()
{
#if defined(_WIN32)
	// Only query the performance counter frequency once
	static TIME_TYPE frequency = 0;
	if (frequency == 0)
	{
		LARGE_INTEGER f;
		QueryPerformanceFrequency(&f);
		frequency = f.QuadPart;
	}

	// Query the performance counter
	LARGE_INTEGER c;
	QueryPerformanceCounter(&c);
	const TIME_TYPE counter = c.QuadPart;

	// Convert counter to nanoseconds and return
	const double s = (double)counter / frequency;
	const TIME_TYPE ns = (TIME_TYPE)(s * 1e9);
	return ns;
#elif defined(__APPLE__)
	static mach_timebase_info_data_t timebaseInfo;
	if (timebaseInfo.denom == 0)
	{
		mach_timebase_info(&timebaseInfo);
	}
	TIME_TYPE time = mach_absolute_time();
	return time * timebaseInfo.numer / timebaseInfo.denom;
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
		"\n\t" ALF_TEST_CC_LOGO "AlfTest" ALF_TEST_CC_RESET " is a unit testing library for " 
		ALF_TEST_CC_C "C" ALF_TEST_CC_RESET " and " ALF_TEST_CC_C "C++" ALF_TEST_CC_RESET " that is\n"
		"\teasy to embed into a program without the need to link a\n"
		"\tlibrary\n"
		"\n\t" ALF_TEST_CC_LOGO "Version" ALF_TEST_CC_RESET " - 0.1.0\n\n"
	);
#endif
}

// ========================================================================== //
// Function implementations
// ========================================================================== //

inline AlfTestSuite* alfCreateTestSuite(char* name, AlfTest* tests, INT_TYPE count)
{
	// Do required setup
	_alfSetupConsoleMode();

	// Allocate suite
	AlfTestSuite* suite = ALF_TEST_ALLOC(sizeof(AlfTestSuite));
	if (!suite) { return NULL; }

	// Setup suite
	suite->name = _alfStringCopy(name);
	suite->Setup = _alfDefaultSuiteSetup;
	suite->Teardown = _alfDefaultSuiteTeardown;
	suite->state = (AlfTestState) { suite, 0, 0 };

	// Setup tests
	suite->testCount = count;
	suite->tests = ALF_TEST_ALLOC(sizeof(AlfTest) * suite->testCount);
	for (INT_TYPE i = 0; i < suite->testCount; i++)
	{
		suite->tests[i].name = _alfStringCopy(tests[i].name);
		suite->tests[i].TestFunction = tests[i].TestFunction;
	}

	return suite;
}

// -------------------------------------------------------------------------- //

inline void alfDestroyTestSuite(AlfTestSuite* suite)
{
	for (INT_TYPE i = 0; i < suite->testCount; i++)
	{
		ALF_TEST_FREE(suite->tests[i].name);
	}
	ALF_TEST_FREE(suite->tests);
	ALF_TEST_FREE(suite->name);
	ALF_TEST_FREE(suite);
}

// -------------------------------------------------------------------------- //

inline void alfSetSuiteUserData(AlfTestSuite* suite, void* data)
{
	suite->userData = data;
}

// -------------------------------------------------------------------------- //

inline void* alfGetSuiteUserData(AlfTestSuite* suite)
{
	return suite->userData;
}

// -------------------------------------------------------------------------- //

inline void* alfGetSuiteUserDataFromState(AlfTestState* state)
{
	return state->suite->userData;
}

// -------------------------------------------------------------------------- //

inline void alfSetSuiteSetupCallback(AlfTestSuite* suite, PFN_SuiteSetup callback)
{
	suite->Setup = callback;
}

// -------------------------------------------------------------------------- //

inline void alfSetSuiteTeardownCallback(AlfTestSuite* suite, PFN_SuiteTeardown callback)
{
	suite->Teardown = callback;
}

// -------------------------------------------------------------------------- //

inline void alfClearSuiteSetupCallback(AlfTestSuite* suite)
{
	alfSetSuiteSetupCallback(suite, _alfDefaultSuiteSetup);
}

// -------------------------------------------------------------------------- //

inline void alfClearSuiteTeardownCallback(AlfTestSuite* suite)
{
	alfSetSuiteTeardownCallback(suite, _alfDefaultSuiteTeardown);
}

// -------------------------------------------------------------------------- //

inline INT_TYPE alfRunSuite(AlfTestSuite* suite)
{
	AlfTestSuite* suites[1];
	suites[0] = suite;
	return alfRunSuites(suites, 1);
}

// -------------------------------------------------------------------------- //

inline INT_TYPE alfRunSuites(AlfTestSuite** suites, INT_TYPE suiteCount)
{
	_alfPrintAbout();

	// Run all suites
	INT_TYPE totalCheckCount = 0;
	INT_TYPE failCheckCount = 0;
	INT_TYPE totalTestCount = 0;
	INT_TYPE failTestCount = 0;
	INT_TYPE failSuiteCount = 0;
	const TIME_TYPE startTime = _alfHighPerformanceTimer();
	for (INT_TYPE suiteIndex = 0; suiteIndex < suiteCount; suiteIndex++)
	{
		AlfTestSuite* suite = suites[suiteIndex];
		suite->Setup(suite);
		ALF_TEST_PRINTF(ALF_TEST_CC_SUITE "SUITE" ALF_TEST_CC_RESET " \"%s\"\n", suite->name);

		// Run all tests in suite
		const INT_TYPE suiteFailTestCountBefore = failTestCount;
		const TIME_TYPE suiteStartTime = _alfHighPerformanceTimer();
		for (INT_TYPE testIndex = 0; testIndex < suite->testCount; testIndex++)
		{
			// Clear state
			suite->state.count = 0;
			suite->state.failCount = 0;

			// Run test
			AlfTest* test = &suite->tests[testIndex];
			ALF_TEST_PRINTF("Running " ALF_TEST_CC_NAME "%s" ALF_TEST_CC_RESET ":\n", test->name);
			const TIME_TYPE timeBefore = _alfHighPerformanceTimer();
			test->TestFunction(&suite->state);
			const TIME_TYPE timeDelta = _alfHighPerformanceTimer() - timeBefore;
			ALF_TEST_PRINTF("\tTest finished in " ALF_TEST_CC_TIME "%.3f" ALF_TEST_CC_RESET " ms\n", timeDelta / 1000000.0);

			// Update suite state
			totalTestCount++;
			totalCheckCount += suite->state.count;
			failCheckCount += suite->state.failCount;
			if (suite->state.failCount > 0) { failTestCount++; }
		}
		const TIME_TYPE suiteElapsedTime = _alfHighPerformanceTimer() - suiteStartTime;
		ALF_TEST_PRINTF("Suite finished in " ALF_TEST_CC_TIME "%.3f" ALF_TEST_CC_RESET " ms\n\n", suiteElapsedTime / 1000000.0);
		if (failTestCount - suiteFailTestCountBefore > 0) { failSuiteCount++; }
		suite->Teardown(suite);
	}
	const TIME_TYPE totalTime = _alfHighPerformanceTimer() - startTime;

	// Print summary and return
	const INT_TYPE passSuiteCount = suiteCount - failSuiteCount;
	const INT_TYPE passCheckCount = totalCheckCount - failCheckCount;
	const INT_TYPE passTestCount = totalTestCount - failTestCount;
	ALF_TEST_PRINTF(ALF_TEST_CC_SUITE "SUMMARY\n" ALF_TEST_CC_RESET);
	ALF_TEST_PRINTF("Type\t\tTotal\t\tPass\t\tFail\n");
	ALF_TEST_PRINTF("Suite\t\t%i\t\t%i\t\t%i\n", suiteCount, passSuiteCount, failSuiteCount);
	ALF_TEST_PRINTF("Test\t\t%i\t\t%i\t\t%i\n", totalTestCount, passTestCount, failTestCount);
	ALF_TEST_PRINTF("Check\t\t%i\t\t%i\t\t%i\n", totalCheckCount, passCheckCount, failCheckCount);
	ALF_TEST_PRINTF("Run completed in " ALF_TEST_CC_TIME "%f" ALF_TEST_CC_RESET " ms\n", totalTime / 1000000.0);

	// Print final result
	if (failTestCount == 0) { ALF_TEST_PRINTF(ALF_TEST_CC_PASS "ALL TESTS PASSED " ALF_TEST_CC_RESET "\n"); }
	else { ALF_TEST_PRINTF(ALF_TEST_CC_FAIL "SOME TESTS FAILED" ALF_TEST_CC_RESET "\n"); }

	// Return fail count
	return failTestCount;
}

// -------------------------------------------------------------------------- //

inline void alfCheck(AlfTestState* state, INT_TYPE condition, char* conditionText, char* file, INT_TYPE line)
{
	alfCheckM(state, condition, conditionText, file, line, NULL);
}

// -------------------------------------------------------------------------- //

inline void alfCheckM(AlfTestState* state, INT_TYPE condition, char* conditionText, char* file, INT_TYPE line, char* explanation)
{
	state->count++;
	if (!condition)
	{
		state->failCount++;
		ALF_TEST_PRINTF(
			"\t" ALF_TEST_CC_FILE "%s" ALF_TEST_CC_RESET ":" ALF_TEST_CC_LINE "%i" ALF_TEST_CC_RESET ": "
			ALF_TEST_CC_FAIL "FAIL" ALF_TEST_CC_RESET " - " ALF_TEST_CC_TYPE "%s" ALF_TEST_CC_RESET "%s%s%s\n", file, line,
			conditionText,
			explanation ? " - \"" : "",
			explanation ? explanation : "",
			explanation ? "\"" : ""
		);
	}
	else
	{
		ALF_TEST_PRINTF(
			"\t" ALF_TEST_CC_FILE "%s" ALF_TEST_CC_RESET ":" ALF_TEST_CC_LINE "%i" ALF_TEST_CC_RESET ": "
			ALF_TEST_CC_PASS "PASS" ALF_TEST_CC_RESET " - " ALF_TEST_CC_TYPE "%s" ALF_TEST_CC_RESET "%s%s%s\n", file, line,
			conditionText,
			explanation ? " - \"" : "",
			explanation ? explanation : "",
			explanation ? "\"" : ""
		);
	}
}

// -------------------------------------------------------------------------- //

inline char* alfLastIndexOf(char* string, char character)
{
	INT_TYPE length = _alfStringLength(string);
	while (length >= 0)
	{
		if (string[length] == character) { return string + length; }
		length--;
	}
	return string;
}


#endif // ALF_ALLOC_IMPLEMENTATION

// ========================================================================== //
// End of header
// ========================================================================== //

#if defined(__cplusplus)
}
#endif

#endif ALF_TEST_H
