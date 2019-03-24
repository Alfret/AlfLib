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
 * - Timing data for suites, tests and overall. This however should not be used
 *   for checking actual performance.
 * - Colorized output with multiple predefined or custom themes
 * - No dependency on standard library if configurated
 * 
 * USAGE:
 * To use it simply drop the "alf_test.h" and "alf_test.c" files into your 
 * project.
 *
 * CONFIG:
 * There are also some optional macros that can be defined, when compiling the 
 * code as part of your program, to alter the behaviour of the implementation.
 * Note that it's NOT enough to specify these in your own header as that is not
 * included by the implementation file.
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
 * "void name(AlfTestState* state)". However the user should not define tests 
 * manually but instead use the ALF_TEST macro. This macro takes the name of the
 * test and the group that it belongs to. None of these two parameters are 
 * however specified as quoted strings, just the name. These test are then 
 * collected in an array and specified as the argument, together with the count,
 * during creation of the test suite.
 * 
 * Suites are created with "alfCreateTestSuite". This function takes the name of
 * the suite, the array of tests and a count for the number of tests in the 
 * array. When the user is done with the suite it should call 
 * "alfDestroyTestSuite" to free all memory that it's using. When listing the 
 * tests the macro ALF_TEST_LISTING should be used with the same name and group
 * specified when defining the test. This ensures that the correct functions is
 * listed.
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
 * During the tests the user calls one of the many "check" functions (in some
 * implementations called 'assert') to check that some conditions are true. Each
 * of these macros takes an optional reason string that should be used to 
 * describe why the test is important.
 * The possible alternatives are:
 * - ALF_CHECK_TRUE: For checking that a condition is true.
 * - ALF_CHECK_FALSE: For checking that a condition is false.
 * - ALF_CHECK_NOT_NULL: For checking that a pointer is not NULL.
 * - ALF_CHECK_NULL: For checking that a pointer is NULL.
 * - ALF_CHECK_MEM_EQ: For checking that two regions of memory contains the same 
 *		data.
 * - ALF_CHECK_STR_EQ: For checking that two nul-terminated c-strings are equal.
 * - ALF_CHECK_FLOAT_EQ: For checking that two single-precision floating-point 
 *		numbers are equal. This takes into account the machine epsilon.
 * - ALF_CHECK_DOUBLE_EQ: For checking that two double-precision floating-point 
 *		numbers are equal. This takes into account the machine epsilon.
 *
 * EXAMPLE:
 * Here follows a simple example of how to setup a suite to run 3 different 
 * tests. The number of fails from running the test is then returned. 
 *
 * -------------------------------------------------------------------------- *
 * 
 * #include "alf_test.h"
 * 
 * ALF_TEST(one, demo)
 * {
 *     ALF_CHECK_FALSE(2 == 3);	
 * 	   ALF_CHECK_TRUE(3 == 3, "3 must equal 3");
 * }
 * 
 * ALF_TEST(two, demo)
 * {
 *     ALF_CHECK_STREQ("a string", "a string");
 *     ALF_CHECK_NOT_NULL((void*)1245);
 * }
 * 
 * ALF_TEST(three, demo)
 * {
 * 	   ALF_CHECK_NULL(NULL);
 * 	   ALF_CHECK_FLOAT_EQ(state, 3.14f, 3.15f);
 * }
 *
 * int main()
 * {
 *     AlfTest tests[] = {
 *	       ALF_TEST_LISTING(one, demo),
 *	       ALF_TEST_LISTING(two, demo),
 *	       ALF_TEST_LISTING(three, demo)
 *     }
 *     AlfTestSuite* suite = alfCreateTestSuite("Demo Suite", tests, 3);
 * 
 *     const int fails = alfRunSuite(suite);
 *     alfDestroyTestSuite(suite);
 * 	   return fails;
 * }
 *
 * -------------------------------------------------------------------------- *
 * 
 * END:
 * For more information and some more examples visit the github repository: 
 * https://github.com/Alfret/AlfLib
 */

// ====================================================================== //
// Forward Declarations
// ====================================================================== //
	
typedef struct tag_AlfTestState AlfTestState;
	
// ========================================================================== //
// Type definitions
// ========================================================================== //

/** Test function **/
typedef void(*PFN_AlfTest)(AlfTestState* state);

// ========================================================================== //
// Macro declarations
// ========================================================================== //

// Filename macro
#if defined(_WIN32)
	/** Macro for name of current file **/
#	define __FILENAME__ (alfLastIndexOf((const char*)__FILE__, '\\') ?	\
		alfLastIndexOf((const char*)__FILE__, '\\') + 1 : (const char*)__FILE__)
