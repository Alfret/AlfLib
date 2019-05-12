// MIT License
//
// Copyright (c) 2019 Filip Björklund
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

#include "alf_file.h"

// ========================================================================== //
// Headers
// ========================================================================== //

// Standard Headers
#include <assert.h>
#include <malloc.h>

// ========================================================================== //
// Platform Specifics
// ========================================================================== //

#if defined(_WIN32)
#define ALF_FILE_TARGET_WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#elif defined(__linux__)
#define ALF_FILE_TARGET_LINUX
#include <limits.h>
#include <unistd.h>
#elif defined(__APPLE__)
#define ALF_FILE_TARGET_APPLE
#else
#error "Invalid target platform"
#endif

// ========================================================================== //
// Macros
// ========================================================================== //

/** Macro for the largest of two numbers **/
#define ALF_FILE_MAX(a, b) a > b ? a : b

// ========================================================================== //
// Private Functions
// ========================================================================== //

/** Returns copy of string **/
static char*
alfFileStringCopy(const char* string)
{
  const size_t length = strlen(string);
  if (length == 0) {
    return NULL;
  }
  char* buffer = malloc(length + 1);
  if (!buffer) {
    return NULL;
  }
  buffer[length] = 0;
  return memcpy(buffer, string, length);
}

// -------------------------------------------------------------------------- //

#if defined(ALF_FILE_TARGET_WIN32)

/** Convert Win32 error to AlfFileResult **/
static enum AlfFileResult
alfFileWin32ResultFromError(DWORD error)
{
  switch (error) {
    case ERROR_SUCCESS:
      return kAlfFileSuccess;
    case ERROR_NOACCESS:
      return kAlfFileAccessDenied;
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
      return kAlfFileNotFound;
    case ERROR_FILE_EXISTS:
      return kAlfFileAlreadyExists;
    default:
      return kAlfFileUnknownError;
  }
}

// -------------------------------------------------------------------------- //

/** Convert UTF-16 to UTF-8 **/
static char*
alfFileUTF16ToUTF8(const wchar_t* wstring)
{
  if (!wstring) {
    return NULL;
  }

  const size_t inLength = wcslen(wstring);
  const int outLength =
    WideCharToMultiByte(CP_UTF8, 0, wstring, inLength, NULL, 0, NULL, NULL);

  char* output = malloc(outLength + 1ull);
  if (!output) {
    return NULL;
  }
  output[outLength] = 0;
  WideCharToMultiByte(CP_UTF8, 0, wstring, inLength, output, outLength, 0, 0);

  return output;
}

// -------------------------------------------------------------------------- //

/** Convert UTF-8 to UTF-16 **/
static wchar_t*
alfFileUTF8ToUTF16(const char* string)
{
  if (!string) {
    return NULL;
  }

  const size_t inLength = strlen(string);
  const int outLength = MultiByteToWideChar(
    CP_UTF8, MB_ERR_INVALID_CHARS, string, (int)inLength, NULL, 0);

  wchar_t* output = malloc((outLength + 1ull) * sizeof(wchar_t));
  if (!output) {
    return NULL;
  }
  output[outLength] = 0;
  MultiByteToWideChar(
    CP_UTF8, MB_ERR_INVALID_CHARS, string, (int)inLength, output, outLength);

  return output;
}

// -------------------------------------------------------------------------- //

static AlfBool
alfFileWin32GetFileAttributes(const char* path,
                              WIN32_FILE_ATTRIBUTE_DATA* attributes)
{
  wchar_t* _path = alfFileUTF8ToUTF16(path);
  const BOOL result =
    GetFileAttributesExW(_path, GetFileExInfoStandard, attributes);
  free(_path);
  return result != 0;
}

// -------------------------------------------------------------------------- //

int64_t
alfFileWin32LastOffsetOfWString(const wchar_t* str, wchar_t chr)
{
  int64_t lastIndex = -1;
  for (size_t i = 0; i < wcslen(str); i++) {
    if (str[i] == chr) {
      lastIndex = i;
    }
  }
  return lastIndex;
}

// -------------------------------------------------------------------------- //

