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

#ifndef ALF_FILE_H
#define ALF_FILE_H

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

#ifndef ALF_TRUE
  /** Boolean value true **/
# define ALF_TRUE ((AlfBool)1)
#endif

// -------------------------------------------------------------------------- //

#ifndef ALF_FALSE
  /** Boolean value false **/
# define ALF_FALSE ((AlfBool)0)
#endif

// -------------------------------------------------------------------------- //

/** Macro for the maximum path length on the library **/
#define ALF_FILE_MAX_PATH_LENGTH 4096

// -------------------------------------------------------------------------- //

#if defined(_WIN32)
  /** Path separator character **/
# define ALF_FILE_PATH_SEPARATOR '\\'
  /** Path separator string **/
# define ALF_FILE_PATH_SEPARATOR_STR "\\"
#else
  /** Path separator character **/
# define ALF_FILE_PATH_SEPARATOR '/'
  /** Path separator string **/
# define ALF_FILE_PATH_SEPARATOR_STR "/"
#endif

// ========================================================================== //
// Enumerations
// ========================================================================== //

/** \enum AlfFileResult
 * \author Filip Björklund
 * \date 13 januari 2019 - 12:12
 * \brief File results.
 * \details
 * Enumeration of file operation results.
 */
typedef enum AlfFileResult
{
  /** Success **/
  kAlfFileSuccess,
  /** Unknown error **/
  kAlfFileUnknownError,
  /** Out of memory **/
  kAlfFileOutOfMemory,
  /** End of file reached **/
  kAlfFileEof,
  /** Invalid arguments **/
  kAlfFileInvalidArguments,
  /** File was not found **/
  kAlfFileNotFound,
  /** File already exists **/
  kAlfFileAlreadyExists,
  /** Access to file was denied **/
  kAlfFileAccessDenied,
} AlfFileResult;

// -------------------------------------------------------------------------- //

/** \enum AlfFileFlag
 * \author Filip Björklund
 * \date 13 januari 2019 - 12:20
 * \brief File open flags.
 * \details
 * Enumerations of flags that can be specified when opening a file.
 */
typedef enum AlfFileFlag
{
  /** Open file for reading **/
  kAlfFileFlagRead = (1 << 0),
  /** Open file for writing **/
  kAlfFileFlagWrite = (1 << 1),
  /** Open file for reading and writing **/
  kAlfFileFlagReadWrite = kAlfFileFlagRead | kAlfFileFlagWrite,
  /** Open file in shared read mode **/
  kAlfFileFlagShareRead = (1 << 2),
  /** Open file in shared write mode **/
  kAlfFileFlagShareWrite = (1 << 3),
  /** Open file in shared read and write mode **/
  kAlfFileFlagShareReadWrite = kAlfFileFlagShareRead | kAlfFileFlagShareWrite,
  /** Create file if it does not exist **/
  kAlfFileFlagCreate = (1 << 4),
  /** Overwrite file. If kAlfFileFlagCreate was not specified then this will
   * truncate the file **/
  kAlfFileFlagOverwrite = (1 << 5),
  /** Open file with the cursor placed at the end. This means that contents
   * can be appended to an already existing file **/
  kAlfFileFlagAppend = (1 << 6)
} AlfFileFlag;

// ========================================================================== //
// Structures
// ========================================================================== //

/** \struct AlfFile
 * \author Filip Björklund
 * \date 13 januari 2019 - 12:14
 * \brief File handle.
 * \details
 * Structure that represents a handle to an open file.
 */
typedef struct tag_AlfFile AlfFile;

// ========================================================================== //
// Path Functions
// ========================================================================== //

/** Join two paths together into one. It's guaranteed to be separated by a 
 * single separator even if any of the two paths contains a preceding or 
 * trailing delimiter. 
 * \brief Join paths.
 * \param outputPath Buffer where the result is written. Must be at least 
 * ALF_FILE_MAX_PATH_LENGTH bytes in size.
 * \param path0 First path.
 * \param path1 Second path.
 * \return True if the join is successful, otherwise false.
 */
