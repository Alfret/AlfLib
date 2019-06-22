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
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
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
#include <assert.h>
#include <memory.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Windows headers
#if defined(_WIN32)
#include <Windows.h>
#endif

// MacOS headers
#if defined(__APPLE__)
#include <mach/mach_time.h>
#endif

// Linux headers
#if defined(__linux__)
#include <dlfcn.h>
#include <time.h>
#endif

// ========================================================================== //
// Color themes declarations
// ========================================================================== //

#define ALF_TEST_THEME_PALE

// Template for color themes
#if 0

#endif

// Color reset macro
#if !defined(ALF_THEME_NONE)
#define ALF_TEST_CC_RESET "\033[0m"
#else
#define ALF_TEST_CC_RESET ""
#endif

// Color macros for different themes and support
#if defined(ALF_TEST_NO_TRUECOLOR) || defined(ALF_THEME_NONE)
#if defined(ALF_THEME_NONE)
#define ALF_TEST_CC_SUITE ""
#define ALF_TEST_CC_NAME ""
#define ALF_TEST_CC_FILE ""
#define ALF_TEST_CC_LINE ""
#define ALF_TEST_CC_TIME ""
#define ALF_TEST_CC_PASS ""
#define ALF_TEST_CC_FAIL ""
#define ALF_TEST_CC_TYPE ""
#elif defined(ALF_TEST_THEME_PALE)
#define ALF_TEST_CC_SUITE "\033[38;5;93m"
#define ALF_TEST_CC_NAME "\033[38;5;63m"
#define ALF_TEST_CC_FILE "\033[38;5;69m"
#define ALF_TEST_CC_LINE "\033[38;5;26m"
#define ALF_TEST_CC_TIME "\033[38;5;220m"
#define ALF_TEST_CC_PASS "\033[38;5;85m"
#define ALF_TEST_CC_FAIL "\033[38;5;197m"
#define ALF_TEST_CC_TYPE "\033[38;5;171m"
#elif defined(ALF_TEST_THEME_AUTUMN_FRUIT)
#define ALF_TEST_CC_SUITE ""
#define ALF_TEST_CC_NAME ""
#define ALF_TEST_CC_FILE ""
#define ALF_TEST_CC_LINE ""
#define ALF_TEST_CC_TIME ""
#define ALF_TEST_CC_PASS ""
#define ALF_TEST_CC_FAIL ""
#define ALF_TEST_CC_TYPE ""
#else // Default non-truecolor theme
#define ALF_TEST_CC_SUITE ""
#define ALF_TEST_CC_NAME ""
#define ALF_TEST_CC_FILE ""
#define ALF_TEST_CC_LINE ""
#define ALF_TEST_CC_TIME ""
#define ALF_TEST_CC_PASS ""
#define ALF_TEST_CC_FAIL ""
#define ALF_TEST_CC_TYPE ""
#endif
#else
#define ALF_TEST_CC(r, g, b) "\033[38;2;" #r ";" #g ";" #b "m"
#if defined(ALF_TEST_THEME_NONE)
#define ALF_TEST_CC_SUITE ""
#define ALF_TEST_CC_NAME ""
#define ALF_TEST_CC_FILE ""
#define ALF_TEST_CC_LINE ""
#define ALF_TEST_CC_TIME ""
#define ALF_TEST_CC_PASS ""
#define ALF_TEST_CC_FAIL ""
#define ALF_TEST_CC_TYPE ""
#elif defined(ALF_TEST_THEME_PALE)
#define ALF_TEST_CC_SUITE ALF_TEST_CC(137, 93, 226)
#define ALF_TEST_CC_NAME ALF_TEST_CC(91, 138, 224)
#define ALF_TEST_CC_FILE ALF_TEST_CC(120, 159, 230)
#define ALF_TEST_CC_LINE ALF_TEST_CC(66, 120, 220)
#define ALF_TEST_CC_TIME ALF_TEST_CC(255, 196, 88)
#define ALF_TEST_CC_PASS ALF_TEST_CC(77, 225, 169)
#define ALF_TEST_CC_FAIL ALF_TEST_CC(255, 71, 57)
#define ALF_TEST_CC_TYPE ALF_TEST_CC(189, 99, 197)
#elif defined(ALF_TEST_THEME_AUTUMN_FRUIT)
#define ALF_TEST_CC_SUITE ALF_TEST_CC(111, 88, 201)
#define ALF_TEST_CC_NAME ALF_TEST_CC(87, 74, 226)
#define ALF_TEST_CC_FILE ALF_TEST_CC(101, 69, 151)
#define ALF_TEST_CC_LINE ALF_TEST_CC(232, 142, 237)
#define ALF_TEST_CC_TIME ALF_TEST_CC(34, 42, 104)
#define ALF_TEST_CC_PASS ALF_TEST_CC(189, 228, 167)
#define ALF_TEST_CC_FAIL ALF_TEST_CC(216, 30, 91)
#define ALF_TEST_CC_TYPE ALF_TEST_CC(255, 199, 134)
#else // Default truecolor theme
#define ALF_TEST_CC_SUITE ALF_TEST_CC(111, 88, 201)
#define ALF_TEST_CC_NAME ALF_TEST_CC(34, 116, 165)
#define ALF_TEST_CC_FILE ALF_TEST_CC(241, 196, 15)
#define ALF_TEST_CC_LINE ALF_TEST_CC(247, 92, 3)
#define ALF_TEST_CC_TIME ALF_TEST_CC(27, 153, 139)
#define ALF_TEST_CC_PASS ALF_TEST_CC(0, 204, 102)
#define ALF_TEST_CC_FAIL ALF_TEST_CC(217, 3, 104)
#define ALF_TEST_CC_TYPE ALF_TEST_CC(244, 128, 194)
#endif
#endif // defined(ALF_TEST_NO_TRUECOLOR)