#else
	/** Macro for name of current file **/
#	define __FILENAME__ (alfLastIndexOf(__FILE__, '/') ?	\
		alfLastIndexOf(__FILE__, '/') + 1 : __FILE__)
#endif

/** Float epsilon **/
#define ALF_TEST_FLOAT_EPSILON (1.19e-07)
/** Double epsilon **/
#define ALF_TEST_DOUBLE_EPSILON (2.22e-16)

// ========================================================================== //
// Type definitions
// ========================================================================== //

/** Boolean type **/
typedef unsigned long AlfTestBool;
// -------------------------------------------------------------------------- //

/** Standard integer type **/
typedef unsigned char AlfTestByte;

// -------------------------------------------------------------------------- //

/** Standard integer type **/
typedef unsigned int AlfTestInt;

// -------------------------------------------------------------------------- //

/** Size type **/
typedef unsigned long long AlfTestSize;

// -------------------------------------------------------------------------- //

/** Integer type for time **/
typedef unsigned long long AlfTestTime;

// ========================================================================== //
// Structures
// ========================================================================== //

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

/** \struct AlfTestCheckParameters
 * \author Filip Björklund
 * \date 10 mars 2019 - 14:10
 * \brief Check parameters.
 * \details
 * Structure that contains extra parameters for a check function.
 */
typedef struct AlfTestCheckParameters
{
	/** Internally used variable **/
	char _u;
	/** Test reason **/
	const char* reason;
} AlfTestCheckParameters;

// -------------------------------------------------------------------------- //

/** \struct AlfTestData
 * \author Filip Björklund
 * \date 10 mars 2019 - 14:11
 * \brief Test data.
 * \details
 * Structure that hold static data about a test
 */
typedef struct AlfTestData
{
	/** Name of the test **/
	const char* testName;
	/** Test group name **/
	const char* groupName;
} AlfTestData;

// ========================================================================== //
// Test Generation Macros
// ========================================================================== //

/** Macro for providing a main function that runs the tests **/
#define ALF_TEST_MAIN()	\
	int main() { return alfTestRun(); }

// -------------------------------------------------------------------------- //

#if defined(_WIN32)
/** Macro for exporting function **/
#	define ALF_TEST_EXPORT __declspec(dllexport)
#else
#	define ALF_TEST_EXPORT
#endif

/** Macro to concatenate 'a' and 'b' (Auxilliary) **/
#define ALF_TEST_CONCAT_(a, b) a ## b

// -------------------------------------------------------------------------- //

/** Macro to concatenate 'a' and 'b' **/
#define ALF_TEST_CONCAT(a, b) ALF_TEST_CONCAT_(a, b)

// -------------------------------------------------------------------------- //

/** Macro for generating a name for a test **/
#define ALF_TEST_NAME(name, group) _alf_test_ ## name ## _ ## group ## _

// -------------------------------------------------------------------------- //

/** Macro for generating a name for static test data structure **/
#define ALF_TEST_DATA_NAME(name, group) g ## _alf_ ## name ## _ ## group

// -------------------------------------------------------------------------- //

/** Macro for listing a test in an array of tests **/
#define ALF_TEST_LISTING(name, group)	\
	{ ALF_TEST_DATA_NAME(name, group).testName, ALF_TEST_NAME(name, group) }

// -------------------------------------------------------------------------- //

/** Macro for defining a test **/
#define ALF_TEST(name, group)												\
	ALF_TEST_EXPORT void ALF_TEST_CONCAT(_alf_test_get_data_, __COUNTER__)	\
		(AlfTestData* data)													\
	{																		\
		data->testName = name;												\
		data->groupName = group;											\
	}																		\
	ALF_TEST_EXPORT void ALF_TEST_CONCAT(_alf_test_, __COUNTER__)			\
		(AlfTestState* _state_internal_)

// ========================================================================== //
// Check Macros
// ========================================================================== //

#if defined(__cplusplus)
#	define ALF_CHECK_PARAM_TYPE
#else
#	define ALF_CHECK_PARAM_TYPE (AlfTestCheckParameters)
#endif

/** Check that condition is true **/
#define ALF_CHECK_TRUE(condition, ...)										\
	alfCheckTrue(															\
		_state_internal_, 0, condition, #condition, __FILENAME__, __LINE__,	\
		ALF_CHECK_PARAM_TYPE { 0, __VA_ARGS__ }							\
	);

// -------------------------------------------------------------------------- //

