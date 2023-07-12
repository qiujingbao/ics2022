#ifndef __FS_H__
#define __FS_H__

#include <common.h>

#ifndef SEEK_SET
enum
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};
#endif

int fs_open(const char * /*, int flags, mode_t mode*/);
size_t fs_lseek(int fd, size_t offset, int whence);
size_t fs_read(int, void *, size_t);

#endif
