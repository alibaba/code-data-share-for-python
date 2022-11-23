#ifndef PYCDS_PLATFORMS_H
#define PYCDS_PLATFORMS_H

#if defined(__unix__) || defined(__APPLE__)
#define IS_POSIX 1
#define IS_WINDOWS 0
#elif defined(_WIN32) || defined(WIN32)
#define IS_POSIX 0
#define IS_WINDOWS 1
// include windows.h before other Windows-specific headers
#include <windows.h>
#endif

#include <stddef.h>

#if IS_POSIX

#include <sys/fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#elif IS_WINDOWS
#include <fileapi.h>
#include <stdio.h>
#include <tchar.h>
#endif

#if IS_POSIX
typedef int fd_type;
typedef void *ptype;
#elif IS_WINDOWS
typedef HANDLE fd_type;
typedef long long ptype;
#endif

fd_type
create_archive_preallocate(const char *name, size_t size);

void
truncate_fd(fd_type fd, size_t size);

void *
create_map_from_archive(void *addr, size_t size, fd_type fd);

struct CDSArchiveHeader;

struct CDSArchiveHeader *
open_archive(const char *archive, fd_type *fd, struct CDSArchiveHeader *header, size_t header_size);

void *
map_archive(fd_type file, size_t size, void *addr);

void
finalize_map(fd_type *file, size_t size, void *addr);

#endif