/** Check that condition is true **/
#define ALF_REQUIRE_TRUE(condition, ...)									\
	alfCheckTrue(															\
		_state_internal_, 1, condition, #condition, __FILENAME__, __LINE__,	\
		ALF_CHECK_PARAM_TYPE{ 0, __VA_ARGS__ }							\
	);

// -------------------------------------------------------------------------- //

/** Check that condition is false **/
#define ALF_CHECK_FALSE(condition, ...)										\
	alfCheckFalse(															\
		_state_internal_, 0, !(condition), #condition, __FILENAME__,		\
		__LINE__, ALF_CHECK_PARAM_TYPE{ 0, __VA_ARGS__ }				\
	);

// -------------------------------------------------------------------------- //

/** Check that condition is false **/
#define ALF_REQUIRE_FALSE(condition, ...)									\
	alfCheckFalse(															\
		_state_internal_, 1, !(condition), #condition, __FILENAME__,		\
		__LINE__, ALF_CHECK_PARAM_TYPE{ 0, __VA_ARGS__ }				\
	);

// -------------------------------------------------------------------------- //

/** Check that a pointer is not NULL **/
#define ALF_CHECK_NOT_NULL(pointer, ...)									\
	alfCheckNotNull(														\
		_state_internal_, 0, pointer, #pointer, __FILENAME__, __LINE__,		\
		ALF_CHECK_PARAM_TYPE{ 0, __VA_ARGS__ }							\
	);

// -------------------------------------------------------------------------- //

/** Check that a pointer is not NULL **/
#define ALF_REQUIRE_NOT_NULL(pointer, ...)									\
	alfCheckNotNull(														\
		_state_internal_, 1, pointer, #pointer, __FILENAME__, __LINE__,		\
		ALF_CHECK_PARAM_TYPE{ 0, __VA_ARGS__ }							\
	);

// -------------------------------------------------------------------------- //

/** Check that a pointer is NULL **/
#define ALF_CHECK_NULL(pointer, ...)										\
	alfCheckNull(															\
		_state_internal_, 0, pointer, #pointer, __FILENAME__, __LINE__,		\
		ALF_CHECK_PARAM_TYPE{ 0, __VA_ARGS__ }							\
	);

// -------------------------------------------------------------------------- //

/** Check that a pointer is NULL **/
#define ALF_REQUIRE_NULL(pointer, ...)										\
	alfCheckNull(															\
		_state_internal_, 1, pointer, #pointer, __FILENAME__, __LINE__,		\
		ALF_CHECK_PARAM_TYPE{ 0, __VA_ARGS__ }							\
	);

// -------------------------------------------------------------------------- //

/** Check that two memory regions contains the same data **/
#define ALF_CHECK_MEM_EQ(m0, m1, size, ...)									\
	alfCheckMemEq(															\
		_state_internal_, 0, m0, m1, #m0, #m1, size, __FILENAME__, __LINE__,\
		ALF_CHECK_PARAM_TYPE{ 0, __VA_ARGS__ }							\
	);

// -------------------------------------------------------------------------- //

/** Check that two memory regions contains the same data **/
#define ALF_REQUIRE_MEM_EQ(m0, m1, size, ...)								\
	alfCheckMemEq(															\
		_state_internal_, 1, m0, m1, #m0, #m1, size, __FILENAME__, __LINE__,\
		ALF_CHECK_PARAM_TYPE{ 0, __VA_ARGS__ }							\
	);

// -------------------------------------------------------------------------- //

/** Check that two nul-terminated c-strings are equal **/
#define ALF_CHECK_STR_EQ(str0, str1, ...)									\
	alfCheckStrEq(															\
		_state_internal_, 0, str0, str1, #str0, #str1, __FILENAME__,		\
		__LINE__, ALF_CHECK_PARAM_TYPE{ 0, __VA_ARGS__ }				\
	);

// -------------------------------------------------------------------------- //

/** Check that two nul-terminated c-strings are equal **/
#define ALF_REQUIRE_STR_EQ(str0, str1, ...)									\
	alfCheckStrEq(															\
		_state_internal_, 1, str0, str1, #str0, #str1, __FILENAME__,		\
		__LINE__, ALF_CHECK_PARAM_TYPE{ 0, __VA_ARGS__ }				\
	);

// -------------------------------------------------------------------------- //

/** Check that two 32-bit floating-point numbers are equal **/
#define ALF_CHECK_FLOAT_EQ(float0, float1, ...)								\
	alfCheckFloatEq(														\
		_state_internal_, 0, float0, float1, #float0, #float1,				\
		__FILENAME__, __LINE__,												\
		ALF_CHECK_PARAM_TYPE{ 0, __VA_ARGS__ }							\
	);

