// MIT License
//
// Copyright (c) 2018-2019 Filip Björklund
//
// Permission is hereby granted, free of AlfChar8ge, to any person obtaining a copy
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

/** Code unit type for UTF-8 strings **/
typedef char AlfChar8;

// -------------------------------------------------------------------------- //

/** Code unit type for UTF-16 strings **/
#if defined(_WIN32)
	typedef uint16_t AlfChar16;
#else
	typedef uint16_t AlfChar16;
#endif
// -------------------------------------------------------------------------- //

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

// ========================================================================== //
// Callback Function Types
// ========================================================================== //

/** Prototype for a function that can be used as a callback during unicode 
 * string iteration.
 * \param codepoint Codepoint at current iteration index.
 * \param byte offset of codepoint.
 * \param index Codepoint index in string.
 * \param Return true to continue iteration, return false to stop.
 */
typedef AlfBool(*PFN_AlfUnicodeIterateCodepoint)(
	uint32_t codepoint,
	uint64_t byteOffset,
	uint64_t index);

// -------------------------------------------------------------------------- //

typedef AlfBool(*PFN_AlfUnicodeCodepointPredicate)(uint32_t codepoint);

// ========================================================================== //
// UTF-8 Functions
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
 * \return True if a valid unicode codepoint could be decoded else false.
 */
AlfBool alfUTF8Decode(
	const AlfChar8* string,
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
	AlfChar8* string,
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

/** Returns the offset in number of bytes from the specified offset in the 
 * string to the following codepoint.
 * \brief Returns offset to next codepoint.
 * \param string String to get offset to next codepoint in.
 * \param offset Offset at which to return offset to next codepoint at.
 * \return Offset to next codepoint from the specified offset in the input 
 * string.
 */
int32_t alfUTF8OffsetToNextCodepoint(const AlfChar8* string, uint32_t offset);

// -------------------------------------------------------------------------- //

/** Returns information about the location of the next word from the specified 
 * offset in a UTF-8 encoded string. The start offset might differ from the 
 * offset specified to start looking for the word at, if there is whitespace 
 * before the start of the word. The number of bytes are set to the distance 
 * between the start offset and the offset of the end of the string.
 * If no word is found and a nul-terminator was instead reached then the 
 * function returns false.
 * \brief Returns information about next word in UTF-8 encoded strings.
 * \param[in] string String to get next word in.
 * \param[in] offset Offset to start looking for word at.
 * \param[out] startOffset Set to the offset of the start of the word.
 * \param[out] numBytes Set to the number of bytes that the word occupies.
 * \param[in] isWhitespace Function that is called by the implementation to 
 * determine if a codepoint is considered whitespace.
 * \return True if a word was found otherwise false.
 */
AlfBool alfUTF8NextWord(
	const AlfChar8* string,
	uint32_t offset,
	uint32_t* startOffset,
	uint32_t* numBytes,
	PFN_AlfUnicodeCodepointPredicate isWhitespace);

// -------------------------------------------------------------------------- //

/** Returns the length of an UTF-8 encoded string in number of codepoints.
 * \brief Returns UTF-8 string length.
 * \param string String to get length of.
 * \return Length of string.
 */
uint64_t alfUTF8StringLength(const AlfChar8* string);

// -------------------------------------------------------------------------- //

/** Returns the index of the first occurance of a codepoint in a UTF-8 encoded
 * string. If the codepoint is not in the string then -1 is returned instead.
 * \brief Returns first index of codepoint.
 * \param string String.
 * \param codepoint Codepoint to get index of.
 * \return Index of codepoint or -1 if codepoint was not found in string.
 */
int64_t alfUTF8IndexOf(const AlfChar8* string, uint32_t codepoint);

// -------------------------------------------------------------------------- //

/** Returns the index of the last occurance of a codepoint in a UTF-8 encoded
 * string. If the codepoint is not in the string then -1 is returned instead.
 * \note -1 is also returned if string is NULL.
 * \brief Returns last index of codepoint.
 * \param string String.
 * \param codepoint Codepoint to get index of.
 * \return Index of codepoint of -1 if codepoint was not found in string.
 */
int64_t alfUTF8LastIndexOf(const AlfChar8* string, uint32_t codepoint);

// -------------------------------------------------------------------------- //

/** Returns the offset of the first occurance of a codepoint in a UTF-8 encoded
 * string in code units. If the codepoint is not in the string then -1 is
 * returned instead.
 * \brief Returns first offset of codepoint.
 * \param string String.
 * \param codepoint Codepoint to get offset of.
 * \return Offset of codepoint or -1 if codepoint was not found in string.
 */
int64_t alfUTF8OffsetOf(const AlfChar8* string, uint32_t codepoint);

// -------------------------------------------------------------------------- //

/** Returns the offset of the last occurance of a codepoint in a UTF-8 encoded
 * string in code units. If the codepoint is not in the string then -1 is
 * returned instead.
 * \brief Returns last offset of codepoint.
 * \param string String.
 * \param codepoint Codepoint to get offset of.
 * \return Offset of codepoint or -1 if codepoint was not found in string.
 */
int64_t alfUTF8LastOffsetOf(const AlfChar8* string, uint32_t codepoint);

// -------------------------------------------------------------------------- //

/** Returns whether or not a UTF-8 encoded string starts with the specified
 * codepoint.
 * \note False is returned if string is NULL.
 * \brief Returns whether UTF-8 string starts with codepoint.
 * \param string String to check if starts with codepoint.
 * \param codepoint Codepoint to check if string starts with.
 * \return True if the string starts with the codepoint else false.
 */
AlfBool alfUTF8StartsWith(const AlfChar8* string, uint32_t codepoint);

// -------------------------------------------------------------------------- //

/** Returns whether or not a UTF-8 encoded string ends with the specified
 * codepoint.
 * \note False is returned if string is NULL.
 * \brief Returns whether UTF-8 string ends with codepoint.
 * \param string String to check if ends with codepoint.
 * \param codepoint Codepoint to check if string ends with.
 * \return True if the string ends with the codepoint else false.
 */
AlfBool alfUTF8EndsWith(const AlfChar8* string, uint32_t codepoint);

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
AlfChar8* alfUTF8Substring(
	const AlfChar8* string, 
	uint64_t from, 
	uint64_t count);

// -------------------------------------------------------------------------- //

/** Returns substring of a UTF-8 encoded string from the specified 'from' index
 * to the end of the string.
 * \note Returns NULL if string is NULL.
 * \brief Returns UTF-8 substring.
 * \param string String to get substring of.
 * \param from Index to get substring from.
 * \return Substring or NULL on failure.
 */
AlfChar8* alfUTF8SubstringFrom(const AlfChar8* string, uint64_t from);

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
AlfChar8* alfUTF8Insert(
	const AlfChar8* string, 
	uint64_t from, 
	uint64_t count, 
	const AlfChar8* insertion);

// -------------------------------------------------------------------------- //

/** Returns the codepoint at a specified index in a UTF-8 encoded string.
 * \brief Returns codepoint at index in UTF-8 string.
 * \param string String to get codepoint at index from.
 * \param index Index to get codepoint at.
 * \return Codepoint at index.
 */
uint32_t alfUTF8AtIndex(const AlfChar8* string, uint64_t index);

// -------------------------------------------------------------------------- //

/** Returns the byte offset for a specified codepoint index in a UTF-8 encoded
 * string.
 * \note Returns 0 if the string is NULL.
 * \brief Returns byte offset of codepoint index.
 * \param string String to get byte offset of index in.
 * \param index Index to get byte offset of.
 * \return Byte offset of index.
 */
uint64_t alfUTF8OffsetOfIndex(const AlfChar8* string, uint64_t index);

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
	const AlfChar8* string, 
	uint64_t* indices, 
	uint32_t indexCount, 
	uint32_t* indicesSet);

