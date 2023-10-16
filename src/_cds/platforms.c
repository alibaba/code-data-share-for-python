#include "platforms.h"

#ifdef MAP_POPULATE
#define M_POPULATE MAP_POPULATE
#else
#define M_POPULATE 0
#endif

void
verbose(const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    fprintf(stderr, "[cds] ");
    vfprintf(stderr, fmt, arg);
    fprintf(stderr, "\n");
    va_end(arg);
    fflush(stderr);
}

fd_type
create_archive_preallocate(const char *name, size_t size)
{
    fd_type fd;
#if IS_POSIX
    fd = open(name, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd > 0) {
        truncate_fd(fd, size);
    }
#elif IS_WINDOWS
    fd = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                    FILE_FLAG_NO_BUFFERING, NULL);
    if (fd != INVALID_HANDLE_VALUE) {
        truncate_fd(fd, size);
    }
    else {
        fd = 0;
    }
#endif
    return fd;
}

void
truncate_fd(fd_type fd, size_t size)
{
#if IS_POSIX
    ftruncate(fd, size);
#elif IS_WINDOWS
    SetFilePointer(fd, size, NULL, FILE_BEGIN);
    SetEndOfFile(fd);
#endif
}

void *
create_map_from_archive(void *addr, size_t size, fd_type fd)
{
    void *res;
#if IS_POSIX
    res = mmap(addr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (res == MAP_FAILED || res != addr) {
        goto fail;
    }
#elif IS_WINDOWS
    HANDLE mapping =
        CreateFileMapping(fd, NULL, PAGE_READWRITE, 0, size, NULL);
    if (mapping == NULL) {
        verbose("err: %p", GetLastError());
        goto fail;
    }
    res = MapViewOfFileEx(mapping, FILE_MAP_WRITE, 0, 0, 0, addr);
    if (res == NULL || res != addr) {
        verbose("err: %p", GetLastError());
        goto fail;
    }
#endif
    return res;
fail:
    return NULL;
}

struct CDSArchiveHeader *
read_header_from_archive(const char *archive, fd_type *fd,
                         struct CDSArchiveHeader *header, size_t header_size)
{
#if IS_POSIX
    *fd = open(archive, O_RDWR);
    if (*fd <= 0) {
        *fd = NULL_FD;
        goto fail;
    }
    if (read(*fd, header, header_size) != header_size) {
        goto fail;
    }
    return header;
#elif IS_WINDOWS
    *fd = CreateFile(archive, GENERIC_READ, 0, NULL, OPEN_EXISTING,
                     FILE_ATTRIBUTE_NORMAL, NULL);
    if (*fd == INVALID_HANDLE_VALUE) {
        *fd = NULL_FD;
        goto fail;
    }
    DWORD size;
    if (!ReadFile(*fd, header, header_size, &size, NULL) ||
        size != header_size) {
        CloseHandle(fd);
        goto fail;
    }
    return header;
#endif
fail:
    return NULL;
}

void *
map_archive(fd_type file, size_t size, void *addr)
{
#if IS_POSIX
    void *shm = mmap(addr, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_FIXED | M_POPULATE, file, 0);
    if (shm == MAP_FAILED) {
        goto fail;
    }
    return shm;
#elif IS_WINDOWS
    HANDLE mapping =
        CreateFileMapping(file, NULL, PAGE_READONLY, 0, size, NULL);
    if (mapping == NULL) {
        verbose("err1: %p", GetLastError());
        goto fail;
    }
    ptype *shm = MapViewOfFileEx(mapping, FILE_MAP_COPY, 0, 0, 0, addr);
    if (shm == NULL) {
        verbose("err2: %p, %p, %p", GetLastError(), mapping, addr);
        CloseHandle(mapping);
        goto fail;
    }
    return shm;
#endif
fail:
    return NULL;
}

void
close_archive(fd_type *file)
{
#if IS_POSIX
    close(*file);
    *file = 0;
#elif IS_WINDOWS
    CloseHandle(*file);
    *file = 0;
#endif
}

void
finalize_map(fd_type *file, size_t size, void *addr)
{
#if IS_POSIX
    ftruncate(*file, size);
#elif IS_WINDOWS
    UnmapViewOfFile(addr);
    truncate_fd(*file, size);
#endif
    close_archive(file);
}