// -------------------------------------------------------------------------- //

/** Check that two 32-bit floating-point numbers are equal **/
#define ALF_REQUIRE_FLOAT_EQ(float0, float1, ...)							\
	alfCheckFloatEq(														\
		_state_internal_, 1, float0, float1, #float0, #float1,				\
		__FILENAME__, __LINE__,												\
		ALF_CHECK_PARAM_TYPE{ 0, __VA_ARGS__ }							\
	);

// -------------------------------------------------------------------------- //

/** Check that two 64-bit floating-point numbers are equal **/
#define ALF_CHECK_DOUBLE_EQ(double0, double1, ...)							\
	alfCheckDoubleEq(														\
		_state_internal_, 0, double0, double1, #double0, #double1,			\
		__FILENAME__, __LINE__,												\
		ALF_CHECK_PARAM_TYPE{ 0, __VA_ARGS__ }							\
	);

// -------------------------------------------------------------------------- //

/** Check that two 64-bit floating-point numbers are equal **/
#define ALF_REQUIRE_DOUBLE_EQ(double0, double1, ...)						\
	alfCheckDoubleEq(														\
		_state_internal_, 1, double0, double1, #double0, #double1,			\
		__FILENAME__, __LINE__,												\
		ALF_CHECK_PARAM_TYPE{ 0, __VA_ARGS__ }							\
	);

// ========================================================================== //
// Shorthand Macros
// ========================================================================== //

/** Check that condition is true. Alias for the longer version
 * ALF_CHECK_TRUE **/
#define ALF_CHECK(condition, ...) ALF_CHECK_TRUE(condition, ##__VA_ARGS__)

// -------------------------------------------------------------------------- //

/** Check that condition is true. Alias for the longer version
 * ALF_REQUIRE_TRUE **/
#define ALF_REQUIRE(condition, ...) ALF_REQUIRE_TRUE(condition, ##__VA_ARGS__)

// ========================================================================== //
// Catch2 Interop
// ========================================================================== //

#if defined(ALF_TEST_CATCH2_INTEROP)
	/** Declare a test case **/
#	define TEST_CASE(name, group) ALF_TEST(name, group)
	/** Check condition to hold true **/
#	define CHECK(condition, ...) ALF_CHECK_TRUE(condition, ##__VA_ARGS__)
	/** Require condition to hold true **/
#	define REQUIRE(condition, ...) ALF_REQUIRE_TRUE(condition, ##__VA_ARGS__)
#endif

// ========================================================================== //
// Functions
// ========================================================================== //

AlfTestInt alfTestRun();

// ========================================================================== //
// Check Functions
// ========================================================================== //


/** Function to do a check (assertion) during a test. This should however not be
 * used directly. Instead use the check macros.
 * \brief Function for checking during test.
 * \param state Test state.
 * \param require Whether the test is required to pass.
 * \param predicate Predicate to check.
 * \param predicateString Predicate in string form.
 * \param file File in which check is done.
 * \param line Line in file.
 * \param parameters Extended parameters, contains a reason defaulted to NULL.
 */
void alfCheckTrue(
	AlfTestState* state,
	AlfTestBool require,
	AlfTestBool predicate,
	const char predicateString[],
	const char* file,
	AlfTestInt line,
	AlfTestCheckParameters parameters);

// -------------------------------------------------------------------------- //

/** Function to do a check (assertion) during a test. This should however not be
 * used directly. Instead use the check macros.
 * \brief Function for checking during test.
 * \param state Test state.
 * \param require Whether the test is required to pass.
 * \param predicate Predicate to check.
 * \param predicateString Predicate in string form.
 * \param file File in which check is done.
 * \param line Line in file.
 * \param parameters Extended parameters, contains a reason defaulted to NULL.
 */
void alfCheckFalse(
	AlfTestState* state,
	AlfTestBool require,
	AlfTestBool predicate,
	const char predicateString[],
	const char* file,
	AlfTestInt line,
	AlfTestCheckParameters parameters);

// -------------------------------------------------------------------------- //

/** Function to do a check (assertion) during a test. This should however not be
 * used directly. Instead use the check macros.
 * \brief Function for checking during test.
 * \param state Test state.
 * \param require Whether the test is required to pass.
 * \param pointer Pointer to check.
 * \param pointerText Pointer in string form.
 * \param file File in which check is done.
 * \param line Line in file.
 * \param parameters Extended parameters, contains a reason defaulted to NULL.
 */