// Library name color
#if !defined(ALF_THEME_NONE)
#define ALF_TEST_CC_LOGO "\033[38;5;112m"
#define ALF_TEST_CC_C "\033[38;5;45m"
#endif

// ========================================================================== //
// Macro Declarations
// ========================================================================== //

/** Size of format buffer **/
#define ALF_TEST_FORMAT_BUFFER_SIZE 1024

// ========================================================================== //
// Type Definitions
// ========================================================================== //

typedef void (*PFN_AlfTestGetData)(AlfTestData* data);

// ========================================================================== //
// Global Variables
// ========================================================================== //

/** Buffer that is used for temporary storage when formatting string **/
static char sFormatBuffer[ALF_TEST_FORMAT_BUFFER_SIZE];

// ========================================================================== //
// Private structures
// ========================================================================== //

typedef struct AlfTest
{
  const char* group;
  const char* name;
  PFN_AlfTest function;
} AlfTest;

// -------------------------------------------------------------------------- //

/** Private test-state structure **/
typedef struct tag_AlfTestState
{
  /** Total check count **/
  AlfTestInt count;
  /** Failed check count **/
  AlfTestInt failCount;
} tag_AlfTestState;

// ========================================================================== //
// Private function implementations
// ========================================================================== //

/** Setup console for platforms that does not support escape sequences out of
 * the box **/
