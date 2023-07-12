#include <fs.h>

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);
#define FD_SIZE sizeof(file_table) / sizeof(file_table[0])
typedef struct
{
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum
{
  FD_STDIN,
  FD_STDOUT,
  FD_STDERR,
  FD_FB
};

size_t invalid_read(void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}
extern size_t serial_write(const void *, size_t, size_t);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    [FD_STDIN] = {"stdin", 0, 0, invalid_read, invalid_write},
    [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
    [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
#include "files.h"

};

void init_fs()
{
  // TODO: initialize the size of /dev/fb
}
extern size_t ramdisk_read(void *, size_t, size_t);
extern size_t ramdisk_write(const void *, size_t, size_t);
// 忽略flags和mode
int fs_open(const char *pathname /*, int flags, mode_t mode*/)
{
  for (int i = FD_FB + 1; i < FD_SIZE; i++)
  {
    if (strcmp(pathname, file_table[i].name) == 0)
    {
      file_table[i].read = *ramdisk_read;
      file_table[i].write = *ramdisk_write;
      return i;
    }
  }
  return -1;
}
size_t fs_lseek(int fd, size_t offset, int whence)
{
  switch (whence)
  {
  case SEEK_SET:
    file_table[fd].open_offset = offset;
    break;
  case SEEK_CUR:
    file_table[fd].open_offset += offset;
    break;
  case SEEK_END:
    file_table[fd].open_offset = file_table[fd].size + offset;
    break;
  }
  return file_table[fd].open_offset;
}
size_t fs_read(int fd, void *buf, size_t count)
{
  // 处理count
  if (fd > FD_FB && (file_table[fd].open_offset + count >= file_table[fd].size))
  {
    count = file_table[fd].size - file_table[fd].open_offset;
    if (count < 0)
      count = 0;
  }
  count = file_table[fd].read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, count);
  file_table[fd].open_offset += count;
  return count;
}
/* write的操作系统实现 如果是输出到串口则跳转到对应的函数 */
size_t fs_write(int fd, const void *buf, size_t count)
{
  if (fd > FD_FB && (file_table[fd].open_offset + count >= file_table[fd].size))
  {
    count = file_table[fd].size - file_table[fd].open_offset;
    if (count < 0)
      count = 0;
  }
  count = file_table[fd].write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, count);
  file_table[fd].open_offset += count;
  return count;
}
int fs_close(int fd)
{
  file_table[fd].open_offset = 0;
  return 0;
}
