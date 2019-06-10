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

#include "alf_unicode.h"

// ========================================================================== //
// Header Includes
// ========================================================================== //

// Standard headers
#include <memory.h>
#include <stdlib.h>

// ========================================================================== //
// Private Functions
// ========================================================================== //

/** Returns string size in bytes. Does not care about encoding **/
uint32_t
alfStringSize(const AlfChar8* string)
{
  uint32_t size = 0;
  while (string[size++] != 0) {
  }
  return size - 1;
}

// ========================================================================== //
// UTF-8 Macro Declarations
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
// UTF-8 Private Functions
// ========================================================================== //

/** Returns byte-width of codepoint from the first byte. The encoded codepoint
 * is expected to be valid. Validity is not checked by this function **/
uint32_t
alfUTF8CodepointWidthFromFirstByte(uint8_t c)
{
  return c < 0xC0 ? 1 : c < 0xE0 ? 2 : c < 0xF0 ? 3 : 4;
}

// ========================================================================== //
// UTF-8 Functions
// ========================================================================== //

AlfBool
alfUTF8Decode(const AlfChar8* string,
              uint64_t offset,
              uint32_t* codepoint,
              uint32_t* numBytes)
{
  const uint32_t firstByte = (uint32_t)string[offset];

  // Check if no continuation bytes are present (Most common)
  if ((firstByte & 0x80) == 0) {
    *codepoint = firstByte;
    *numBytes = 1;
    return ALF_TRUE;
  }

  // Check if 1-cont byte
  if ((firstByte & 0xE0) == 0xC0) {
    const uint32_t firstCont =
      (uint32_t)string[offset + 1] & ALF_UTF8_CONT_MASK;
    *codepoint = (firstByte & ALF_UTF8_MASK_2BYTE_FIRST) << 6 | firstCont;
    *numBytes = 2;
    return ALF_TRUE;
  }

  // Check if 2-cont bytes
  if ((firstByte & 0xF0) == 0xE0) {
    const uint32_t firstCont =
      (uint32_t)string[offset + 1] & ALF_UTF8_CONT_MASK;
    const uint32_t secondCont =
      (uint32_t)string[offset + 2] & ALF_UTF8_CONT_MASK;
    *codepoint = (firstByte & ALF_UTF8_MASK_3BYTE_FIRST) << 12 |
                 firstCont << 6 | secondCont;
    *numBytes = 3;
    return ALF_TRUE;
  }

  // Check if 3-cont bytes
  if ((firstByte & 0xF8) == 0xF0) {
    const uint32_t firstCont =
      (uint32_t)string[offset + 1] & ALF_UTF8_CONT_MASK;
    const uint32_t secondCont =
      (uint32_t)string[offset + 2] & ALF_UTF8_CONT_MASK;
    const uint32_t thirdCont =
      (uint32_t)string[offset + 3] & ALF_UTF8_CONT_MASK;
    *codepoint = (firstByte & ALF_UTF8_MASK_4BYTE_FIRST) << 18 |
                 firstCont << 12 | secondCont << 6 | thirdCont;
    *numBytes = 4;
    return ALF_TRUE;
  }

  return ALF_FALSE;
}

// -------------------------------------------------------------------------- //

AlfBool
alfUTF8Encode(AlfChar8* string,
              uint64_t offset,
              uint32_t codepoint,
              uint32_t* numBytes)
{
  const uint32_t _numBytes = alfUTF8CodepointWidth(codepoint);
  if (numBytes) {
    *numBytes = _numBytes;
  }

  // Encode in 1 byte
  if (_numBytes == 1) {
    string[offset] = (AlfChar8)codepoint;
    return ALF_TRUE;
  }
  // Encode in 2 byte
  if (_numBytes == 2) {
    string[offset] =
      (AlfChar8)(((codepoint >> 6) & ALF_UTF8_MASK_2BYTE_FIRST) | 0xC0);
    string[offset + 1] =
      (AlfChar8)(((codepoint >> 0) & ALF_UTF8_CONT_MASK) | 0x80);
    return ALF_TRUE;
  }
  // Encode in 3 byte
  if (_numBytes == 3) {
    string[offset] =
      (AlfChar8)(((codepoint >> 12) & ALF_UTF8_MASK_3BYTE_FIRST) | 0xE0);
    string[offset + 1] =
      (AlfChar8)(((codepoint >> 6) & ALF_UTF8_CONT_MASK) | 0x80);
    string[offset + 2] =
      (AlfChar8)(((codepoint >> 0) & ALF_UTF8_CONT_MASK) | 0x80);
    return ALF_TRUE;
  }
  // Encode in 4 byte
  if (_numBytes == 4) {
    string[offset] =
      (AlfChar8)(((codepoint >> 18) & ALF_UTF8_MASK_4BYTE_FIRST) | 0xF0);
    string[offset + 1] =
      (AlfChar8)(((codepoint >> 12) & ALF_UTF8_CONT_MASK) | 0x80);
    string[offset + 2] =
      (AlfChar8)(((codepoint >> 6) & ALF_UTF8_CONT_MASK) | 0x80);
    string[offset + 3] =
      (AlfChar8)(((codepoint >> 0) & ALF_UTF8_CONT_MASK) | 0x80);
    return ALF_TRUE;
  }

  return ALF_FALSE;
}