static void
alfSetupConsole()
{
#if defined(_WIN32)
  static AlfTestInt isSetup = 0;
  if (!isSetup) {
    // Enable virtual terminal processing for Color support
    DWORD mode;
    const HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (outputHandle) {
      BOOL success = GetConsoleMode(outputHandle, &mode);
      assert(success && "Failet to get Windows console mode");
      success =
        SetConsoleMode(outputHandle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
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

static AlfTestInt
alfStringLength(const char* string)
{
  AlfTestInt length = 0;
  while (string[length++] != 0) {
  }
  return length - 1;
}

// -------------------------------------------------------------------------- //

/** Returns a relative time from high-performance timer **/
static AlfTestTime
alfTestTimer()
{
#if defined(_WIN32)
  // Only query the performance counter frequency once
  static AlfTestTime frequency = 0;
  if (frequency == 0) {
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
  if (timebaseInfo.denom == 0) {
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

/** Internal check function **/
static void
alfTestCheckInternal(AlfTestState* state,
                     AlfTestBool require,
                     AlfTestBool condition,
                     const char* message,
                     const char* file,
                     AlfTestInt line,
                     const char* explanation)
{
  state->count++;
  state->failCount += (condition ? 0 : 1);
  printf("    " ALF_TEST_CC_FILE "%s" ALF_TEST_CC_RESET ":" ALF_TEST_CC_LINE
         "%i" ALF_TEST_CC_RESET ": %s%s" ALF_TEST_CC_RESET
         " - " ALF_TEST_CC_TYPE "%s" ALF_TEST_CC_RESET "%s%s%s\n",
         file,
         (int)line,
         condition ? ALF_TEST_CC_PASS : ALF_TEST_CC_FAIL,
         condition ? "PASS" : "FAIL",
         message,
         explanation ? " - \"" : "",
         explanation ? explanation : "",
         explanation ? "\"" : "");
  if (!condition && require) {
    printf("Required test failed...");
    exit(-1);
  }
}

// -------------------------------------------------------------------------- //

static char*
alfTestFormatString(const char* format, ...)
{
  va_list arguments;
  va_start(arguments, format);

  // Format string
  va_list copy;
  va_copy(copy, arguments);
  const int32_t writtenBytes =
    vsnprintf(sFormatBuffer, ALF_TEST_FORMAT_BUFFER_SIZE, format, arguments);
  va_end(copy);

  // Copy buffer
  char* result = malloc(writtenBytes + 1ull);
  if (!result) {
    return NULL;
  }
  memcpy(result, sFormatBuffer, writtenBytes);
  result[writtenBytes] = 0;

  va_end(arguments);
  return result;
}

// -------------------------------------------------------------------------- //

static void*
alfTestOpenLibrary()
{
#if defined(_WIN32)
  return GetModuleHandleW(NULL);
#else
  return dlopen(NULL, RTLD_LAZY);
#endif
}

// -------------------------------------------------------------------------- //

static void*
alfTestGetProcAddress(void* library, const char* name)
{
#if defined(_WIN32)
  return (PFN_AlfTestGetData)GetProcAddress(library, name);
#else
  return dlsym(library, name);
#endif
}

// ========================================================================== //
// Internal List Implementation
// ========================================================================== //

/** List data **/
typedef struct AlfTestList
{
  /** Capacity **/
  AlfTestSize capacity;
  /** Size of list in number of objects in it **/
  AlfTestSize size;
  /** Size of each object in list, in bytes **/
  AlfTestSize objectSize;

  /** Data buffer **/
  AlfTest* buffer;
} AlfTestList;

// -------------------------------------------------------------------------- //

AlfTestList*
alfTestCreateList()
{
  AlfTestList* list = malloc(sizeof(AlfTestList));
  if (!list) {
    return NULL;
  }

  list->capacity = 10;
  list->size = 0;
  list->buffer = malloc(sizeof(AlfTest) * list->capacity);
  if (!list->buffer) {
    free(list);
    return NULL;
  }

  return list;
}

// -------------------------------------------------------------------------- //

void
alfTestDestroyList(AlfTestList* list)
{
  free(list->buffer);
  free(list);
}

// -------------------------------------------------------------------------- //

AlfTestBool
alfTestListAdd(AlfTestList* list, const AlfTest* test)
{
  if (list->size >= list->capacity) {
    list->capacity *= 2;
    AlfTest* buffer = malloc(sizeof(AlfTest) * list->capacity);
    if (!buffer) {
      return 0;
    }
    memcpy(buffer, list->buffer, sizeof(AlfTest) * list->size);
    free(list->buffer);
    list->buffer = buffer;
  }
  list->buffer[list->size++] = *test;
  return 1;
}

// -------------------------------------------------------------------------- //

AlfTest*
alfTestListGet(AlfTestList* list, AlfTestInt index)
{
  return &list->buffer[index];
}

// -------------------------------------------------------------------------- //

void
alfTestListSwap(AlfTestList* list, AlfTestInt index0, AlfTestInt index1)
{
  AlfTest* test0 = alfTestListGet(list, index0);
  AlfTest* test1 = alfTestListGet(list, index1);
  const AlfTest temp = *test0;
  *test0 = *test1;
  *test1 = temp;
}

// -------------------------------------------------------------------------- //

void
alfTestListSortAux(AlfTestList* list, AlfTestInt low, AlfTestInt high)
{
  if (low < high) {
    AlfTest* pivot = alfTestListGet(list, high);
    uint32_t i = low;
    for (uint32_t j = low; j < high; j++) {
      AlfTest* object = alfTestListGet(list, j);
      if (strcmp(pivot->group, object->group) < 0) {
        alfTestListSwap(list, i, j);
        i++;
      }
    }
    alfTestListSwap(list, i, high);

    alfTestListSortAux(list, low, i > 0 ? i - 1 : 0);
    alfTestListSortAux(list, i + 1, high);
  }
}

// -------------------------------------------------------------------------- //
void
alfTestListSort(AlfTestList* list)
{
  alfTestListSortAux(
    list, 0, (AlfTestInt)(list->size > 0 ? list->size - 1 : 0));
}

// -------------------------------------------------------------------------- //

AlfTestSize
alfTestListSize(AlfTestList* list)
{
  return list->size;
}

// ========================================================================== //
// Function implementations
// ========================================================================== //

AlfTestInt
alfTestRunList(AlfTestList* list)
{
  // Information
  AlfTestInt totalGroupCount = 0;
  AlfTestInt groupFailCount = 0;
  AlfTestInt totalTestCount = 0;
  AlfTestInt testFailCount = 0;
  AlfTestState state = { 0, 0 };

  // Get time for start
  const AlfTestTime startTime = alfTestTimer();
  AlfTestTime groupStartTime = startTime;

  // Run tests from list
  const char* lastTestGroup = NULL;
  AlfTestBool groupDidFail = 0;
  for (AlfTestInt i = 0; i < alfTestListSize(list); i++) {
    // Retrieve information of test to run
    AlfTest* test = alfTestListGet(list, i);
    if (!test) {
      continue;
    }

    // Check if test is part of current group
    if (!lastTestGroup || strcmp(lastTestGroup, test->group) != 0) {
      // Print time of last group
      if (lastTestGroup) {
        const AlfTestTime currentTime = alfTestTimer();
        const AlfTestTime groupTime = currentTime - groupStartTime;
        printf("  group finished in " ALF_TEST_CC_TIME "%.3f" ALF_TEST_CC_RESET
               " ms\n",
               groupTime / 1000000.0);
        groupStartTime = currentTime;
      }

      // New group
      if (groupDidFail) {
        groupFailCount++;
        groupDidFail = 0;
      }
      totalGroupCount++;
      if (lastTestGroup) {
        printf("\n");
      }
      lastTestGroup = test->group;
      printf(ALF_TEST_CC_SUITE "Group" ALF_TEST_CC_RESET " %s\n",
             lastTestGroup);
    }

    // Run test
    const AlfTestInt testCheckFailCountBefore = state.failCount;
    printf(ALF_TEST_CC_NAME "  Test" ALF_TEST_CC_RESET " %s:\n", test->name);
    const AlfTestTime timeBefore = alfTestTimer();
    test->function(&state);
    const AlfTestTime timeDelta = alfTestTimer() - timeBefore;
    printf("    test finished in " ALF_TEST_CC_TIME "%.3f" ALF_TEST_CC_RESET
           " ms\n",
           timeDelta / 1000000.0);
    if (testCheckFailCountBefore != state.failCount) {
      testFailCount++;
      groupDidFail = 1;
    }

    // Update statistics
    totalTestCount++;
    lastTestGroup = test->group;
  }

  // Print time of last group
  if (lastTestGroup) {
    const AlfTestTime currentTime = alfTestTimer();
    const AlfTestTime groupTime = currentTime - groupStartTime;
    printf("  group finished in " ALF_TEST_CC_TIME "%.3f" ALF_TEST_CC_RESET
           " ms\n",
           groupTime / 1000000.0);
  }

  // Print summary
  const AlfTestInt groupPassCount = totalGroupCount - groupFailCount;
  const AlfTestInt testPassCount = totalTestCount - testFailCount;
  const AlfTestInt checkPassCount = state.count - state.failCount;

  printf("\n" ALF_TEST_CC_SUITE "SUMMARY\n" ALF_TEST_CC_RESET);
  printf("Type\t\tTotal\t\tPass\t\tFail\n");
  printf("Group\t\t%i\t\t%i\t\t%i\n",
         (int32_t)totalGroupCount,
         (int32_t)groupPassCount,
         (int32_t)groupFailCount);
  printf("Test\t\t%i\t\t%i\t\t%i\n",
         (int32_t)totalTestCount,
         (int32_t)testPassCount,
         (int32_t)testFailCount);
  printf("Check\t\t%i\t\t%i\t\t%i\n",
         (int32_t)state.count,
         (int32_t)checkPassCount,
         (int32_t)state.failCount);
  printf("completed in " ALF_TEST_CC_TIME "%f" ALF_TEST_CC_RESET " ms\n",
         (alfTestTimer() - startTime) / 1000000.0);

  // Return the failure count
  return state.failCount;
}

// -------------------------------------------------------------------------- //

AlfTestInt
alfTestRun()
{
  alfSetupConsole();

  // Open library of current process to retrieve functions from
  void* library = alfTestOpenLibrary();

  // Create list to hold test data
  AlfTestList* list = alfTestCreateList();

  // Find all test functions
  AlfTestInt index = 0, lastIndex = 0;
  AlfTestBool abortSearch = 0;
  while (!abortSearch) {
    // Check if there is a 'getData' functions at this index
    char nameBuffer[256];
    snprintf(nameBuffer, 256, "_alf_test_get_data_%i", index);
    const PFN_AlfTestGetData GetData =
      alfTestGetProcAddress(library, nameBuffer);
    if (GetData) {
      // Go past the 'getData' function
      index++;

      // And if so, then there is a also a test function at index + 1
      snprintf(nameBuffer, 256, "_alf_test_%i", index);
      const PFN_AlfTest Test = alfTestGetProcAddress(library, nameBuffer);

      // Retrieve test data
      AlfTestData data;
      GetData(&data);

      // Add test
      AlfTest test;
      test.group = data.groupName;
      test.name = data.testName;
      test.function = Test;
      alfTestListAdd(list, &test);

      // Set last found index to this
      lastIndex = index;
    }

    // Abort if we have not found a function in 10 attempts
    if (index >= lastIndex + 10) {
      abortSearch = 1;
    }

    // Increment index to look at next spot
    index++;
  }

  // Sort list
  alfTestListSort(list);

  // Run tests in list
  const AlfTestInt failures = alfTestRunList(list);
  alfTestDestroyList(list);

  // Return number of failed tests
  return failures;
}

// ========================================================================== //
// Check Functions
// ========================================================================== //

void
alfCheckTrue(AlfTestState* state,
             AlfTestBool require,
             AlfTestBool predicate,
             const char predicateString[],
             const char* file,
             AlfTestInt line,
             AlfTestCheckParameters parameters)
{
  char* message = alfTestFormatString("TRUE(%s)", predicateString);
  alfTestCheckInternal(
    state, require, predicate, message, file, line, parameters.reason);
  free(message);
}

// -------------------------------------------------------------------------- //

void
alfCheckFalse(AlfTestState* state,
              AlfTestBool require,
              AlfTestBool predicate,
              const char predicateString[],
              const char* file,
              AlfTestInt line,
              AlfTestCheckParameters parameters)
{
  char* message = alfTestFormatString("FALSE(%s)", predicateString);
  alfTestCheckInternal(
    state, require, predicate, message, file, line, parameters.reason);
  free(message);
}

// -------------------------------------------------------------------------- //

void
alfCheckNotNull(AlfTestState* state,
                AlfTestBool require,
                void* pointer,
                const char pointerText[],
                const char* file,
                AlfTestInt line,
                AlfTestCheckParameters parameters)
{
  char* message =
    alfTestFormatString("NOT_NULL(0x%p (%s))", pointer, pointerText);
  alfTestCheckInternal(
    state, require, pointer != NULL, message, file, line, parameters.reason);
  free(message);
}

// -------------------------------------------------------------------------- //

void
alfCheckNull(AlfTestState* state,
             AlfTestBool require,
             void* pointer,
             const char pointerText[],
             const char* file,
             AlfTestInt line,
             AlfTestCheckParameters parameters)
{
  char* message = alfTestFormatString("NULL(0x%p (%s))", pointer, pointerText);
  alfTestCheckInternal(
    state, require, pointer == NULL, message, file, line, parameters.reason);
  free(message);
}

// -------------------------------------------------------------------------- //

void
alfCheckMemEq(AlfTestState* state,
              AlfTestBool require,
              const void* m0,
              const void* m1,
              const char var0[],
              const char var1[],
              AlfTestSize size,
              const char* file,
              AlfTestInt line,
              AlfTestCheckParameters parameters)
{
  char* message = alfTestFormatString("MEM_EQ(%s == %s)", m0, m0);
  const AlfTestInt predicate =
    (m0 == NULL && m1 == NULL) || (m0 && m1 && memcmp(m0, m1, size) == 0);
  alfTestCheckInternal(
    state, require, predicate, message, file, line, parameters.reason);
  free(message);
}

// -------------------------------------------------------------------------- //

void
alfCheckStrEq(AlfTestState* state,
              AlfTestBool require,
              const char* str0,
              const char* str1,
              const char var0[],
              const char var1[],
              const char* file,
              AlfTestInt line,
              AlfTestCheckParameters parameters)
{
  char* message = alfTestFormatString("STR_EQ(\"%s\" == \"%s\")", str0, str1);
  const AlfTestInt predicate =
    (str0 == NULL && str1 == NULL) || (str0 && str1 && strcmp(str0, str1) == 0);
  alfTestCheckInternal(
    state, require, predicate, message, file, line, parameters.reason);
  free(message);
}

// -------------------------------------------------------------------------- //

void
alfCheckFloatEq(AlfTestState* state,
                AlfTestBool require,
                const float* float0,
                const float* float1,
                const char var0[],
                const char var1[],
                const char* file,
                AlfTestInt line,
                AlfTestCheckParameters parameters)
{
  char* message = alfTestFormatString("FLOAT_EQ(%f == %f)", float0, float1);
  const AlfTestInt predicate = (float0 - float1) <= ALF_TEST_FLOAT_EPSILON;
  alfTestCheckInternal(
    state, require, predicate, message, file, line, parameters.reason);
  free(message);
}

// -------------------------------------------------------------------------- //

void
alfCheckDoubleEq(AlfTestState* state,
                 AlfTestBool require,
                 const double* double0,
                 const double* double1,
                 const char var0[],
                 const char var1[],
                 const char* file,
                 AlfTestInt line,
                 AlfTestCheckParameters parameters)
{
  char* message = alfTestFormatString("DOUBLE_EQ(%f == %f)", double0, double1);
  const AlfTestInt predicate = (double0 - double1) <= ALF_TEST_DOUBLE_EPSILON;
  alfTestCheckInternal(
    state, require, predicate, message, file, line, parameters.reason);
  free(message);
}

// ========================================================================== //
// Utility Functions
// ========================================================================== //

const char*
alfLastIndexOf(const char* string, char character)
{
  AlfTestInt length = alfStringLength(string);
  while (length > 0) {
    if (string[length] == character) {
      return string + length;
    }
    length--;
  }
  return string;
}