AlfBool alfPathJoin(char* outputPath, const char* path0, const char* path1);

// ========================================================================== //
// Filesystem Functions
// ========================================================================== //

/** Returns the current working directory of the program written to the 
 * specified buffer.
 * \note The buffer must be at least of length ALF_FILE_MAX_PATH_LENGTH.
 * \brief Returns the working directory.
 * \param buffer Buffer that the path is written to.
 */
void alfFilesystemGetWorkingDirectory(char* buffer);

// -------------------------------------------------------------------------- //

/** Set the current working directory of the program.
 * \brief Set working directory.
 * \param path Working directory to set.
 */
void alfFilesystemSetWorkingDirectory(const char* path);

// -------------------------------------------------------------------------- //

/** Returns whether or not an object exists in the filesystem at the specified
 * path.
 * \brief Returns whether object exists in filesystem.
 * \param path Path in filesystem.
 * \return True if the object exists, otherwise false.
 */
AlfBool alfFilesystemPathExists(const char* path);

// -------------------------------------------------------------------------- //

/** Create a file in the filesystem at the specified path. If the override flag
 * is set then any existing file at the same path is overridden.
 * \brief Create file.
 * \param path Path to file that is to be created.
 * \param override Whether to override any existing file.
 * \return Result.
 * - kAlfFileSuccess: Successfully created file.
 */
AlfFileResult alfFilesystemCreateFile(const char* path, AlfBool override);

// -------------------------------------------------------------------------- //

/** Create a directory in the filesystem at the specified path. If the recursive
 * flag is set then the function will create any intermediate directories as 
 * needed to create the final directory.
 * \brief Create directory.
 * \param path Path to directory.
 * \param recursive Whether to create intermediate directories.
 * \return Result.
 * - kAlfFileSuccess: Successfully created directory.
 * - kAlfFileNotFound: Intermediate directory or destination directory could 
 *   not be created. This may be caused by the recursive flag not being set, 
 *   thus causing intermediate directories to not be created.
 */
AlfFileResult alfFilesystemCreateDirectory(const char* path, AlfBool recursive);

// -------------------------------------------------------------------------- //

/** Delete a file in the filesystem at the specified path.
 * \brief Delete file.
 * \param path Path to file to delete.
 * \return Result.
 * - kAlfFileSuccess: Successfully deleted file.
 */
AlfFileResult alfFilesystemDeleteFile(const char* path);

// -------------------------------------------------------------------------- //

/** Returns whether or not the object at the specified path in the filesystem is
 * a file. If the object is not a file or does not exist then the function 
 * returns false.
 * \brief Returns whether path points to a file.
 * \param path Path.
 * \return True if the object at the path is a file, otherwise false.
 */
AlfBool alfFilesystemIsFilePath(const char* path);

// -------------------------------------------------------------------------- //

/** Returns whether or not the object at the specified path in the filesystem is
 * a directory. If the object is not a directory or does not exist then the 
 * function returns false.
 * \brief Returns whether path points to a directory.
 * \param path Path.
 * \return True if the object at the path is a directory, otherwise false.
 */
AlfBool alfFilesystemIsDirectoryPath(const char* path);

// -------------------------------------------------------------------------- //

/** Enumerate all objects in a directory.
 * \brief Enumerate directory.
 * \param[out] filesOut Created list of files.
 * \param[out] fileCountOut Number of files.
 * \param directoryPath Path to the directory to enumerate.
 * \return Result.
 * - kAlfFileSuccess: Successfully enumerated files.
 */
AlfFileResult alfFilesystemEnumerateDirectory(const char*** filesOut,
                                              uint32_t* fileCountOut,
                                              const char* directoryPath);

// -------------------------------------------------------------------------- //

/** Free a list of file paths.
 * \brief Free path list.
 * \param[in] files List to free.
 */
void alfFilesystemFreeFileList(const char** files);