// -------------------------------------------------------------------------- //

/** Returns whether or not a string is a valid UTF-8 encoded string.
 * \brief Returns whether string are valid UTF-8.
 * \param string String to check if valid UTF-8.
 * \return True if the string is valid UTF-8 else false.
 */
AlfBool alfUTF8Valid(const AlfChar8* string);

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
AlfBool alfUTF8Iterate(
	const AlfChar8* string, 
	PFN_AlfUnicodeIterateCodepoint iterateFunc);

// ========================================================================== //
// UTF-16 Functions
// ========================================================================== //

/** Decode a codepoint from an UTF-16 string at the specified offset. The
 * codepoint and number of code units that it's encoded in is returned in 
 * parameter 3 and 4.
 * \note If the function fails to decode a codepoint then the codepoint and
 * number of code units are not written to their respective parameters
 * \note One code unit is 2 bytes for UTF-16.
 * \brief Decode UTF-8 codepoint
 * \param string String to decode codepoint in.
 * \param offset Offset to decode at in string (in code units).
 * \param codepoint Codepoint.
 * \param numCodeUnits Number of code units that codepoint is encoded in. This 
 * is either 1 or 2.
 * \return True if a valid unicode codepoint could be decoded else false.
 */
AlfBool alfUTF16Decode(
	const AlfChar16* string,
	uint64_t offset,
	uint32_t* codepoint,
	uint32_t* numCodeUnits);

// -------------------------------------------------------------------------- //

/** Encode a codepoint in UTF-16 at a specified offset in a string. The string 
 * is expected to be large enough to hold the code units for the encoded 
 * codepoint.
 * \note One code unit is 2 bytes for UTF-16.
 * \brief Encode codepoint in UTF-8.
 * \param string String to encode codepoint into.
 * \param offset Offset to encode codepoint at (in code units).
 * \param codepoint Codepoint to encode into string.
 * \param numCodeUnits Number of code units that codepoint was encoded in. This
 * is either 1 or 2.
 * \return Returns true if the encoding is successfull, else false.
 */
AlfBool alfUTF16Encode(
	AlfChar16* string,
	uint64_t offset,
	uint32_t codepoint,
	uint32_t* numCodeUnits);

// -------------------------------------------------------------------------- //

