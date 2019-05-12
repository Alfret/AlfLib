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

#include "alf_file.h"

// ========================================================================== //
// Headers
// ========================================================================== //

// Standard Headers
#include <assert.h>

// ========================================================================== //
// Platform Specifics
// ========================================================================== //

#if defined(_WIN32)
#	define ALF_FILE_TARGET_WIN32
#	include <windows.h>
#elif defined(__linux__)
#	define ALF_FILE_TARGET_LINUX
#elif defined(__APPLE__)
#	define ALF_FILE_TARGET_APPLE
#else
#	error "Invalid target platform"
# endif

// ========================================================================== //
// Private Functions
// ========================================================================== //

/** Returns copy of string **/
static char* alfFileStringCopy(const char* string)
{
	const size_t length = strlen(string);
	if (length == 0) { return NULL; }
	char* buffer = malloc(length + 1);
	if (!buffer) { return NULL; }
	buffer[length] = 0;
	return memcpy(buffer, string, length);
}

// -------------------------------------------------------------------------- //

#if defined(ALF_FILE_TARGET_WIN32)

/** Convert Win32 error to AlfFileResult **/
static enum AlfFileResult alfFileResultFromWin32Error(DWORD error)
{
	switch (error)
	{
	case ERROR_SUCCESS:
		return ALF_FILE_SUCCESS;
	case ERROR_NOACCESS:
		return ALF_FILE_ACCESS_DENIED;
	case ERROR_FILE_NOT_FOUND:
		return ALF_FILE_NOT_FOUND;
	case ERROR_FILE_EXISTS:
		return ALF_FILE_ALREADY_EXISTS;
	default:
		return ALF_FILE_UNKNOWN_ERROR;
	}
}

// -------------------------------------------------------------------------- //

/** Convert UTF-16 to UTF-8 **/
//static char* alfFileUTF16ToUTF8(const wchar_t* wstring)
//{
//	if (!wstring) { return NULL; }
//
//	const size_t inLength = wcslen(wstring);
//	const int outLength = WideCharToMultiByte(CP_UTF8, 0, wstring, inLength,
//		NULL, 0, NULL, NULL);
//
//	char* output = malloc(outLength + 1ull);
//	if (!output) { return NULL; }
//	output[outLength] = 0;
//	WideCharToMultiByte(CP_UTF8, 0, wstring, inLength,
//		output, outLength, 0, 0);
//
//	return output;
//}

// -------------------------------------------------------------------------- //

/** Convert UTF-8 to UTF-16 **/
static wchar_t* alfFileUTF8ToUTF16(const char* string)
{
	if (!string) { return NULL; }

	const size_t inLength = strlen(string);
	const int outLength = MultiByteToWideChar(
		CP_UTF8, 
		MB_ERR_INVALID_CHARS, 
		string, inLength,
		NULL, 0
	);

	wchar_t* output = malloc((outLength + 1ull) * sizeof(wchar_t));
	if (!output) { return NULL; }
	output[outLength] = 0;
	MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		string, inLength,
		output, outLength
	);

	return output;
}

// -------------------------------------------------------------------------- //

static AlfBool alfFileWin32GetFileAttributes(
	const char* path, 
	WIN32_FILE_ATTRIBUTE_DATA *attributes)
{
	wchar_t* _path = alfFileUTF8ToUTF16(path);
	const BOOL result = 
		GetFileAttributesExW(_path, GetFileExInfoStandard, attributes);
	free(_path);
	return result != 0;
}

#elif defined(ALF_FILE_TARGET_LINUX)

/** Convert from errno to AlfFileResult **/
static AlfFileResult alfFileErrorFromErrno(int error)
{
	switch (error)
	{
		// Flags are incorrect
	case EINVAL: return ALF_FILE_INVALID_ARGUMENTS;
		// Access was denied to file
	case EACCES: return ALF_FILE_ACCESS_DENIED;
		// File could not be opened as it does not exist
	case ENOENT: return ALF_FILE_NOT_FOUND;
		// Out of memory
	case ENOMEM: return ALF_FILE_OUT_OF_MEMORY;
		// Unknown file error
	default: return ALF_FILE_UNKNOWN_ERROR;
	}
}

