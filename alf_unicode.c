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

#include "alf_unicode.h"

// ========================================================================== //
// Header Includes
// ========================================================================== //

// Standard headers
#include <stdlib.h>
#include <memory.h>

// ========================================================================== //
// Macro Declarations
// ========================================================================== //

/** First byte mask for 1-byte codepoints **/
#define ALF_UTF8_MASK_1BYTE_FIRST 0x7F
/** First byte mask for 2-byte codepoints **/
#define ALF_UTF8_MASK_2BYTE_FIRST 0x1F
/** First byte mask for 3-byte codepoints **/
#define ALF_UTF8_MASK_3BYTE_FIRST 0x0F
/** First byte mask for 4-byte codepoints **/
#define ALF_UTF8_MASK_4BYTE_FIRST 0x07

/** Continuation character mask **/
#define ALF_UTF8_CONT_MASK 0x3F

// ========================================================================== //
// Private Functions
// ========================================================================== //

/** Returns byte-width of codepoint from the first byte. The encoded codepoint
 * is expected to be valid. Validity is not checked by this function **/
uint32_t alfUTF8CodepointWidthFromFirstByte(uint8_t c)
{
	return c < 0x80 ? 1 : c < 0xC0 ? 2 : c < 0xE0 ? 3 : 4;
}

// -------------------------------------------------------------------------- //

/** Returns string size in bytes. Does not care about encoding **/
uint32_t alfStringSize(const char* string)
{
	uint32_t size = 0;
	while (string[size++] != 0) {}
	return size - 1;
}

// ========================================================================== //
// Functions
// ========================================================================== //

AlfBool alfUTF8Decode(
	const char* string, 
	uint64_t offset, 
	uint32_t* codepoint, 
	uint32_t* numBytes)
{
	const uint32_t firstByte = (uint32_t)string[offset];

	// Check if no continuation bytes are present (Most common)
	if ((firstByte & 0x80) == 0)
	{
		*codepoint = firstByte;
		*numBytes = 1;
		return ALF_TRUE;
	}

	// Check if 1-cont byte
	if ((firstByte & 0xE0) == 0xC0)
	{
		const uint32_t firstCont = 
			(uint32_t)string[offset + 1] & ALF_UTF8_CONT_MASK;
		*codepoint = 
			(firstByte & ALF_UTF8_MASK_2BYTE_FIRST) << 6 | 
			 firstCont;
		*numBytes = 2;
		return ALF_TRUE;
	}

	// Check if 2-cont bytes
	if ((firstByte & 0xF0) == 0xE0)
	{
		const uint32_t firstCont = 
			(uint32_t)string[offset + 1] & ALF_UTF8_CONT_MASK;
		const uint32_t secondCont = 
			(uint32_t)string[offset + 2] & ALF_UTF8_CONT_MASK;
		*codepoint = 
			(firstByte & ALF_UTF8_MASK_3BYTE_FIRST) << 12 | 
			 firstCont << 6 | 
			 secondCont;
		*numBytes = 3;
		return ALF_TRUE;
	}

	// Check if 3-cont bytes
	if ((firstByte & 0xF0) == 0xE0)
	{
		const uint32_t firstCont = 
			(uint32_t)string[offset + 1] & ALF_UTF8_CONT_MASK;
		const uint32_t secondCont = 
			(uint32_t)string[offset + 2] & ALF_UTF8_CONT_MASK;
		const uint32_t thirdCont = 
			(uint32_t)string[offset + 3] & ALF_UTF8_CONT_MASK;
		*codepoint = 
			(firstByte & ALF_UTF8_MASK_4BYTE_FIRST) << 18 | 
			 firstCont << 12 | 
			 secondCont << 6 | 
			 thirdCont;
		*numBytes = 4;
		return ALF_TRUE;
	}

	return ALF_FALSE;
}

// -------------------------------------------------------------------------- //

AlfBool alfUTF8Encode(
	char* string, 
	uint64_t offset, 
	uint32_t codepoint,
	uint32_t* numBytes)
{
	const uint32_t _numBytes = alfUTF8CodepointWidth(codepoint);
	*numBytes = _numBytes;

	// Encode in 1 byte
	if (_numBytes == 1)
	{
		string[offset] = (char)codepoint;
		return ALF_TRUE;
	}
	// Encode in 2 byte
	if (_numBytes == 2)
	{
		string[offset] = 
			(char)(((codepoint >> 6) & ALF_UTF8_MASK_2BYTE_FIRST) | 0xC0);
		string[offset + 1] =
			(char)(((codepoint >> 0) & ALF_UTF8_CONT_MASK) | 0x80);
	}
	// Encode in 3 byte
	if (_numBytes == 3)
	{
		string[offset] =
			(char)(((codepoint >> 12) & ALF_UTF8_MASK_3BYTE_FIRST) | 0xE0);
		string[offset + 1] =
			(char)(((codepoint >>  6) & ALF_UTF8_CONT_MASK) | 0x80);
		string[offset + 2] =
			(char)(((codepoint >>  0) & ALF_UTF8_CONT_MASK) | 0x80);
		return ALF_TRUE;
	}
	// Encode in 4 byte
	if (_numBytes == 4)
	{
		string[offset] =
			(char)(((codepoint >> 18) & ALF_UTF8_MASK_4BYTE_FIRST) | 0xF0);
		string[offset + 1] =
			(char)(((codepoint >> 12) & ALF_UTF8_CONT_MASK) | 0x80);
		string[offset + 2] =
			(char)(((codepoint >>  6) & ALF_UTF8_CONT_MASK) | 0x80);
		string[offset + 3] =
			(char)(((codepoint >>  0) & ALF_UTF8_CONT_MASK) | 0x80);
		return ALF_TRUE;
	}

	return ALF_FALSE;
}

