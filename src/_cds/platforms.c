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
