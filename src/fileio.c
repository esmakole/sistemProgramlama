/**
 * Dosyayi sadece okuma modunda acar.
 * Basarili olursa dosya tanitici (fd) dondurur.
 * Hata durumunda -1 dondurur.
 */
#include "tarsau.h"
int fileio_open_read(const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd == -1)
        tarsau_error("Dosya acilamadi (okuma): %s", path);
    return fd;
}

/**
 * Dosyayi yazma modunda acar.
 * Dosya yoksa olusturur, varsa icerigini sifirlar.
 * Verilen mode parametresi ile izinleri ayarlar.
 */
int fileio_open_write(const char *path, mode_t mode)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (fd == -1)
        tarsau_error("Dosya acilamadi (yazma): %s", path);
    return fd;
}

/**
 * Acik olan dosyayi kapatir.
 * close() sistem cagrisi kullanilir.
 */
int fileio_close(int fd)
{
    if (close(fd) == -1) {
        tarsau_error("Dosya kapatilamadi (fd=%d)", fd);
        return TARSAU_ERR_IO;
    }
    return TARSAU_SUCCESS;
}

/**
 * Dosyadan belirtilen boyut kadar veri okur.
 * Eksik okuma durumlarinda tum veri okunana kadar devam eder.
 * EINTR hatasinda okuma islemini tekrar dener.
 */
ssize_t fileio_read_full(int fd, void *buf, size_t size)
{
    size_t total = 0;
    ssize_t n;

    while (total < size) {
        n = read(fd, (char *)buf + total, size - total);

        if (n == -1) {
            if (errno == EINTR)
                continue;

            tarsau_error("read() basarisiz");
            return -1;
        }

        if (n == 0)
            break;

        total += (size_t)n;
    }

    return (ssize_t)total;
}

/**
 * Tampon bellekteki veriyi dosyaya yazar.
 * Tum veri yazilana kadar write() islemini tekrarlar.
 * EINTR hatasinda yazma islemini yeniden dener.
 */
ssize_t fileio_write_full(int fd, const void *buf, size_t size)
{
    size_t total = 0;
    ssize_t n;

    while (total < size) {
        n = write(fd, (const char *)buf + total, size - total);

        if (n == -1) {
            if (errno == EINTR)
                continue;

            tarsau_error("write() basarisiz");
            return -1;
        }

        total += (size_t)n;
    }

    return (ssize_t)total;
}

/**
 * Verilen dosyanin boyutunu byte cinsinden alir.
 * stat() sistem cagrisi kullanilir.
 */
off_t fileio_get_size(const char *path)
{
    struct stat st;

    if (stat(path, &st) == -1) {
        tarsau_error("stat() basarisiz: %s", path);
        return -1;
    }

    return st.st_size;
}

/**
 * Yeni bir dizin olusturur.
 * Dizin zaten varsa hata vermez.
 */
int fileio_mkdir(const char *path, mode_t mode)
{
    if (mkdir(path, mode) == -1) {

        if (errno == EEXIST) {
            errno = 0;
            return TARSAU_SUCCESS;
        }

        tarsau_error("mkdir() basarisiz: %s", path);
        return TARSAU_ERR_IO;
    }

    return TARSAU_SUCCESS;
}