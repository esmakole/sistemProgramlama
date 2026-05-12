/**
 * @file fileio.h
 * @brief Dosya G/C islemleri modulu (POSIX system calls)
 */

#ifndef FILEIO_H
#define FILEIO_H

int     fileio_open_read(const char *path);
int     fileio_open_write(const char *path, mode_t mode);
int     fileio_close(int fd);
ssize_t fileio_read_full(int fd, void *buf, size_t size);
ssize_t fileio_write_full(int fd, const void *buf, size_t size);
off_t   fileio_get_size(const char *path);
int     fileio_mkdir(const char *path, mode_t mode);

#endif /* FILEIO_H */