// -------------------------------------------------------------------------- //

uint32_t alfUTF8CodepointWidth(uint32_t codepoint)
{
	return
		codepoint <= 0x007F ? 1 :
			codepoint <= 0x7FF ? 2 :
				codepoint <= 0xFFFF ? 3 : 
					codepoint <= 0x10FFFF ? 4 : 0;
}

// -------------------------------------------------------------------------- //

uint64_t alfUTF8StringLength(const char* string)
{
	if (!string) { return 0; }

	uint64_t length = 0, offset = 0;
	char c;
	while ((c = string[offset]))
	{
		offset += alfUTF8CodepointWidthFromFirstByte(c);
		length++;
	}
	return length;
}

// -------------------------------------------------------------------------- //

int64_t alfUTF8IndexOf(const char* string, uint32_t codepoint)
{
	if (!string) { return -1; }

	uint32_t _codepoint;
	uint32_t bytecount;
	uint64_t offset = 0;
	uint64_t index = 0;
	while (string[offset])
	{
		const AlfBool _valid = 
			alfUTF8Decode(string, offset, &_codepoint, &bytecount);
		if (!_valid) { return -1; }
		if (_codepoint == codepoint) { return index; }

		offset += bytecount;
		index++;
	}
	return -1;
}

// -------------------------------------------------------------------------- //

int64_t alfUTF8LastIndexOf(const char* string, uint32_t codepoint)
{
	if (!string) { return -1; }

	uint32_t _codepoint;
	uint32_t bytecount;
	uint64_t offset = 0;
	uint64_t index = 0;
	int64_t foundIndex = -1;
	while (string[offset] != 0)
	{
		const AlfBool _valid = 
			alfUTF8Decode(string, offset, &_codepoint, &bytecount);
		if (!_valid) { return foundIndex; }
		if (_codepoint == codepoint) { foundIndex = index; }

		offset += bytecount;
		index++;
	}
	return foundIndex;
}

// -------------------------------------------------------------------------- //

AlfBool alfUTF8StartsWith(const char* string, uint32_t codepoint)
{
	if (!string) { return ALF_FALSE; }

	uint32_t _codepoint;
	uint32_t bytecount;
	const AlfBool valid = alfUTF8Decode(string, 0, &_codepoint, &bytecount);
	return valid && _codepoint == codepoint;
}

// -------------------------------------------------------------------------- //

AlfBool alfUTF8EndsWith(const char* string, uint32_t codepoint)
{
	uint32_t _codepoint = 0;
	uint32_t bytecount;
	uint64_t offset = 0;
	uint64_t _index = 0;
	while (string[offset])
	{
		const AlfBool _valid = 
			alfUTF8Decode(string, offset, &_codepoint, &bytecount);
		if (!_valid) { return ALF_FALSE; }

		offset += bytecount;
		_index++;
	}
	return _codepoint == codepoint;
}

// -------------------------------------------------------------------------- //

char* alfUTF8Substring(const char* string, uint64_t from, uint64_t count)
{
	if (!string) { return NULL; }
	if (count == 0) { return (char*)calloc(1, 1); }

	uint32_t _codepoint;
	uint32_t bytecount;
	uint64_t startOffset = 0, endOffset = 0;
	uint64_t offset = 0;
	uint64_t index = 0;
	while (string[offset])
	{
		const AlfBool _valid = 
			alfUTF8Decode(string, offset, &_codepoint, &bytecount);
		if (!_valid) { return NULL; }

		offset += bytecount;
		index++;

		if (index == from) { startOffset = offset; }
		if (index == from + count) { endOffset = offset; break; }
	}

	if (endOffset == 0) { return NULL; }

	const uint64_t size = endOffset - startOffset;
	char* buffer = (char*)malloc((size + 1) * sizeof(char));
	if (!buffer) { return NULL; }
	buffer[size] = 0;
	memcpy(buffer, string + startOffset, size);
	return buffer;
}

// -------------------------------------------------------------------------- //