// -------------------------------------------------------------------------- //

uint32_t
alfUTF8CodepointWidth(uint32_t codepoint)
{
  return codepoint <= 0x007F
           ? 1
           : codepoint <= 0x7FF
               ? 2
               : codepoint <= 0xFFFF ? 3 : codepoint <= 0x10FFFF ? 4 : 0;
}

// -------------------------------------------------------------------------- //

int32_t
alfUTF8OffsetToNextCodepoint(const AlfChar8* string, uint32_t offset)
{
  const AlfChar8 firstByte = string[offset];
  if ((firstByte & 0x80) == 0) {
    return 1;
  }
  if ((firstByte & 0xE0) == 0xC0) {
    return 2;
  }
  if ((firstByte & 0xF0) == 0xE0) {
    return 3;
  }
  if ((firstByte & 0xF0) == 0xE0) {
    return 4;
  }
  return -1;
}

// -------------------------------------------------------------------------- //

AlfBool
alfUTF8NextWord(const AlfChar8* string,
                uint32_t offset,
                uint32_t* startOffset,
                uint32_t* numBytes,
                PFN_AlfUnicodeCodepointPredicate isWhitespace)
{
  // Read past any whitespaces
  uint32_t codepoint, _numBytes;
  alfUTF8Decode(string, offset, &codepoint, &_numBytes);
  while (codepoint && isWhitespace(codepoint)) {
    offset += _numBytes;
    alfUTF8Decode(string, offset, &codepoint, &_numBytes);
  }
  if (!codepoint) {
    return ALF_FALSE;
  }

  // Find entire word
  const uint32_t wordStartOffset = offset;
  while (codepoint && !isWhitespace(codepoint)) {
    offset += _numBytes;
    alfUTF8Decode(string, offset, &codepoint, &_numBytes);
  }

  // Finalize
  *startOffset = wordStartOffset;
  *numBytes = (offset - wordStartOffset);
  return ALF_TRUE;
}

// -------------------------------------------------------------------------- //

uint64_t
alfUTF8StringLength(const AlfChar8* string)
{
  if (!string) {
    return 0;
  }

  uint64_t length = 0, offset = 0;
  AlfChar8 c;
  while ((c = string[offset])) {
    offset += alfUTF8CodepointWidthFromFirstByte(c);
    length++;
  }
  return length;
}

// -------------------------------------------------------------------------- //

int64_t
alfUTF8IndexOf(const AlfChar8* string, uint32_t codepoint)
{
  if (!string) {
    return -1;
  }

  uint32_t _codepoint;
  uint32_t bytecount;
  uint64_t offset = 0;
  uint64_t index = 0;
  while (string[offset]) {
    const AlfBool _valid =
      alfUTF8Decode(string, offset, &_codepoint, &bytecount);
    if (!_valid) {
      return -1;
    }
    if (_codepoint == codepoint) {
      return index;
    }

    offset += bytecount;
    index++;
  }
  return -1;
}

// -------------------------------------------------------------------------- //

int64_t
alfUTF8LastIndexOf(const AlfChar8* string, uint32_t codepoint)
{
  if (!string) {
    return -1;
  }

  uint32_t _codepoint;
  uint32_t bytecount;
  uint64_t offset = 0;
  uint64_t index = 0;
  int64_t foundIndex = -1;
  while (string[offset] != 0) {
    const AlfBool _valid =
      alfUTF8Decode(string, offset, &_codepoint, &bytecount);
    if (!_valid) {
      return foundIndex;
    }
    if (_codepoint == codepoint) {
      foundIndex = index;
    }

    offset += bytecount;
    index++;
  }
  return foundIndex;
}