void alfCheckNotNull(
	AlfTestState* state,
	AlfTestBool require,
	void* pointer,
	const char pointerText[],
	const char* file,
	AlfTestInt line,
	AlfTestCheckParameters parameters);

// -------------------------------------------------------------------------- //

/** Function to do a check (assertion) during a test. This should however not be
 * used directly. Instead use the check macros.
 * \brief Function for checking during test.
 * \param state Test state.
 * \param require Whether the test is required to pass.
 * \param pointer Pointer to check.
 * \param pointerText Pointer in string form.
 * \param file File in which check is done.
 * \param line Line in file.
 * \param parameters Extended parameters, contains a reason defaulted to NULL.
 */
void alfCheckNull(
	AlfTestState* state,
	AlfTestBool require,
	void* pointer,
	const char pointerText[],
	const char* file,
	AlfTestInt line,
	AlfTestCheckParameters parameters);

// -------------------------------------------------------------------------- //

/** Function to do a check (assertion) during a test. This should however not be
 * used directly. Instead use the check macros.
 * \brief Function for checking during test.
 * \param state Test state.
 * \param require Whether the test is required to pass.
 * \param m0 First memory region.
 * \param m1 Second memory region.
 * \param var0 First pointer name.
 * \param var1 Second pointer name.
 * \param size Memory region size.
 * \param file File in which check is done.
 * \param line Line in file.
 * \param parameters Extended parameters, contains a reason defaulted to NULL.
 */
void alfCheckMemEq(
	AlfTestState* state,
	AlfTestBool require,
	const void* m0,
	const void* m1,
	const char var0[],
	const char var1[],
	AlfTestSize size,
	const char* file,
	AlfTestInt line,
	AlfTestCheckParameters parameters);

// -------------------------------------------------------------------------- //

/** Function to do a check (assertion) during a test. This should however not be
 * used directly. Instead use the check macros.
 * \brief Function for checking during test.
 * \param state Test state.
 * \param require Whether the test is required to pass.
 * \param str0 First string.
 * \param str1 Second string.
 * \param var0 First string name.
 * \param var1 Second string name.
 * \param file File in which check is done.
 * \param line Line in file.
 * \param parameters Extended parameters, contains a reason defaulted to NULL.
 */
void alfCheckStrEq(
	AlfTestState* state,
	AlfTestBool require,
	const char* str0, 
	const char* str1, 
	const char var0[], 
	const char var1[],
	const char* file,
	AlfTestInt line,
	AlfTestCheckParameters parameters);

// -------------------------------------------------------------------------- //

/** Function to do a check (assertion) during a test. This should however not be
 * used directly. Instead use the check macros.
 * \brief Function for checking during test.
 * \param state Test state.
 * \param require Whether the test is required to pass.
 * \param float0 First float.
 * \param float1 Second float.
 * \param var0 First float name.
 * \param var1 Second float name.
 * \param file File in which check is done.
 * \param line Line in file.
 * \param parameters Extended parameters, contains a reason defaulted to NULL.
 */
void alfCheckFloatEq(
	AlfTestState* state,
	AlfTestBool require,
	const float* float0,
	const float* float1,
	const char var0[],
	const char var1[],
	const char* file,
	AlfTestInt line,
	AlfTestCheckParameters parameters);

// -------------------------------------------------------------------------- //

/** Function to do a check (assertion) during a test. This should however not be
 * used directly. Instead use the check macros.
 * \brief Function for checking during test.
 * \param state Test state.
 * \param require Whether the test is required to pass.
 * \param double0 First double.
 * \param double1 Second double.
 * \param var0 First float name.
 * \param var1 Second float name.
 * \param file File in which check is done.
 * \param line Line in file.
 * \param parameters Extended parameters, contains a reason defaulted to NULL.
 */
void alfCheckDoubleEq(
	AlfTestState* state,
	AlfTestBool require,
	const double* double0,
	const double* double1,
	const char var0[],
	const char var1[],
	const char* file,
	AlfTestInt line,
	AlfTestCheckParameters parameters);

// ========================================================================== //
// Utility Functions
// ========================================================================== //

/** Returns the index of the last occurance of a character in an ASCII string.
 * \brief Returns last index of character.
 * \param string String to get last index of character in.
 * \param character Character to look for.
 * \return Last index of.
 */
const char* alfLastIndexOf(const char* string, char character);

// ========================================================================== //
// End of header
// ========================================================================== //

#if defined(__cplusplus)
}
#endif

#endif // ALF_TEST_H
