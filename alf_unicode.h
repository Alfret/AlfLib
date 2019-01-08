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

#ifndef ALF_UNICODE_H
#define ALF_UNICODE_H

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

/** True value (1) **/
#define ALF_TRUE 1

// -------------------------------------------------------------------------- //

/** False value (0) **/
#define ALF_FALSE 0

// -------------------------------------------------------------------------- //

/** Prototype for a function that can be used as a callback during UTF-8 string
 * iteration.
 * \param codepoint Codepoint at current iteration index.
 * \param byte offset of codepoint.
 * \param index Codepoint index in string.
 * \param Return true to continue iteration, return false to stop.
 */
typedef AlfBool(*PFN_AlfUTF8Iterate)(
	uint32_t codepoint,
	uint64_t byteOffset,
	uint64_t index
	);

// ========================================================================== //
// Functions
// ========================================================================== //

/** Decode a codepoint from an UTF-8 string at the specified offset. The 
 * codepoint and number of bytes that it's encoded in is returned in parameter 
 * 3 and 4.
 * \note If the function fails to decode a codepoint then the codepoint and 
 * number of bytes parameters are not overwritten.
 * \brief Decode UTF-8 codepoint
 * \param string String to decode codepoint in.
 * \param offset Offset to decode at in string.
 * \param codepoint Codepoint.
 * \param numBytes Number of bytes that codepoint is encoded in.
 * \return True if a valid UTF-8 codepoint could be decoded else false.
 */
AlfBool alfUTF8Decode(
	const char* string,
	uint64_t offset,
	uint32_t* codepoint,
	uint32_t* numBytes);

// -------------------------------------------------------------------------- //

/** Encode a codepoint in UTF-8 at a specified offset in a string. The string is
 * expected to be large enough to hold the bytes for the encoded codepoint.
 * \brief Encode codepoint in UTF-8.
 * \param string String to encode codepoint into.
 * \param offset Offset to encode codepoint at.
 * \param codepoint Codepoint to encode into string.
 * \param numBytes Number of bytes that codepoint was encoded in.
 * \return Returns true if the encoding is successfull, else false.
 */
AlfBool alfUTF8Encode(
	char* string,
	uint64_t offset,
	uint32_t codepoint,
	uint32_t* numBytes
	);

// -------------------------------------------------------------------------- //

/** Returns the width of a codepoint in bytes when encoded as UTF-8.
 * \brief Returns UTF-8 codepoint byte-width.
 * \param codepoint Codepoint to get width of.
 * \return number of bytes that codepoint is encoded in. 0 is returned if the 
 * codepoint is not a valid unicode codepoint.
 */
uint32_t alfUTF8CodepointWidth(uint32_t codepoint);

// -------------------------------------------------------------------------- //

/** Returns the length of an UTF-8 encoded string in number of codepoints.
 * \brief Returns UTF-8 string length.
 * \param string String to get length of.
 * \return Length of string.
 */
uint64_t alfUTF8StringLength(const char* string);

// -------------------------------------------------------------------------- //

/** Returns the index of the first occurance of a codepoint in a UTF-8 encoded
 * string. If the codepoint is not in the string then -1 is returned instead.
 * \brief Returns first index of codepoint.
 * \param string String.
 * \param codepoint Codepoint to get index of.
 * \return Index of codepoint or -1 if codepoint was not found in string.
 */
int64_t alfUTF8IndexOf(const char* string, uint32_t codepoint);

// -------------------------------------------------------------------------- //

/** Returns the index of the last occurance of a codepoint in a UTF-8 encoded
 * string. If the codepoint is not in the string then -1 is returned instead.
 * \note -1 is also returned if string is NULL.
 * \brief Returns last index of codepoint.
 * \param string String.
 * \param codepoint Codepoint to get index of.
 * \return Index of codepoint of -1 if codepoint was not found in string.
 */
int64_t alfUTF8LastIndexOf(const char* string, uint32_t codepoint);

// -------------------------------------------------------------------------- //

/** Returns whether or not a UTF-8 encoded string starts with the specified
 * codepoint.
 * \note False is returned if string is NULL.
 * \brief Returns whether UTF-8 string starts with codepoint.
 * \param string String to check if starts with codepoint.
 * \param codepoint Codepoint to check if string starts with.
 * \return True if the string starts with the codepoint else false.
 */
AlfBool alfUTF8StartsWith(const char* string, uint32_t codepoint);

// -------------------------------------------------------------------------- //

/** Returns whether or not a UTF-8 encoded string ends with the specified
 * codepoint.
 * \note False is returned if string is NULL.
 * \brief Returns whether UTF-8 string ends with codepoint.
 * \param string String to check if ends with codepoint.
 * \param codepoint Codepoint to check if string ends with.
 * \return True if the string ends with the codepoint else false.
 */
