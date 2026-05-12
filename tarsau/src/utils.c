/**
 * @file utils.c
 * @brief Yardimci fonksiyonlar - hata yonetimi, dosya dogrulama
 */

#include "tarsau.h"
#include <stdarg.h>

void tarsau_error(const char *fmt, ...)
{
    va_list ap;
    fprintf(stderr, "[HATA] ");
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    if (errno != 0) {
        fprintf(stderr, ": %s", strerror(errno));
        errno = 0;
    }
    fprintf(stderr, "\n");
}

void tarsau_info(const char *fmt, ...)
{
    va_list ap;
    fprintf(stdout, "[BILGI] ");
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
    fprintf(stdout, "\n");
}

int file_exists(const char *path)
{
    return (access(path, F_OK) == 0) ? 1 : 0;
}

/**
 * Dosyanin ASCII metin dosyasi olup olmadigini kontrol eder.
 * Her bayti kontrol eder: NULL (0x00) veya >127 varsa metin degil.
 */
int is_ascii_file(const char *path)
{
    int fd;
    unsigned char buf[TARSAU_BUFFER_SIZE];
    ssize_t bytes_read;
    ssize_t idx;

    fd = fileio_open_read(path);
    if (fd == -1)
        return 0;

    while ((bytes_read = read(fd, buf, sizeof(buf))) > 0) {
        for (idx = 0; idx < bytes_read; idx++) {
            if (buf[idx] == 0 || (buf[idx] & 0x80) != 0) {
                fileio_close(fd);
                return 0;
            }
        }
    }

    fileio_close(fd);
    return (bytes_read == -1) ? 0 : 1;
}

int has_extension(const char *filename, const char *ext)
{
    size_t name_len, ext_len;
    if (filename == NULL || ext == NULL)
        return 0;
    name_len = strlen(filename);
    ext_len = strlen(ext);
    if (ext_len > name_len)
        return 0;
    return (strcmp(filename + name_len - ext_len, ext) == 0) ? 1 : 0;
}

const char *get_basename(const char *path)
{
    const char *base;
    if (path == NULL)
        return NULL;
    base = strrchr(path, '/');
    return (base != NULL) ? base + 1 : path;
}