/** Returns the width of a codepoint in number of code units when encoded as 
 * UTF-16.
 * \brief Returns UTF-16 codepoint code-unit-width.
 * \param codepoint Codepoint to get width of.
 * \return number of bytes that codepoint is encoded in. 0 is returned if the
 * codepoint is not a valid unicode codepoint.
 */
uint32_t alfUTF16CodepointWidth(uint32_t codepoint);

// -------------------------------------------------------------------------- //

/** Returns the length of an UTF-16 encoded string in number of codepoints.
 * \brief Returns UTF-8 string length.
 * \param string String to get length of.
 * \return Length of string.
 */
uint64_t alfUTF16StringLength(const AlfChar16* string);

// -------------------------------------------------------------------------- //

/** Returns the index of the first occurance of a codepoint in a UTF-16 encoded
 * string. If the codepoint is not in the string then -1 is returned instead.
 * \brief Returns first index of codepoint.
 * \param string String.
 * \param codepoint Codepoint to get index of.
 * \return Index of codepoint or -1 if codepoint was not found in string.
 */
int64_t alfUTF16IndexOf(const AlfChar16* string, uint32_t codepoint);

// -------------------------------------------------------------------------- //

/** Returns the index of the last occurance of a codepoint in a UTF-16 encoded
 * string. If the codepoint is not in the string then -1 is returned instead.
 * \note -1 is also returned if string is NULL.
 * \brief Returns last index of codepoint.
 * \param string String.
 * \param codepoint Codepoint to get index of.
 * \return Index of codepoint of -1 if codepoint was not found in string.
 */
int64_t alfUTF16LastIndexOf(const AlfChar16* string, uint32_t codepoint);

// -------------------------------------------------------------------------- //

/** Returns the offset of the first occurance of a codepoint in a UTF-16 encoded
 * string in code units. If the codepoint is not in the string then -1 is 
 * returned instead.
 * \brief Returns first offset of codepoint.
 * \param string String.
 * \param codepoint Codepoint to get offset of.
 * \return Offset of codepoint or -1 if codepoint was not found in string.
 */
int64_t alfUTF16OffsetOf(const AlfChar16* string, uint32_t codepoint);

// -------------------------------------------------------------------------- //

/** Returns the offset of the last occurance of a codepoint in a UTF-16 encoded
 * string in code units. If the codepoint is not in the string then -1 is
 * returned instead.
 * \brief Returns last offset of codepoint.
 * \param string String.
 * \param codepoint Codepoint to get offset of.
 * \return Offset of codepoint or -1 if codepoint was not found in string.
 */
int64_t alfUTF16LastOffsetOf(const AlfChar16* string, uint32_t codepoint);

// ========================================================================== //
// Conversion Functions
// ========================================================================== //

/** Convert a string that is encoded in UTF-16 to a string that is encoded in 
 * UTF-8. The function returns false if the input string is not valid UTF-16.
 * \note The user is responsible for making sure that the buffer is large enough
 * to be able to contain the entire UTF-8 encoded string. To retrieve the number
 * of bytes required this function can be called with a NULL buffer, in which 
 * case only the number of bytes parameter will be set.
 * \brief Convert UTF-16 to UTF-8 string.
 * \param string String to convert from.
 * \param numBytes Set to the number of bytes that the output string occupies.
 * This is excluding the nul-terminator which the user must take into account 
 * when allocating the buffer.
 * \param buffer Buffer to encode string into. If this is NULL then no encoding
 * is done.
 * \return True if the operation was successful, otherwise false. Success can 
 * either mean that the string was correctly converted or that the number of 
 * bytes was successfully determined.
 */
AlfBool alfUTF16ToUTF8(
	const AlfChar16* string, 
	uint32_t* numBytes, 
	AlfChar8* buffer);

// -------------------------------------------------------------------------- //

/** Convert a string that is encoded in UTF-8 ti a string that is encoded in 
 * UTF-16. The functions returns false if the input string is not valid UTF-8.
 * \note The user is responsible for making sure that the buffer is large enough
 * to be able to contain the entire UTF-16 encoded string. To retrieve the 
 * number of code units required this function can be called with a NULL buffer,
 * in  which case only the number of code units parameter will be set.
 * \brief Convert UTF-8 to UTF-16 string.
 * \param[in] string String to convert from.
 * \param[out] numCodeUnits Set to the number of code units that the output 
 * string occupies when encoded.
 * \param[in] buffer Buffer to encode string into. If this is NULL then no 
 * encoding is done and only the number of code units are returned.
 * \return True if the operation was successful, otherwise false. Success can 
 * either mean that the string was correctly converted or that the number of 
 * bytes was successfully determined.
 */
AlfBool alfUTF8ToUTF16(
	const AlfChar8* string,
	uint32_t* numCodeUnits,
	AlfChar16* buffer);

// -------------------------------------------------------------------------- //

AlfBool alfUTF8FromCodepointList(
	const uint32_t* codepoints,
	uint32_t count,
	uint32_t* numBytes,
	AlfChar8* buffer);

// ========================================================================== //
// End of Header
// ========================================================================== //

#if defined(__cplusplus)
}
#endif

#endif // ALF_UNICODE_H