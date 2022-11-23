#include "platforms.h"

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
    fd = CreateFileA(name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
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
    HANDLE mapping = CreateFileMappingA(fd, NULL, PAGE_READWRITE | SEC_COMMIT,
                                        0, size, NULL);
    if (mapping == NULL) {
        goto fail;
    }
    res = MapViewOfFileEx(mapping, FILE_MAP_WRITE, 0, 0, 0, addr);
    if (res == NULL) {
        goto fail;
    }
#endif
    return res;
fail:
    return NULL;
}

struct CDSArchiveHeader *
open_archive(const char *archive, fd_type *fd, struct CDSArchiveHeader *header)
{
#if IS_POSIX
    *fd = open(archive, O_RDWR);
    if (*fd <= 0) {
        *fd = 0;
        goto fail;
    }
    if (read(*fd, header, sizeof(*h)) != sizeof(*h)) {
        goto fail;
    }
    return header;
#elif IS_WINDOWS
#endif
fail:
    return NULL;
}

void *
map_archive(fd_type file, size_t size, void *addr)
{
#if IS_POSIX
    mmap(addr, aize, PROT_READ | PROT_WRITE,
         MAP_PRIVATE | MAP_FIXED | M_POPULATE, file, 0);
    if (shm == MAP_FAILED) {
        goto fail;
    }
    return shm;
#elif IS_WINDOWS
#endif
fail:
    return NULL;
}

void
finalize_map(fd_type *file, size_t size, void *addr)
{
#if IS_POSIX
    ftruncate(*file, size);
    close(*file);
    *file = 0;
#elif IS_WINDOWS
    UnmapViewOfFile(addr);
    truncate_fd(*file, size);
    CloseHandle(*file);
    *file = 0;
#endif
}