DWORD
alfFileWin32CreateDirectoryRecursively(wchar_t* path)
{
  // Try to create directory
  BOOL result = CreateDirectoryW(path, NULL);
  if (result == 0) {
    // Retrieve error
    const DWORD error = GetLastError();

    // Check if invalid path or other error
    if (error == ERROR_PATH_NOT_FOUND) {
      // Create sub directory
      // Support both '\' and '/' file separators
      const int64_t lastIndex0 = alfFileWin32LastOffsetOfWString(path, '\\');
      const int64_t lastIndex1 = alfFileWin32LastOffsetOfWString(path, '/');
      const int64_t lastIndex = ALF_FILE_MAX(lastIndex0, lastIndex1);

      wchar_t subPath[MAX_PATH];
      if (lastIndex < 0) {
        return ERROR_PATH_NOT_FOUND;
      }
      memcpy(subPath, path, sizeof(wchar_t) * lastIndex);
      subPath[lastIndex] = 0;

      // Recursively create sub-path
      const DWORD recursiveResult =
        alfFileWin32CreateDirectoryRecursively(subPath);
      if (recursiveResult != ERROR_SUCCESS) {
        return recursiveResult;
      }

      // Create this directory
      result = CreateDirectoryW(path, NULL);
      return result == 0 ? GetLastError() : ERROR_SUCCESS;
    }

    // Return other error
    return error;
  }

  // Return success
  return ERROR_SUCCESS;
}

#elif defined(ALF_FILE_TARGET_LINUX)

/** Convert from errno to AlfFileResult **/
static AlfFileResult
alfFileErrorFromErrno(int error)
{
  switch (error) {
      // Flags are incorrect
    case EINVAL:
      return kAlfFileInvalidArguments;
      // Access was denied to file
    case EACCES:
      return kAlfFileAccessDenied;
      // File could not be opened as it does not exist
    case ENOENT:
      return kAlfFileNotFound;
      // Out of memory
    case ENOMEM:
      return kAlfFileOutOfMemory;
      // Unknown file error
    default:
      return kAlfFileUnknownError;
  }
}

#elif defined(ALF_FILE_TARGET_APPLE)

/** Convert from errno to AlfFileResult **/
static AlfFileResult
alfFileErrorFromErrno(int error)
{
  switch (error) {
      // Flags are incorrect
    case EINVAL:
      return kAlfFileInvalidArguments;
      // Access was denied to file
    case EACCES:
      return kAlfFileAccessDenied;
      // File could not be opened as it does not exist
    case ENOENT:
      return kAlfFileNotFound;
      // Out of memory
    case ENOMEM:
      return kAlfFileOutOfMemory;
      // Unknown file error
    default:
      return kAlfFileUnknownError;
  }
}

#endif

// ========================================================================== //
// Path Functions
// ========================================================================== //

AlfBool
alfPathJoin(char* outputPath, const char* path0, const char* path1)
{
  // Determine path information
  const size_t len0 = strlen(path0);
  const size_t len1 = strlen(path1);
  const size_t separatorLength = strlen(ALF_FILE_PATH_SEPARATOR_STR);
  const AlfBool hasTrailing =
    (path0[len0 - 1] == '/' || path0[len0 - 1] == '\\') ? 1 : 0;
  const AlfBool hasPreceding = (path1[0] == '/' || path1[0] == '\\') ? 1 : 0;

  // Check that paths fit
  if (len0 + len1 + 1 >= ALF_FILE_MAX_PATH_LENGTH) {
    return ALF_FALSE;
  }

  // Concatenate path
  size_t offset = 0;
  memcpy(outputPath + offset, path0, len0 - hasTrailing);
  offset += len0 - hasTrailing;
  memcpy(outputPath + offset,
         ALF_FILE_PATH_SEPARATOR_STR,
         strlen(ALF_FILE_PATH_SEPARATOR_STR));
  offset += separatorLength;
  memcpy(outputPath + offset, path1 + hasPreceding, len1 - hasPreceding);
  offset += len1 - hasPreceding;
  outputPath[offset] = 0;

  return ALF_TRUE;
}