#elif defined(ALF_FILE_TARGET_APPLE)

/** Convert from errno to AlfFileResult **/
static AlfFileResult alfFileErrorFromErrno(int error)
{
	switch (error)
	{
		// Flags are incorrect
	case EINVAL: return ALF_FILE_INVALID_ARGUMENTS;
		// Access was denied to file
	case EACCES: return ALF_FILE_ACCESS_DENIED;
		// File could not be opened as it does not exist
	case ENOENT: return ALF_FILE_NOT_FOUND;
		// Out of memory
	case ENOMEM: return ALF_FILE_OUT_OF_MEMORY;
		// Unknown file error
	default: return ALF_FILE_UNKNOWN_ERROR;
	}
}

#endif

// ========================================================================== //
// Filesystem Functions
// ========================================================================== //


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

AlfFileResult alfFileOpen(
	const char* path, 
	AlfFileFlag flags, 
	AlfFile** fileOut)
{
	*fileOut = NULL;

	AlfFile* file = (AlfFile*)malloc(sizeof(AlfFile));
	if (!file) { return ALF_FILE_OUT_OF_MEMORY; }
	file->path = alfFileStringCopy(path);
	file->cursorPosition = 0;

	// Assert preconditions
	const size_t pathSize = strlen(path);
	assert(pathSize > 0, "Failed to open file, path is empty");

#if defined(ALF_FILE_TARGET_WIN32)
	// Setup file access
	DWORD access = 0;
	if (flags & ALF_FILE_FLAG_READ) {
		access |= GENERIC_READ;
	}
	if (flags & ALF_FILE_FLAG_WRITE) {
		access |= GENERIC_WRITE;
	}

	// Setup file sharing mode
	DWORD share = 0;
	if (flags & ALF_FILE_FLAG_SHARE_READ)
	{
		share |= FILE_SHARE_READ;
	}
	if (flags & ALF_FILE_FLAG_SHARE_WRITE)
	{
		share |= FILE_SHARE_WRITE;
	}

	// Setup file disposition
	DWORD disposition = 0;
	if (flags & ALF_FILE_FLAG_CREATE) {
		disposition = flags & ALF_FILE_FLAG_OVERWRITE ?
			CREATE_ALWAYS : OPEN_ALWAYS;
	}
	else {
		disposition = flags & ALF_FILE_FLAG_OVERWRITE ?
			TRUNCATE_EXISTING : OPEN_EXISTING;
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
		return alfFileResultFromWin32Error(error);
	}
#elif defined(ALF_FILE_TARGET_LINUX) || defined(ALF_FILE_TARGET_MACOS)
	// Create file or check that it exists
	if (flags & ALF_FILE_FLAG_CREATE)
	{
		AlfFileResult result = 
			alfFilesystemCreateFile(path, flags & ALF_FILE_FLAG_OVERWRITE);
		if (result != ALF_SUCCESS && result != ALF_FILE_ALREADY_EXISTS)
		{
			free(file->path);
			free(file);
			return result;
		}
	}
	else if (!alfFilesystemPathExists(path))
	{
		free(file->path);
		free(file);
		return IC_FILE_NOT_FOUND;
	}

	// Determine correct mode
	if (flags & ALF_FILE_FLAG_APPEND) { mode = "a+"; }
	else if (flags & ALF_FILE_FLAG_OVERWRITE) { mode = "w+"; }
	else { mode = "r+"; }

	// Open file
	FILE* handle = fopen(path, mode);

	// Handle failure to open
	if (!handle)
	{
		// Get file error and return corresponding code
		AlfFileResult result = alfFileErrorFromErrno(errno);
		if (result == ALF_FILE_INVALID_ARGUMENTS)
		{
			result = ALF_FILE_NOT_FOUND;
		}
		free(file->path);
		free(file);
		return result;
	}
#endif

	// Seek to the end if ASH_FILE_FLAG_APPEND was specified
	if (flags & ALF_FILE_FLAG_APPEND) {
		alfFileSeekEnd(file);
	}
	// Success
	*fileOut = file;
	return ALF_FILE_SUCCESS;
}