// -------------------------------------------------------------------------- //

int64_t
alfUTF8OffsetOf(const AlfChar8* string, uint32_t codepoint)
{
  if (!string) {
    return -1;
  }

  uint32_t _codepoint;
  uint32_t bytecount;
  uint64_t offset = 0;
  while (string[offset]) {
    const AlfBool _valid =
      alfUTF8Decode(string, offset, &_codepoint, &bytecount);
    if (!_valid) {
      return -1;
    }
    if (_codepoint == codepoint) {
      return offset;
    }

    offset += bytecount;
  }
  return -1;
}

// -------------------------------------------------------------------------- //

int64_t
alfUTF8LastOffsetOf(const AlfChar8* string, uint32_t codepoint)
{
  if (!string) {
    return -1;
  }

  uint32_t _codepoint;
  uint32_t bytecount;
  uint64_t offset = 0;
  int64_t foundOffset = -1;
  while (string[offset] != 0) {
    const AlfBool _valid =
      alfUTF8Decode(string, offset, &_codepoint, &bytecount);
    if (!_valid) {
      return foundOffset;
    }
    if (_codepoint == codepoint) {
      foundOffset = offset;
    }

    offset += bytecount;
  }
  return foundOffset;
}

// -------------------------------------------------------------------------- //

AlfBool
alfUTF8StartsWith(const AlfChar8* string, uint32_t codepoint)
{
  if (!string) {
    return ALF_FALSE;
  }

  uint32_t _codepoint;
  uint32_t bytecount;
  const AlfBool valid = alfUTF8Decode(string, 0, &_codepoint, &bytecount);
  return valid && _codepoint == codepoint;
}

// -------------------------------------------------------------------------- //

AlfBool
alfUTF8EndsWith(const AlfChar8* string, uint32_t codepoint)
{
  const uint32_t width = alfUTF8CodepointWidth(codepoint);
  const uint32_t size = alfStringSize(string);
  if (width > size) {
    return ALF_FALSE;
  }

  // Decode at expected location from end
  uint32_t _codepoint, numBytes;
  if (alfUTF8Decode(string, size - width, &_codepoint, &numBytes)) {
    return _codepoint == codepoint;
  }
  return ALF_FALSE;
}

// -------------------------------------------------------------------------- //

AlfChar8*
alfUTF8Substring(const AlfChar8* string, uint64_t from, uint64_t count)
{
  if (!string) {
    return NULL;
  }
  if (count == 0 || from >= alfUTF8StringLength(string)) {
    return (AlfChar8*)calloc(1, 1);
  }

  uint32_t _codepoint;
  uint32_t bytecount;
  uint64_t startOffset = 0, endOffset = 0;
  uint64_t offset = 0;
  uint64_t index = 0;
  while (string[offset]) {
    const AlfBool _valid =
      alfUTF8Decode(string, offset, &_codepoint, &bytecount);
    if (!_valid) {
      return NULL;
    }

    offset += bytecount;
    index++;

    if (index == from) {
      startOffset = offset;
    }
    if (index == from + count) {
      endOffset = offset;
      break;
    }
  }

  if (endOffset == 0) {
    endOffset = offset;
  }

  const uint64_t size = endOffset - startOffset;
  AlfChar8* buffer = (AlfChar8*)malloc((size + 1) * sizeof(AlfChar8));
  if (!buffer) {
    return NULL;
  }
  buffer[size] = 0;
  memcpy(buffer, string + startOffset, size);
  return buffer;
}

// -------------------------------------------------------------------------- //

AlfBool
alfUTF8SubstringRange(const AlfChar8* string,
                      uint64_t from,
                      uint64_t count,
                      AlfUnicodeRange* range)
{
  *range = (AlfUnicodeRange){ 0, 0 };
  if (!string || count == 0 || from >= alfUTF8StringLength(string)) {
    return ALF_FALSE;
  }

  uint32_t _codepoint;
  uint32_t bytecount;
  uint64_t startOffset = 0, endOffset = 0;
  uint64_t offset = 0;
  uint64_t index = 0;
  while (string[offset]) {
    const AlfBool _valid =
      alfUTF8Decode(string, offset, &_codepoint, &bytecount);
    if (!_valid) {
      return ALF_FALSE;
    }

    offset += bytecount;
    index++;

    if (index == from) {
      startOffset = offset;
    }
    if (index == from + count) {
      endOffset = offset;
      break;
    }
  }

  // If we did not find end then set to last index
  if (endOffset == 0) {
    return endOffset == offset;
  }

  // Set the range
  const uint64_t size = endOffset - startOffset;
  range->offset = startOffset;
  range->size = size;
  return ALF_TRUE;
}

