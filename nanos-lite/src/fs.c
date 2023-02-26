#include <fs.h>
#include <klib.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENTS, FD_DISPINFO, FD_FB};

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_EVENTS] = {"/dev/events", 0, 0, events_read, invalid_write},
  [FD_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
  [FD_FB] = {"/dev/fb",  0, 0, invalid_read, fb_write},
#include "files.h"
};

void init_fs() {
    file_table[FD_FB].size = io_read(AM_GPU_CONFIG).width * 
                             io_read(AM_GPU_CONFIG).height * 
                             sizeof(uint32_t);
}

int fs_open(const char *pathname, int flags, int mode) {
    
    for (int32_t i = 0; i < sizeof(file_table) / sizeof(file_table[0]); ++i) {
        if (strcmp(file_table[i].name, pathname) == 0) {
            file_table[i].open_offset = file_table[i].disk_offset;
            return i;
        }
    }

    return -1;
}

size_t fs_read(int fd, void *buf, size_t len) {
    if (fd < 0 || fd >= sizeof(file_table) / sizeof(file_table[0])) {
        return -1;
    }

    if (file_table[fd].read != invalid_read && file_table[fd].read != NULL) {
        return file_table[fd].read(buf, file_table[fd].open_offset, len);
    }

    size_t targer_offset = file_table[fd].open_offset + len;
    size_t max_offset = file_table[fd].disk_offset + file_table[fd].size;
    size_t read_len = len;

    if(targer_offset > max_offset) {
        read_len = max_offset - file_table[fd].open_offset;
    }
    
    ramdisk_read(buf, file_table[fd].open_offset, read_len);

    file_table[fd].open_offset += read_len;

    return read_len;
}

size_t fs_write(int fd, void *buf, size_t len) {
    if (fd < 0 || fd >= sizeof(file_table) / sizeof(file_table[0])) {
        return -1;
    }

    if (file_table[fd].write != invalid_write && file_table[fd].write != NULL) {
        return file_table[fd].write(buf, file_table[fd].open_offset, len);
    }

    size_t targer_offset = file_table[fd].open_offset + len;
    size_t max_offset = file_table[fd].disk_offset + file_table[fd].size;
    size_t write_len = len;

    if(targer_offset > max_offset) {
        write_len = max_offset - file_table[fd].open_offset;
    }

    ramdisk_write(buf, file_table[fd].open_offset, write_len);
    
    file_table[fd].open_offset += write_len;

    return write_len;
}


size_t fs_lseek(int fd, size_t offset, int whence) {
    if (fd < 0 || fd >= sizeof(file_table) / sizeof(file_table[0])) {
        return -1;
    }
    
    size_t target_offset = 0;

    switch (whence)
    {
    case SEEK_SET:
        target_offset = file_table[fd].disk_offset + offset;
        break;
    case SEEK_CUR:
        target_offset = file_table[fd].open_offset + offset;
        break;
    case SEEK_END:
        target_offset = file_table[fd].disk_offset + file_table[fd].size;
        break;
    default:
        assert(0);
        break;
    }

    if (target_offset < file_table[fd].disk_offset) {
        target_offset = file_table[fd].disk_offset;
    } else if (target_offset > file_table[fd].disk_offset + file_table[fd].size) {
        target_offset = file_table[fd].disk_offset + file_table[fd].size;
    }

    file_table[fd].open_offset = target_offset;

    // Upon successful completion, lseek() returns the resulting offset location as measured in bytes from the beginning of the file.
    return target_offset - file_table[fd].disk_offset;
}

int fs_close(int fd) {
    return 0;
}