// -------------------------------------------------------------------------- //

void alfFileClose(AlfFile* file)
{
	if (!file) { return; }

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

void alfFileFlush(AlfFile* file)
{
#if defined(ALF_FILE_TARGET_WIN32)
	BOOL result = FlushFileBuffers(file->handle);
	(void)result;
#elif defined(ALF_FILE_TARGET_LINUX)
	fflush(file->handle);
#endif
}

// -------------------------------------------------------------------------- //

void alfFileSeek(AlfFile* file, uint64_t position)
{
	// Check if we already at position
	if (file->cursorPosition == position) 
	{
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

void alfFileSeekEnd(AlfFile* file)
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

AlfFileResult alfFileRead(
	const AlfFile* file,
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
		return alfFileResultFromWin32Error(GetLastError());
	}

	// Set read bytes
	if (bytesRead) 
	{
		*bytesRead = _bytesRead;
	}
#elif defined(ALF_FILE_TARGET_LINUX) || defined(ALF_FILE_TARGET_APPLE)
	// Read file into buffer
	uint64_t _bytesRead = fread(buffer, 1, bytesToRead, file->handle);

	// Check if everything was read
	if (_bytesRead != bytesToRead)
	{
		// Chech if end of file was reached
		if (feof(file->handle))
		{
			return ALF_FILE_EOF;
		}
		return alfFileErrorFromErrno(ferror(file->handle));
	}

	// Set read bytes
	if (bytesRead)
	{
		*bytesRead = _bytesRead;
	}
#endif

	// Success
	return ALF_FILE_SUCCESS;
}

// -------------------------------------------------------------------------- //

AlfFileResult alfFileWrite(
	const AlfFile* file, 
	uint8_t* buffer, 
	uint64_t bytesToWrite, 
	uint64_t* writtenBytes)
{
#if defined(ALF_FILE_TARGET_WIN32)
	// Write to the file
	DWORD _writtenBytes;
	const BOOL result = WriteFile(
		file->handle, buffer, (DWORD)bytesToWrite, &_writtenBytes, NULL);

	// Check result
	if (result != TRUE || _writtenBytes != bytesToWrite) 
	{
		// Return error
		return alfFileResultFromWin32Error(GetLastError());
	}

	// Set number of bytes written
	if (writtenBytes) 
	{
		*writtenBytes = _writtenBytes;
	}
#elif defined(ALF_FILE_TARGET_LINUX) || defined(ALF_FILE_TARGET_APPLE)
	// Write from buffer into file
	uint64_t _writtenBytes = fwrite(buffer, 1, bytesToWrite, file->handle);

	// Check if error occured
	if (_writtenBytes != bytesToWrite)
	{
		return alfFileErrorFromErrno(ferror(file->handle));
	}

	// Set number of bytes written
	if (writtenBytes) {
		*writtenBytes = _writtenBytes;
	}
#endif

	// Success
	return ALF_FILE_SUCCESS;
}

// -------------------------------------------------------------------------- //

uint64_t alfFileGetSize(const AlfFile* file)
{
#if defined(ALF_FILE_TARGET_WIN32)
	WIN32_FILE_ATTRIBUTE_DATA attributes;
	alfFileWin32GetFileAttributes(file->path, &attributes);
	const uint64_t sizeHigh = 
		(uint64_t)attributes.nFileSizeHigh << sizeof(DWORD);
	const uint64_t sizeLow = attributes.nFileSizeLow;
	return sizeLow | sizeHigh;
#elif defined(ALF_FILE_TARGET_LINUX) || defined(ALF_FILE_TARGET_APPLE)
	// Retrieve file stats
	struct stat fileStats;
	s32 result = stat(file->path, &fileStats);
	if (result != 0)
	{
		// TODO(Filip Björklund): Handle error by either size or result in 
		// return and other as second parameter?
	}

	// Return file size
	return (uint64_t)fileStats.st_size;
#endif
}

// -------------------------------------------------------------------------- //

uint64_t alfFileGetCursorPosition(const AlfFile* file)
{
	return file->cursorPosition;
}