// -------------------------------------------------------------------------- //

AlfChar8*
alfUTF8SubstringFrom(const AlfChar8* string, uint64_t from)
{
  if (!string) {
    return NULL;
  }

  uint32_t _codepoint;
  uint32_t bytecount;
  uint64_t startOffset = 0;
  uint64_t offset = 0;
  uint64_t index = 0;
  while (string[offset]) {
    const AlfBool _valid =
      alfUTF8Decode(string, offset, &_codepoint, &bytecount);
    if (!_valid) {
      return NULL;
    }

    offset += bytecount;
    index++;

    if (index == from) {
      startOffset = offset;
    }
  }

  const uint64_t size = offset - startOffset;
  AlfChar8* buffer = (AlfChar8*)malloc((size + 1) * sizeof(AlfChar8));
  if (!buffer) {
    return NULL;
  }
  buffer[size] = 0;
  memcpy(buffer, string + startOffset, size);
  return buffer;
}

// -------------------------------------------------------------------------- //

AlfChar8*
alfUTF8Insert(const AlfChar8* string,
              uint64_t from,
              uint64_t count,
              const AlfChar8* insertion)
{
  if (!string) {
    return NULL;
  }

  uint32_t _codepoint;
  uint32_t bytecount;
  uint64_t startOffset = 0, endOffset = 0;
  uint64_t offset = 0;
  uint64_t index = 0;
  while (string[offset]) {
    const AlfBool _valid =
      alfUTF8Decode(string, offset, &_codepoint, &bytecount);
    if (!_valid) {
      return NULL;
    }

    offset += bytecount;
    index++;

    if (index == from) {
      startOffset = offset;
    }
    if (index == from + count) {
      endOffset = offset;
    }
  }

  if (endOffset < startOffset) {
    return NULL;
  }

  const uint64_t insertSize = alfStringSize(insertion);
  const uint64_t beforeSize = startOffset;
  const uint64_t afterSize = offset - endOffset;
  const uint64_t totalSize = insertSize + beforeSize + afterSize;
  AlfChar8* buffer = (AlfChar8*)malloc((totalSize + 1) * sizeof(AlfChar8));
  if (!buffer) {
    return NULL;
  }
  buffer[totalSize] = 0;
  memcpy(buffer, string, beforeSize);
  memcpy(buffer + beforeSize, insertion, insertSize);
  memcpy(buffer + beforeSize + insertSize, string + endOffset, afterSize);
  return buffer;
}

// -------------------------------------------------------------------------- //

AlfChar8*
alfUTF8ReplaceCodepoint(AlfChar8* string, uint32_t from, uint32_t to)
{
  const uint32_t width0 = alfUTF8CodepointWidth(from);
  const uint32_t width1 = alfUTF8CodepointWidth(to);
  const uint32_t size = alfStringSize(string);
  AlfChar8* buffer = (AlfChar8*)malloc(
    sizeof(AlfChar8) * size * (uint64_t)((float)width1 / width0) + 1);
  if (!buffer) {
    return NULL;
  }

  // Decode/Encode
  uint32_t offset = 0, writeOffset = 0;
  uint32_t codepoint, numBytes, numWrittenBytes;
  while (alfUTF8Decode(string, offset, &codepoint, &numBytes)) {
    if (codepoint == from) {
      alfUTF8Encode(buffer, writeOffset, to, &numWrittenBytes);
    } else {
      alfUTF8Encode(buffer, writeOffset, codepoint, &numWrittenBytes);
    }
    writeOffset += numWrittenBytes;
    offset += numBytes;
  }
  buffer[writeOffset] = 0;
  return buffer;
}

// -------------------------------------------------------------------------- //

