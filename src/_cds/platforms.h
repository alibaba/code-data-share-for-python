#ifndef PYCDS_PLATFORMS_H
#define PYCDS_PLATFORMS_H

#ifdef __unix__
#define IS_POSIX 1
#define IS_WINDOWS 0
#elif defined(_WIN32) || defined(WIN32)
#define IS_POSIX 0
#define IS_WINDOWS 1
// include windows.h before other Windows-specific headers
#include <windows.h>
#endif

#if IS_POSIX
#include <sys/fcntl.h>
#include <sys/mman.h>
#elif IS_WINDOWS
#include <fileapi.h>
#include <stdio.h>
#include <tchar.h>
#endif

#if IS_POSIX
typedef int fd_type;
#elif IS_WINDOWS
typedef HANDLE fd_type;
#endif

fd_type
create_archive_preallocate(const char *name, size_t size);

void
truncate_fd(fd_type fd, size_t size);

void *
request_map_from_archive(void *addr, size_t size, fd_type fd);

void
finalize_map(fd_type *file, size_t size, void *addr);

#endif