// ========================================================================== //
// Filesystem Functions
// ========================================================================== //

void
alfFilesystemGetWorkingDirectory(char* buffer)
{
#if defined(ALF_FILE_TARGET_WIN32)
  wchar_t _buffer[ALF_FILE_MAX_PATH_LENGTH];
  GetCurrentDirectoryW(ALF_FILE_MAX_PATH_LENGTH, _buffer);

  // TODO(Filip Björklund): Let the conversion directly write into the buffer
  char* converted = alfFileUTF16ToUTF8(_buffer);
  const size_t convertedSize = strlen(converted);
  memcpy(buffer, converted, convertedSize);
  buffer[convertedSize] = 0;
  free(converted);
#elif defined(ALF_FILE_TARGET_LINUX) || defined(ALF_FILE_TARGET_MACOS)
  getcwd(buffer, ALF_FILE_MAX_PATH_LENGTH);
#endif
}

// -------------------------------------------------------------------------- //

void
alfFilesystemSetWorkingDirectory(const char* path)
{
#if defined(ALF_FILE_TARGET_WIN32)
  wchar_t* _path = alfFileUTF8ToUTF16(path);
  SetCurrentDirectoryW(_path);
  free(_path);
#elif defined(ALF_FILE_TARGET_LINUX) || defined(ALF_FILE_TARGET_MACOS)
  assert(false && "Not implemented")
#endif
}

// -------------------------------------------------------------------------- //

AlfBool
alfFilesystemPathExists(const char* path)
{
#if defined(ALF_FILE_TARGET_WIN32)
  // Get file attributes
  WIN32_FILE_ATTRIBUTE_DATA attributes;
  const AlfBool success = alfFileWin32GetFileAttributes(path, &attributes);
  if (!success) {
    return ALF_FALSE;
  }

  // Check if the object exists
  return attributes.dwFileAttributes != INVALID_FILE_ATTRIBUTES;
#elif defined(ALF_FILE_TARGET_LINUX)
  assert(0 && "Not implemented");
#elif defined(ALF_FILE_TARGET_MACOS)
  assert(0 && "Not implemented");
#endif
}

// -------------------------------------------------------------------------- //