// ========================================================================== //
// File Functions
// ========================================================================== //

/** Open a file at the specified path. The flags determine how the file is
 * opened.
 * \note If the create flag is set and the file does not exist it will be
 * created.
 * \note If the overwrite flag is set and the file does exist it will be
 * overwritten before being opened.
 * \pre The path must not be null.
 * \brief Open file.
 * \param path Path to open file at.
 * \param flags Flag to determine how to open file.
 * \param fileOut Handle to opened file.
 * \return Result.
 * - kAlfFileSuccess: Successfully opened file.
 * - LN_OUT_OF_MEMORY: Failed allocation.
 * - LN_INVALID_ARGUMENTS: Invalid flags.
 * - LN_ACCESS_DENIED: Access denied.
 * - LN_FILE_NOT_FOUND: File could not be found.
 * - LN_UNKNOWN_ERROR: Unknown error. Call icGetLastError to retrieve a
 * description of the error.
 */
AlfFileResult alfFileOpen(const char* path,
                          AlfFileFlag flags,
                          AlfFile** fileOut);

// -------------------------------------------------------------------------- //

/** Close a file that has previously been opened with a call to bcOpenFile.
 * \brief Close file.
 * \param file File to close.
 */
void alfFileClose(AlfFile* file);

// -------------------------------------------------------------------------- //

/** Flush all buffers for a specified file. This will write all data that is
 * still in the buffers.
 * \brief Flush file buffers.
 * \param file File to flush.
 */
void alfFileFlush(AlfFile* file);

// -------------------------------------------------------------------------- //

/** Seek to the specified position in a file. The position is relative to the
 * start of the file. To seek to the end use bcSeekFileEnd instead.
 * \brief Seek in file.
 * \param file File to seek in.
 * \param position Position to seek to.
 */
void alfFileSeek(AlfFile* file, uint64_t position);

// -------------------------------------------------------------------------- //

/** Seek to the end of a file.
 * \brief Seek to end of file.
 * \param file File to seek in.
 */
void alfFileSeekEnd(AlfFile* file);

// -------------------------------------------------------------------------- //

/** Read from a file into a buffer. The number of bytes to read is specified by
 * the bytesToRead parameter. The bytesRead parameter will, if not null, be set
 * to the number of bytes that were actually read.
 * \brief Read file.
 * \param file File to read.
 * \param buffer Buffer to read into.
 * \param bytesToRead Number of bytes to read.
 * \param bytesRead Set to number of read bytes. This can be null in which case
 * it will not be set.
 * \return Result.
 */
AlfFileResult alfFileRead(const AlfFile* file,
                          uint8_t* buffer,
                          uint64_t bytesToRead,
                          uint64_t* bytesRead);

// -------------------------------------------------------------------------- //

/** Write the specified number of bytes from a buffer into a file. The number of
 * bytes that was written is set in 'writtenBytes' if it's not null.
 * \brief Write to file.
 * \param file File to write to.
 * \param buffer Buffer to write content from.
 * \param bytesToWrite Number of bytes to write from buffer.
 * \param[out] writtenBytes Number of bytes that was written.
 * \return Result.
 */
AlfFileResult alfFileWrite(const AlfFile* file,
                           uint8_t* buffer,
                           uint64_t bytesToWrite,
                           uint64_t* writtenBytes);

// -------------------------------------------------------------------------- //

/** Returns the size of a file in number of bytes.
 * \brief Returns file size.
 * \param file File to get size of.
 * \return Size of file in bytes.
 */
uint64_t alfFileGetSize(const AlfFile* file);

// -------------------------------------------------------------------------- //

/** Returns the current cursor position in the file.
 * \brief Returns cursor position in file.
 * \param file File to get cursor position for.
 * \return Cursor position.
 */
uint64_t alfFileGetCursorPosition(const AlfFile* file);

// ========================================================================== //
// End of Header
// ========================================================================== //

#if defined(__cplusplus)
}
#endif

#endif // ALF_FILE_H