AlfBool
alfUTF8ReplaceCodepointEqualWidth(AlfChar8* string, uint32_t from, uint32_t to)
{
  const uint32_t width0 = alfUTF8CodepointWidth(from);
  const uint32_t width1 = alfUTF8CodepointWidth(to);
  if (width0 != width1) {
    return ALF_FALSE;
  }

  // Encode the codepoint to be able to copy
  AlfChar8 encodedCodepoint[5];
  encodedCodepoint[width0] = 0;
  alfUTF8Encode(encodedCodepoint, 0, to, NULL);

  // Replace all occurances
  uint32_t offset = 0;
  uint32_t codepoint, numBytes;
  while (alfUTF8Decode(string, offset, &codepoint, &numBytes)) {
    if (codepoint == from) {
      memcpy(string + offset, encodedCodepoint, width0 * sizeof(AlfChar8));
    }
    offset += numBytes;
  }
  return ALF_TRUE;
}

// -------------------------------------------------------------------------- //

uint32_t
alfUTF8AtIndex(const AlfChar8* string, uint64_t index)
{
  uint32_t codepoint;
  uint32_t bytecount;
  uint64_t offset = 0;
  uint64_t _index = 0;
  while (string[offset]) {
    const AlfBool validCodepoint =
      alfUTF8Decode(string, offset, &codepoint, &bytecount);
    if (!validCodepoint) {
      return 0;
    }
    if (_index == index) {
      return codepoint;
    }
    offset += bytecount;
    _index++;
  }
  return 0;
}

// -------------------------------------------------------------------------- //

uint64_t
alfUTF8OffsetOfIndex(const AlfChar8* string, uint64_t index)
{
  uint32_t codepoint;
  uint32_t bytecount;
  uint64_t offset = 0;
  while (string[offset] && index > 0) {
    const AlfBool validCodepoint =
      alfUTF8Decode(string, offset, &codepoint, &bytecount);
    if (!validCodepoint) {
      return ALF_FALSE;
    }
    offset += bytecount;
    index--;
  }
  return offset;
}

// -------------------------------------------------------------------------- //

AlfBool
alfUTF8OffsetOfIndices(const AlfChar8* string,
                       uint64_t* indices,
                       uint32_t indexCount,
                       uint32_t* indicesSet)
{
  uint32_t codepoint;
  uint32_t byteCount;
  uint64_t byteOffset = 0;
  uint64_t index = 0;
  const uint32_t bufferIndex = 0;
  *indicesSet = 0;
  while (string[byteOffset]) {
    const AlfBool validCodepoint =
      alfUTF8Decode(string, byteOffset, &codepoint, &byteCount);
    if (!validCodepoint) {
      return ALF_FALSE;
    }

    if (indices[*indicesSet] == index) {
      indices[(*indicesSet)++] = byteOffset;
      if (bufferIndex >= indexCount) {
        return ALF_TRUE;
      }
    }

    byteOffset += byteCount;
    index++;
  }
  return ALF_FALSE;
}

// -------------------------------------------------------------------------- //

AlfBool
alfUTF8Valid(const AlfChar8* string)
{
  uint32_t codepoint;
  uint32_t bytecount;
  uint64_t offset = 0;
  uint64_t index = 0;
  while (string[offset]) {
    const AlfBool validCodepoint =
      alfUTF8Decode(string, offset, &codepoint, &bytecount);
    if (!validCodepoint) {
      return ALF_FALSE;
    }
    offset += bytecount;
    index++;
  }
  return ALF_TRUE;
}

// -------------------------------------------------------------------------- //

AlfBool
alfUTF8Iterate(const AlfChar8* string,
               PFN_AlfUnicodeIterateCodepoint iterateFunc)
{
  uint32_t codepoint;
  uint32_t byteCount;
  uint64_t byteOffset = 0;
  uint64_t index = 0;
  while (string[byteOffset]) {
    const AlfBool validCodepoint =
      alfUTF8Decode(string, byteOffset, &codepoint, &byteCount);
    if (!validCodepoint) {
      return ALF_FALSE;
    }

    const AlfBool continueIteration = iterateFunc(codepoint, byteOffset, index);
    if (!continueIteration) {
      return ALF_FALSE;
    }

    byteOffset += byteCount;
    index++;
  }
  return ALF_TRUE;
}

// ========================================================================== //
// UTF-8 Macro Declarations
// ========================================================================== //

/** First byte mask for 2-byte codepoints **/
#define ALF_UTF16_MASK_1PART_FIRST 0x7F
/** First byte mask for 4-byte codepoints **/
#define ALF_UTF16_MASK_2PART_FIRST 0x1F

/** Continuation character mask **/
#define ALF_UTF16_CONT_MASK 0x3F