AlfBool alfUTF8EndsWith(const char* string, uint32_t codepoint);

// -------------------------------------------------------------------------- //s

/** Returns substring of a UTF-8 encoded string from the specified 'from' index
 * and 'count' number of indices forward.
 * \note Returns NULL if string is NULL.
 * \note A sum of 'from' and 'count' greater than the lenght of the string will
 * result in undefined behaviour.
 * \brief Returns UTF-8 substring.
 * \param string String to get substring of.
 * \param from Index to get substring from.
 * \param count Number of codepoints in substring.
 * \return Substring or NULL on failure.
 */
char* alfUTF8Substring(const char* string, uint64_t from, uint64_t count);

// -------------------------------------------------------------------------- //

/** Returns substring of a UTF-8 encoded string from the specified 'from' index
 * to the end of the string.
 * \note Returns NULL if string is NULL.
 * \brief Returns UTF-8 substring.
 * \param string String to get substring of.
 * \param from Index to get substring from.
 * \return Substring or NULL on failure.
 */
char* alfUTF8SubstringFrom(const char* string, uint64_t from);

// -------------------------------------------------------------------------- //

/** Insert a string into another string. The insertion string is inserted 
 * between the 'from' index and 'count' indices forward. These properties give 
 * this function great flexibility.
 * 
 * Common usages are:
 * - Insertion: Insert strings into the original string with 'count' set to 0, 
 *   this means that it's a pure insertion and does not remove any letters in 
 *   the original text.
 * - Deletion: Insert an empty string into the original with 'count' != 0. 
 *   Everything from index 'from' and 'count' letters forward are removed.
 * - Replace: Insert a non-empty string into the original string where 'count'
 *   is not 0.
 * 
 * \brief Insert string into other string.
 * \param[in] string String to insert other string into.
 * \param[in] from Index to start insertion at.
 * \param[in] count Number of indices after 'from' to insert to.
 * \param[in] insertion String to insert.
 * \return Resulting string after insertion.
 */
char* alfUTF8Insert(
	const char* string, 
	uint64_t from, 
	uint64_t count, 
	const char* insertion);

// -------------------------------------------------------------------------- //

/** Returns the codepoint at a specified index in a UTF-8 encoded string.
 * \brief Returns codepoint at index in UTF-8 string.
 * \param string String to get codepoint at index from.
 * \param index Index to get codepoint at.
 * \return Codepoint at index.
 */
uint32_t alfUTF8AtIndex(const char* string, uint64_t index);

// -------------------------------------------------------------------------- //

/** Returns the byte offset for a specified codepoint index in a UTF-8 encoded
 * string.
 * \note Returns 0 if the string is NULL.
 * \brief Returns byte offset of codepoint index.
 * \param string String to get byte offset of index in.
 * \param index Index to get byte offset of.
 * \return Byte offset of index.
 */
uint64_t alfUTF8OffsetOfIndex(const char* string, uint64_t index);

// -------------------------------------------------------------------------- //

/** Maps each of the indices in the 'indices' parameter to their respective byte
 * offset in the string. The indices must be strictly increasing. The number of
 * indices that was written will be returned in the 'indicesSet' parameter.
 * \brief Maps indices to offsets in UTF-8 string.
 * \param[in] string String to map indices in.
 * \param[in,out] indices Indices to map to offsets. Offsets are written over
 * the indices of the input
 * \param[in] indexCount Number of indices in list.
 * \param[out] indicesSet Returns the number of indices set.
 * \return True if all indices were successfully set, otherwise false.
 */
AlfBool alfUTF8OffsetOfIndices(
	const char* string, 
	uint64_t* indices, 
	uint32_t indexCount, 
	uint32_t* indicesSet);

// -------------------------------------------------------------------------- //

/** Returns whether or not a string is a valid UTF-8 encoded string.
 * \brief Returns whether string are valid UTF-8.
 * \param string String to check if valid UTF-8.
 * \return True if the string is valid UTF-8 else false.
 */
AlfBool alfUTF8Valid(const char* string);

// -------------------------------------------------------------------------- //

/** Iterates through all the codepoints in a UTF-8 encoded string and calls the
 * specified callback function with the codepoint, byte offset and index.
 * \brief Iterate through UTF-8 string.
 * \param string String to iterate through.
 * \param iterateFunc Iterator function.
 * \return True if the complete string was iterated through. False if the
 * iterateFunc callback returned false or if an error occured such as the string
 * is not valid UTF-8.
 */
AlfBool alfUTF8Iterate(const char* string, PFN_AlfUTF8Iterate iterateFunc);

// ========================================================================== //
// End of Header
// ========================================================================== //

#if defined(__cplusplus)
}
#endif

#endif // ALF_UNICODE_H