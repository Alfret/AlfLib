# AlfLib
AlfLib is a collection of small C libraries. Each library is just a header and 
an accompanying implementatione file. It's as simple as dropping both files into
a project to use it.

All libraries are licensed under the MIT license.

## Libraries
There are a small set of libraries that all are related to a single purpose.

| Library    | Version | Description                                |
|------------|---------|--------------------------------------------|
| collection | 1.0.0   | Hash-map, Array-list and other collections |
| unicode    | 1.0.0   | Unicode utilites, UTF-8 and UTF-16         |
| thread     | 1.0.0   | Thread creation and management             |
| test       | 0.1.0   | Testing framework                          |
| file (WIP) | -.-.-   | File and filesystem management             |

### Collection
The collection library contains implementations for two types of lists. 

First is an implementation of an array-list that can hold pointers to other objects.

Second is the list that stores objects directly in the list. To be able to do this the list has to be told at creation how large an object is. This list has very good cache behaviour.

### Unicode
The unicode library contains utilities for UTF-8 and UTF-16 management. It has functions to encode and decode codepoints into and from the supported encodings. Then there are also functions for manipulating strings encoded in the supported encodings.

Finally there are functions for converting between different encodings.

### Thread
The thread library contains functions for starting threads and manipulating running threads. As well there are also primitives for synchronizing threads, such as mutees, semaphores and read-write locks. There are also support for condition variables.

Atomic functions are also part of the library. These can be used to perform atomic operations on data.

**Note**: This library must be initialized before it can be used. And it can also optionally be deinitialized on shutdown.

### Test
A simple testing framework with support for a range of different checks. Each test is part of a suite. Multiple suites may be run together.

A summary of the tests are shown in the standard output.

### File (WIP)
File management and filesystem utilities. This is work in progress.

## How to use
To use one of the libraries simply copy the 2 files into the project and compile them with the rest of the code