AlfFileResult
alfFilesystemCreateFile(const char* path, AlfBool override)
{
#if defined(ALF_FILE_TARGET_WIN32)
  wchar_t* _path = alfFileUTF8ToUTF16(path);
  const HANDLE file = CreateFileW(_path,
                                  0,
                                  0,
                                  NULL,
                                  override ? OPEN_ALWAYS : CREATE_NEW,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
  free(_path);

  // Check the result
  if (file == INVALID_HANDLE_VALUE) {
    return alfFileWin32ResultFromError(GetLastError());
  }
  CloseHandle(file);
  return kAlfFileSuccess;
#elif defined(ALF_FILE_TARGET_LINUX)
  assert(0 && "Not implemented");
#elif defined(ALF_FILE_TARGET_MACOS)
  assert(0 && "Not implemented");
#endif
}

// -------------------------------------------------------------------------- //

AlfFileResult
alfFilesystemCreateDirectory(const char* path, AlfBool recursive)
{
#if defined(ALF_FILE_TARGET_WIN32)
  // Check if the directory exists
  if (alfFilesystemPathExists(path)) {
    return kAlfFileAlreadyExists;
  }

  // Recursively create directories
  wchar_t* _path = alfFileUTF8ToUTF16(path);
  if (recursive) {
    alfFileWin32CreateDirectoryRecursively(_path);
  } else {
    const BOOL result = CreateDirectoryW(_path, NULL);
    if (result == 0) {
      const DWORD error = GetLastError();
      return alfFileWin32ResultFromError(error);
    }
  }
  free(_path);
#elif defined(ALF_FILE_TARGET_LINUX)
  assert(0 && "Not implemented");
#elif defined(ALF_FILE_TARGET_MACOS)
  assert(0 && "Not implemented");
#endif

  return kAlfFileSuccess;
}

// -------------------------------------------------------------------------- //

AlfFileResult
alfFilesystemDeleteFile(const char* path)
{
#if defined(ALF_FILE_TARGET_WIN32)
  // Check that it is a file and that it exists
  if (!alfFilesystemPathExists(path) || !alfFilesystemIsFilePath(path)) {
    return kAlfFileNotFound;
  }

  // Delete file
  wchar_t* _path = alfFileUTF8ToUTF16(path);
  if (!_path) {
    return kAlfFileOutOfMemory;
  }
  const BOOL result = DeleteFileW(_path);
  free(_path);

  // Check result
  if (result == FALSE) {
    return alfFileWin32ResultFromError(GetLastError());
  }

  // Success
  return kAlfFileSuccess;
#elif defined(ALF_FILE_TARGET_LINUX)
  assert(0 && "Not implemented");
#elif defined(ALF_FILE_TARGET_MACOS)
  assert(0 && "Not implemented");
#endif
}

// -------------------------------------------------------------------------- //

AlfBool
alfFilesystemIsFilePath(const char* path)
{
#if defined(ALF_FILE_TARGET_WIN32)
  // Get file attributes
  WIN32_FILE_ATTRIBUTE_DATA attributes;
  const AlfBool result = alfFileWin32GetFileAttributes(path, &attributes);
  if (result == FALSE) {
    return ALF_FALSE;
  }

  // Return whether this is a directory or not
  return !(attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
#elif defined(ALF_FILE_TARGET_LINUX)
  assert(0 && "Not implemented");
#elif defined(ALF_FILE_TARGET_MACOS)
  assert(0 && "Not implemented");
#endif
}

// -------------------------------------------------------------------------- //

AlfBool
alfFilesystemIsDirectoryPath(const char* path)
{
#if defined(ALF_FILE_TARGET_WIN32)
  // Get file attributes
  WIN32_FILE_ATTRIBUTE_DATA attributes;
  const AlfBool result = alfFileWin32GetFileAttributes(path, &attributes);
  if (result == FALSE) {
    return ALF_FALSE;
  }

  // Return whether this is a directory or not
  return attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
#elif defined(ALF_FILE_TARGET_LINUX)
  assert(0 && "Not implemented");
#elif defined(ALF_FILE_TARGET_MACOS)
  assert(0 && "Not implemented");
#endif
}

// ========================================================================== //
// File Structures
// ========================================================================== //

typedef struct tag_AlfFile
{
#if defined(ALF_FILE_TARGET_WIN32)
  /** File handle **/
  HANDLE handle;
#elif defined(ALF_FILE_TARGET_LINUX)
  /** File handle **/
  FILE* handle;
#elif defined(ALF_FILE_TARGET_APPLE)
  /** File handle **/
  FILE* handle;
#endif

  /** Path to file **/
  char* path;
  /** Current cursor position **/
  uint64_t cursorPosition;
} tag_AlfFile;

// ========================================================================== //
// File Functions
// ========================================================================== //

AlfFileResult
alfFileOpen(const char* path, AlfFileFlag flags, AlfFile** fileOut)
{
  *fileOut = NULL;

  AlfFile* file = (AlfFile*)malloc(sizeof(AlfFile));
  if (!file) {
    return kAlfFileOutOfMemory;
  }
  file->path = alfFileStringCopy(path);
  file->cursorPosition = 0;

  // Assert preconditions
  const size_t pathSize = strlen(path);
  assert(pathSize > 0 && "Failed to open file, path is empty");

#if defined(ALF_FILE_TARGET_WIN32)
  // Setup file access
  DWORD access = 0;
  if (flags & kAlfFileFlagRead) {
    access |= GENERIC_READ;
  }
  if (flags & kAlfFileFlagWrite) {
    access |= GENERIC_WRITE;
  }

  // Setup file sharing mode
  DWORD share = 0;
  if (flags & kAlfFileFlagShareRead) {
    share |= FILE_SHARE_READ;
  }
  if (flags & kAlfFileFlagShareWrite) {
    share |= FILE_SHARE_WRITE;
  }

  // Setup file disposition
  DWORD disposition = 0;
  if (flags & kAlfFileFlagCreate) {
    disposition = flags & kAlfFileFlagOverwrite ? CREATE_ALWAYS : OPEN_ALWAYS;
  } else {
    disposition =
      flags & kAlfFileFlagOverwrite ? TRUNCATE_EXISTING : OPEN_EXISTING;
  }

  // Setup file attributes
  const DWORD attributes = FILE_ATTRIBUTE_NORMAL;

  // Open the file
  wchar_t* _path = alfFileUTF8ToUTF16(path);
  file->handle =
    CreateFileW(_path, access, share, NULL, disposition, attributes, NULL);
  free(_path);

  // Check the file handle
  if (file->handle == INVALID_HANDLE_VALUE) {
    free(file->path);
    free(file);
    const DWORD error = GetLastError();
    return alfFileWin32ResultFromError(error);
  }
#elif defined(ALF_FILE_TARGET_LINUX) || defined(ALF_FILE_TARGET_MACOS)
  // Create file or check that it exists
  if (flags & kAlfFileFlagCreate) {
    AlfFileResult result =
      alfFilesystemCreateFile(path, flags & kAlfFileFlagOverwrite);
    if (result != ALF_SUCCESS && result != kAlfFileAlreadyExists) {
      free(file->path);
      free(file);
      return result;
    }
  } else if (!alfFilesystemPathExists(path)) {
    free(file->path);
    free(file);
    return IC_FILE_NOT_FOUND;
  }

  // Determine correct mode
  if (flags & kAlfFileFlagAppend) {
    mode = "a+";
  } else if (flags & kAlfFileFlagOverwrite) {
    mode = "w+";
  } else {
    mode = "r+";
  }

  // Open file
  FILE* handle = fopen(path, mode);

  // Handle failure to open
  if (!handle) {
    // Get file error and return corresponding code
    AlfFileResult result = alfFileErrorFromErrno(errno);
    if (result == kAlfFileInvalidArguments) {
      result = kAlfFileNotFound;
    }
    free(file->path);
    free(file);
    return result;
  }
#endif

  // Seek to the end if ASH_FILE_FLAG_APPEND was specified
  if (flags & kAlfFileFlagAppend) {
    alfFileSeekEnd(file);
  }
  // Success
  *fileOut = file;
  return kAlfFileSuccess;
}

// -------------------------------------------------------------------------- //

void
alfFileClose(AlfFile* file)
{
  if (!file) {
    return;
  }

#if defined(ALF_FILE_TARGET_WIN32)
  CloseHandle(file->handle);
#elif defined(ALF_FILE_TARGET_LINUX) || defined(ALF_FILE_TARGET_APPLE)
  fflush(file->handle);
  fclose(file->handle);
#endif

  free(file->path);
  free(file);
}

// -------------------------------------------------------------------------- //

void
alfFileFlush(AlfFile* file)
{
#if defined(ALF_FILE_TARGET_WIN32)
  BOOL result = FlushFileBuffers(file->handle);
  (void)result;
#elif defined(ALF_FILE_TARGET_LINUX)
  fflush(file->handle);
#endif
}

// -------------------------------------------------------------------------- //

void
alfFileSeek(AlfFile* file, uint64_t position)
{
  // Check if we already at position
  if (file->cursorPosition == position) {
    return;
  }

#if defined(ALF_FILE_TARGET_WIN32)
  // Set the seek position
  LARGE_INTEGER seekOffset;
  seekOffset.QuadPart = (LONGLONG)position;

  // Seek
  SetFilePointerEx(file->handle, seekOffset, NULL, FILE_BEGIN);
#elif defined(ALF_FILE_TARGET_LINUX) || defined(ALF_FILE_TARGET_APPLE
  // Seek
  fseeko(file->handle, position, SEEK_CUR);
#endif

  // Update cursor position
  file->cursorPosition = position;
}

// -------------------------------------------------------------------------- //

void
alfFileSeekEnd(AlfFile* file)
{
#if defined(ALF_FILE_TARGET_WIN32)
  // Set the seek offset
  LARGE_INTEGER seekOffset;
  seekOffset.QuadPart = (LONGLONG)0;

  // New offset
  LARGE_INTEGER newOffset;

  // Seek
  SetFilePointerEx(file->handle, seekOffset, &newOffset, FILE_END);
#elif defined(ALF_FILE_TARGET_LINUX) || defined(ALF_FILE_TARGET_APPLE)
  // Seek
  fseeko(file->handle, 0, SEEK_END);
#endif

  file->cursorPosition = (uint64_t)newOffset.QuadPart;
}

// -------------------------------------------------------------------------- //

AlfFileResult
alfFileRead(const AlfFile* file,
            uint8_t* buffer,
            uint64_t bytesToRead,
            uint64_t* bytesRead)
{
#if defined(ALF_FILE_TARGET_WIN32)
  // Read the file
  DWORD _bytesRead;
  const BOOL result =
    ReadFile(file->handle, buffer, (DWORD)bytesToRead, &_bytesRead, NULL);

  // Check result
  if (result != TRUE) {
    // Return error
    return alfFileWin32ResultFromError(GetLastError());
  }

  // Set read bytes
  if (bytesRead) {
    *bytesRead = _bytesRead;
  }
#elif defined(ALF_FILE_TARGET_LINUX) || defined(ALF_FILE_TARGET_APPLE)
  // Read file into buffer
  uint64_t _bytesRead = fread(buffer, 1, bytesToRead, file->handle);

  // Check if everything was read
  if (_bytesRead != bytesToRead) {
    // Chech if end of file was reached
    if (feof(file->handle)) {
      return kAlfFileEof;
    }
    return alfFileErrorFromErrno(ferror(file->handle));
  }

  // Set read bytes
  if (bytesRead) {
    *bytesRead = _bytesRead;
  }
#endif

  // Success
  return kAlfFileSuccess;
}

// -------------------------------------------------------------------------- //

AlfFileResult
alfFileWrite(const AlfFile* file,
             uint8_t* buffer,
             uint64_t bytesToWrite,
             uint64_t* writtenBytes)
{
#if defined(ALF_FILE_TARGET_WIN32)
  // Write to the file
  DWORD _writtenBytes;
  const BOOL result =
    WriteFile(file->handle, buffer, (DWORD)bytesToWrite, &_writtenBytes, NULL);

  // Check result
  if (result != TRUE || _writtenBytes != bytesToWrite) {
    // Return error
    return alfFileWin32ResultFromError(GetLastError());
  }

  // Set number of bytes written
  if (writtenBytes) {
    *writtenBytes = _writtenBytes;
  }
#elif defined(ALF_FILE_TARGET_LINUX) || defined(ALF_FILE_TARGET_APPLE)
  // Write from buffer into file
  uint64_t _writtenBytes = fwrite(buffer, 1, bytesToWrite, file->handle);

  // Check if error occured
  if (_writtenBytes != bytesToWrite) {
    return alfFileErrorFromErrno(ferror(file->handle));
  }

  // Set number of bytes written
  if (writtenBytes) {
    *writtenBytes = _writtenBytes;
  }
#endif

  // Success
  return kAlfFileSuccess;
}

// -------------------------------------------------------------------------- //

uint64_t
alfFileGetSize(const AlfFile* file)
{
#if defined(ALF_FILE_TARGET_WIN32)
  WIN32_FILE_ATTRIBUTE_DATA attributes;
  alfFileWin32GetFileAttributes(file->path, &attributes);
  const uint64_t sizeHigh = (uint64_t)attributes.nFileSizeHigh << sizeof(DWORD);
  const uint64_t sizeLow = attributes.nFileSizeLow;
  return sizeLow | sizeHigh;
#elif defined(ALF_FILE_TARGET_LINUX) || defined(ALF_FILE_TARGET_APPLE)
  // Retrieve file stats
  struct stat fileStats;
  s32 result = stat(file->path, &fileStats);
  if (result != 0) {
    // TODO(Filip Björklund): Handle error by either size or result in
    // return and other as second parameter?
  }

  // Return file size
  return (uint64_t)fileStats.st_size;
#endif
}

// -------------------------------------------------------------------------- //

uint64_t
alfFileGetCursorPosition(const AlfFile* file)
{
  return file->cursorPosition;
}