// ========================================================================== //
// UTF-16 Private Functions
// ========================================================================== //

/** Returns number of codeunits that a codepoint is encoded in for the UTF-16
 * encoding **/
int32_t
alfUTF16CodepointWidthFromFirstByte(AlfChar16 c)
{
  if (c <= 0xFFFF && (c < 0xD800 || c > 0xDFFF)) {
    return 1;
  }
  if (c <= 0x10FFFF) {
    return 2;
  }
  return -1;
}

// ========================================================================== //
// UTF-16 Functions
// ========================================================================== //

AlfBool
alfUTF16Decode(const AlfChar16* string,
               uint64_t offset,
               uint32_t* codepoint,
               uint32_t* numCodeUnits)
{
  const AlfChar16 firstPart = string[offset];

  // Encoded in single 16-bit value
  const int32_t codeUnitCount = alfUTF16CodepointWidthFromFirstByte(firstPart);
  if (codeUnitCount == 1) {
    *codepoint = (uint32_t)firstPart;
    *numCodeUnits = 1;
    return ALF_TRUE;
  }
  // Encoded as surrogate pair
  if (codeUnitCount == 2) {
    const uint16_t secondPart = string[offset + 1];
    const uint32_t high = (firstPart - 0xD800) * 0x400;
    const uint32_t low = secondPart - 0xDC00;
    *codepoint = high + low + 0x10000;
    *numCodeUnits = 2;
    return ALF_TRUE;
  }
  return ALF_FALSE;
}

// -------------------------------------------------------------------------- //

AlfBool
alfUTF16Encode(AlfChar16* string,
               uint64_t offset,
               uint32_t codepoint,
               uint32_t* numCodeUnits)
{
  const uint32_t codeUnits = alfUTF16CodepointWidth(codepoint);
  *numCodeUnits = codeUnits;

  // Encode
  if (codeUnits == 1) {
    string[offset] = (uint16_t)codepoint;
    return ALF_TRUE;
  }
  // Encode surrogate pair
  if (codeUnits == 2) {
    codepoint -= 0x10000;
    string[offset] = (codepoint >> 10) + 0xD800;
    string[offset + 1] = (codepoint & 0x3FF) + 0xDC00;
    return ALF_TRUE;
  }
  return ALF_FALSE;
}

// -------------------------------------------------------------------------- //

uint32_t
alfUTF16CodepointWidth(uint32_t codepoint)
{
  return codepoint < 0x010000 ? 1 : 2;
}

// -------------------------------------------------------------------------- //

uint64_t
alfUTF16StringLength(const AlfChar16* string)
{
  if (!string) {
    return 0;
  }

  uint64_t length = 0, offset = 0;
  AlfChar16 c;
  while ((c = string[offset])) {
    offset += alfUTF16CodepointWidthFromFirstByte(c);
    length++;
  }
  return length;
}

// -------------------------------------------------------------------------- //

int64_t
alfUTF16IndexOf(const AlfChar16* string, uint32_t codepoint)
{
  if (!string) {
    return -1;
  }

  uint32_t _codepoint;
  uint32_t codeUnitCount;
  uint64_t offset = 0;
  uint64_t index = 0;
  while (string[offset]) {
    const AlfBool _valid =
      alfUTF16Decode(string, offset, &_codepoint, &codeUnitCount);
    if (!_valid) {
      return -1;
    }
    if (_codepoint == codepoint) {
      return index;
    }

    offset += codeUnitCount;
    index++;
  }
  return -1;
}

// -------------------------------------------------------------------------- //

int64_t
alfUTF16LastIndexOf(const AlfChar16* string, uint32_t codepoint)
{
  if (!string) {
    return -1;
  }

  uint32_t _codepoint;
  uint32_t bytecount;
  uint64_t offset = 0;
  uint64_t index = 0;
  int64_t foundIndex = -1;
  while (string[offset] != 0) {
    const AlfBool _valid =
      alfUTF16Decode(string, offset, &_codepoint, &bytecount);
    if (!_valid) {
      return foundIndex;
    }
    if (_codepoint == codepoint) {
      foundIndex = index;
    }

    offset += bytecount;
    index++;
  }
  return foundIndex;
}

// -------------------------------------------------------------------------- //