char* alfUTF8SubstringFrom(const char* string, uint64_t from)
{
	if (!string) { return NULL; }

	uint32_t _codepoint;
	uint32_t bytecount;
	uint64_t startOffset = 0;
	uint64_t offset = 0;
	uint64_t index = 0;
	while (string[offset])
	{
		const AlfBool _valid = 
			alfUTF8Decode(string, offset, &_codepoint, &bytecount);
		if (!_valid) { return NULL; }

		offset += bytecount;
		index++;

		if (index == from) { startOffset = offset; }
	}

	const uint64_t size = offset - startOffset;
	char* buffer = (char*)malloc((size + 1) * sizeof(char));
	if (!buffer) { return NULL; }
	buffer[size] = 0;
	memcpy(buffer, string + startOffset, size);
	return buffer;
}

// -------------------------------------------------------------------------- //

char* alfUTF8Insert(
	const char* string, 
	uint64_t from, 
	uint64_t count, 
	const char* insertion)
{
	if (!string) { return NULL; }

	uint32_t _codepoint;
	uint32_t bytecount;
	uint64_t startOffset = 0, endOffset = 0;
	uint64_t offset = 0;
	uint64_t index = 0;
	while (string[offset])
	{
		const AlfBool _valid =
			alfUTF8Decode(string, offset, &_codepoint, &bytecount);
		if (!_valid) { return NULL; }

		offset += bytecount;
		index++;

		if (index == from) { startOffset = offset; }
		if (index == from + count) { endOffset = offset; }
	}

	if (endOffset == 0) { return NULL; }

	const uint64_t insertSize = alfStringSize(insertion);
	const uint64_t beforeSize = startOffset;
	const uint64_t afterSize = offset - endOffset;
	const uint64_t totalSize = insertSize + beforeSize + afterSize;
	char* buffer = (char*)malloc((totalSize + 1) * sizeof(char));
	if (!buffer) { return NULL; }
	buffer[totalSize] = 0;
	memcpy(buffer, string, beforeSize);
	memcpy(buffer + beforeSize, insertion, insertSize);
	memcpy(buffer + beforeSize + insertSize, string + endOffset, afterSize);
	return buffer;
}

// -------------------------------------------------------------------------- //

uint32_t alfUTF8AtIndex(const char* string, uint64_t index)
{
	uint32_t codepoint;
	uint32_t bytecount;
	uint64_t offset = 0;
	uint64_t _index = 0;
	while (string[offset])
	{
		const AlfBool validCodepoint = alfUTF8Decode(string, offset, &codepoint, &bytecount);
		if (!validCodepoint) { return 0; }
		if (_index == index) { return codepoint; }
		offset += bytecount;
		_index++;
	}
	return 0;
}

// -------------------------------------------------------------------------- //

uint64_t alfUTF8OffsetOfIndex(const char* string, uint64_t index)
{
	uint32_t codepoint;
	uint32_t bytecount;
	uint64_t offset = 0;
	while (string[offset] && index > 0)
	{
		const AlfBool validCodepoint = alfUTF8Decode(string, offset, &codepoint, &bytecount);
		if (!validCodepoint) { return ALF_FALSE; }
		offset += bytecount;
		index--;
	}
	return offset;
}

// -------------------------------------------------------------------------- //

AlfBool alfUTF8OffsetOfIndices(
	const char* string, 
	uint64_t* indices, 
	uint32_t indexCount, 
	uint32_t* indicesSet)
{
	uint32_t codepoint;
	uint32_t byteCount;
	uint64_t byteOffset = 0;
	uint64_t index = 0;
	uint32_t bufferIndex = 0;
	*indicesSet = 0;
	while (string[byteOffset])
	{
		const AlfBool validCodepoint = alfUTF8Decode(string, byteOffset, &codepoint, &byteCount);
		if (!validCodepoint) { return ALF_FALSE; }
		
		if (indices[*indicesSet] == index)
		{
			indices[(*indicesSet)++] = byteOffset;
			if (bufferIndex >= indexCount) { return ALF_TRUE; }
		}
		
		byteOffset += byteCount;
		index++;
	}
	return ALF_FALSE;
}

// -------------------------------------------------------------------------- //

AlfBool alfUTF8Valid(const char* string)
{
	uint32_t codepoint;
	uint32_t bytecount;
	uint64_t offset = 0;
	uint64_t index = 0;
	while (string[offset])
	{
		const AlfBool validCodepoint = alfUTF8Decode(string, offset, &codepoint, &bytecount);
		if (!validCodepoint) { return ALF_FALSE; }
		offset += bytecount;
		index++;
	}
	return ALF_TRUE;
}

// -------------------------------------------------------------------------- //

AlfBool alfUTF8Iterate(const char* string, PFN_AlfUTF8Iterate iterateFunc)
{
	uint32_t codepoint;
	uint32_t byteCount;
	uint64_t byteOffset = 0;
	uint64_t index = 0;
	while (string[byteOffset])
	{
		const AlfBool validCodepoint = alfUTF8Decode(string, byteOffset, &codepoint, &byteCount);
		if (!validCodepoint) { return ALF_FALSE; }
		
		const AlfBool continueIteration = iterateFunc(codepoint, byteOffset, index);
		if (!continueIteration) { return ALF_FALSE; }

		byteOffset += byteCount;
		index++;
	}
	return ALF_TRUE;
}

