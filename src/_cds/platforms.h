#ifndef PYCDS_PLATFORMS_H
#define PYCDS_PLATFORMS_H

#if defined(__unix__) || defined(__APPLE__)
#define IS_POSIX 1
#define IS_WINDOWS 0
#elif defined(_WIN32) || defined(WIN32)
#define IS_POSIX 0
#define IS_WINDOWS 1
#include <windows.h>
#endif

#include <stddef.h>
#include <stdio.h>

#if IS_POSIX
#include <printf.h>
#include <stdarg.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#elif IS_WINDOWS
#include <fileapi.h>
#include <stdbool.h>
#include <tchar.h>
#endif

#if IS_POSIX
typedef int fd_type;
#define P_VALUE void

#define NULL_FD 0

#ifdef MAP_POPULATE
#define M_POPULATE MAP_POPULATE
#else
#define M_POPULATE 0
#endif
#elif IS_WINDOWS
typedef HANDLE fd_type;
#define P_VALUE DWORD

#define NULL_FD NULL
#endif

typedef P_VALUE *p_type;

struct CDSStatus;

void
verbose(const char *fmt, ...);

fd_type
create_archive_preallocate(const char *name, size_t size);

void
truncate_fd(fd_type fd, size_t size);

void *
create_map_from_archive(void *addr, size_t size, struct CDSStatus cds_status);

struct CDSArchiveHeader;

struct CDSArchiveHeader *
read_header_from_archive(const char *archive, fd_type *fd,
                         struct CDSArchiveHeader *header, size_t header_size);

void
close_archive(fd_type *fd);

void *
map_archive(struct CDSStatus cds_status, size_t size, void *addr);

void
finalize_map(struct CDSStatus cds_status, size_t size, void *addr);
#endif
