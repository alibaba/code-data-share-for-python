#include "platforms.h"

#include "_cdscontext.h"

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

#if IS_WINDOWS
inline void
debug_windows_err(LPCVOID lpAddress, DWORD size)
{
    if (GetLastError() != ERROR_INVALID_ADDRESS) {
        verbose("unhandled error");
    }

    LPCVOID lpEndAddress = (LPCVOID)((DWORD_PTR)lpAddress + size);

    MEMORY_BASIC_INFORMATION mbi;
    bool canBeMapped = true;

    while ((DWORD_PTR)lpAddress < (DWORD_PTR)lpEndAddress) {
        if (VirtualQuery(lpAddress, &mbi, sizeof(mbi)) == 0) {
            verbose("VirtualQuery failed with error: %p", GetLastError());
            return;
        }

        if (mbi.State != MEM_FREE) {
            canBeMapped = false;
            verbose(
                "Address range starting from %p of size %ld bytes is not "
                "free. State: %d",
                lpAddress, mbi.RegionSize, mbi.State);
        }

        lpAddress = (LPCVOID)((DWORD_PTR)mbi.BaseAddress + mbi.RegionSize);
    }
}
#endif

void *
create_map_from_archive(void *addr, size_t size, struct CDSStatus cds_status)
{
    void *res;
#if IS_POSIX
    res = mmap(addr, size, PROT_READ | PROT_WRITE, MAP_SHARED,
               cds_status.archive_fd, 0);
    if (res == MAP_FAILED || res != addr) {
        goto fail;
    }
#elif IS_WINDOWS
    cds_status.mapping = CreateFileMapping(cds_status.archive_fd, NULL,
                                           PAGE_READWRITE, 0, size, NULL);
    if (cds_status.mapping == NULL) {
        verbose("mapping from new archive failed: %p", GetLastError());
        goto fail;
    }
    res = MapViewOfFileEx(cds_status.mapping, FILE_MAP_WRITE, 0, 0, 0, addr);
    if (res == NULL || res != addr) {
        debug_windows_err(addr, size);
        CloseHandle(cds_status.mapping);
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
map_archive(struct CDSStatus cds_status, size_t size, void *addr)
{
#if IS_POSIX
    void *shm =
        mmap(addr, size, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_FIXED | M_POPULATE, cds_status.archive_fd, 0);
    if (shm == MAP_FAILED) {
        goto fail;
    }
    return shm;
#elif IS_WINDOWS
    cds_status.mapping = CreateFileMapping(cds_status.archive_fd, NULL,
                                           PAGE_READONLY, 0, size, NULL);
    if (cds_status.mapping == NULL) {
        verbose("load mapping from archive failed: %p", GetLastError());
        goto fail;
    }
    p_type *shm =
        MapViewOfFileEx(cds_status.mapping, FILE_MAP_COPY, 0, 0, 0, addr);
    if (shm == NULL) {
        debug_windows_err(addr, size);
        CloseHandle(cds_status.mapping);
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
    if (file > 0) {
#if IS_POSIX
        close(*file);
        *file = 0;
#elif IS_WINDOWS
        CloseHandle(*file);
        *file = 0;
#endif
    }
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