int64_t
alfUTF16OffsetOf(const AlfChar16* string, uint32_t codepoint)
{
  if (!string) {
    return -1;
  }

  uint32_t _codepoint;
  uint32_t codeUnitCount;
  uint64_t offset = 0;
  while (string[offset]) {
    const AlfBool _valid =
      alfUTF16Decode(string, offset, &_codepoint, &codeUnitCount);
    if (!_valid) {
      return -1;
    }
    if (_codepoint == codepoint) {
      return offset;
    }

    offset += codeUnitCount;
  }
  return -1;
}

// -------------------------------------------------------------------------- //

int64_t
alfUTF16LastOffsetOf(const AlfChar16* string, uint32_t codepoint)
{
  if (!string) {
    return -1;
  }

  uint32_t _codepoint;
  uint32_t bytecount;
  uint64_t offset = 0;
  int64_t foundOffset = -1;
  while (string[offset] != 0) {
    const AlfBool _valid =
      alfUTF16Decode(string, offset, &_codepoint, &bytecount);
    if (!_valid) {
      return foundOffset;
    }
    if (_codepoint == codepoint) {
      foundOffset = offset;
    }

    offset += bytecount;
  }
  return foundOffset;
}

// ========================================================================== //
// Conversion Functions
// ========================================================================== //

AlfBool
alfUTF16ToUTF8(const AlfChar16* string, uint32_t* numBytes, AlfChar8* buffer)
{
  // Decode UTF-16
  uint32_t _numBytes = 0;
  uint32_t offset = 0, encodeOffset = 0;
  uint32_t codepoint, numCodeUnits;
  while (alfUTF16Decode(string, offset, &codepoint, &numCodeUnits)) {
    if (codepoint == 0) {
      break;
    }
    _numBytes += alfUTF8CodepointWidth(codepoint);
    offset += numCodeUnits;

    // Encode UTF-8 if buffer is valid
    if (buffer) {
      uint32_t numEncodeBytes;
      const AlfBool encodeResult =
        alfUTF8Encode(buffer, encodeOffset, codepoint, &numEncodeBytes);
      if (!encodeResult) {
        return ALF_FALSE;
      }
      encodeOffset += numEncodeBytes;
    }
  }
  *numBytes = _numBytes;
  return ALF_TRUE;
}

// -------------------------------------------------------------------------- //

AlfBool
alfUTF8ToUTF16(const AlfChar8* string,
               uint32_t* numCodeUnits,
               AlfChar16* buffer)
{
  // Decode UTF-8
  uint32_t _numCodeUnits = 0;
  uint32_t offset = 0, encodeOffset = 0;
  uint32_t codepoint, numBytes;
  while (alfUTF8Decode(string, offset, &codepoint, &numBytes)) {
    if (codepoint == 0) {
      break;
    }
    _numCodeUnits += alfUTF16CodepointWidth(codepoint);
    offset += numBytes;

    // Encode UTF-8 if buffer is valid
    if (buffer) {
      uint32_t numEncodeCodeUnits;
      const AlfBool encodeResult =
        alfUTF16Encode(buffer, encodeOffset, codepoint, &numEncodeCodeUnits);
      if (!encodeResult) {
        return ALF_FALSE;
      }
      encodeOffset += numEncodeCodeUnits;
    }
  }
  *numCodeUnits = _numCodeUnits;
  return ALF_TRUE;
}

// -------------------------------------------------------------------------- //

AlfBool
alfUTF8FromCodepointList(const uint32_t* codepoints,
                         uint32_t count,
                         uint32_t* numBytes,
                         AlfChar8* buffer)
{
  // Only calculate size of buffer is NULL
  if (!buffer) {
    uint32_t accumulator = 1; // space for nul-terminator
    for (uint32_t i = 0; i < count; i++) {
      accumulator += alfUTF8CodepointWidth(codepoints[i]);
    }
    *numBytes = accumulator;
    return ALF_TRUE;
  }

  // Otherwise encode into buffer
  uint64_t offset = 0;
  uint32_t index = 0;
  for (uint32_t i = 0; i < count; i++) {
    // Check that buffer can hold the encoded codepoint
    const uint32_t codepointWidth = alfUTF8CodepointWidth(codepoints[i]);
    if (offset + codepointWidth > *numBytes) {
      return ALF_FALSE;
    }

    // Encode codepoint
    AlfBool success = alfUTF8Encode(buffer, offset, codepoints[i], NULL);

    offset += codepointWidth;
    index++;
  }

  // Add nul-terminator
  buffer[offset] = 0;
  return ALF_TRUE;
}
