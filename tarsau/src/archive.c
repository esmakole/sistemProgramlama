/**
 * @file archive.c
 * @brief Arsiv olusturma modulu
 *
 * .sau arsiv formati:
 *   [10 bayt: metadata boyutu][metadata][dosya icerikleri]
 *
 * Metadata formati:
 *   |dosya_adi,izinler,boyut||dosya_adi2,izinler2,boyut2|...
 *
 * Dosya icerikleri ayirici olmadan arka arkaya yazilir.
 */

#include "tarsau.h"

/**
 * Tek bir dosyanin metadata kaydini olusturur: |dosya_adi,izin,boyut|
 * Donen deger: yazilan karakter sayisi, hata durumunda -1.
 */
static int build_entry_metadata(const char *basename_str,
                                mode_t perms, off_t fsize,
                                char *buf, size_t buflen)
{
    char perm_str[16];
    int written;

    perm_mode_to_octal(perms, perm_str, sizeof(perm_str));
    written = snprintf(buf, buflen, "|%s,%s,%ld|",
                       basename_str, perm_str, (long)fsize);

    if (written < 0 || (size_t)written >= buflen)
        return -1;
    return written;
}

/**
 * Tum dosyalarin metadata stringini olusturur.
 * meta_buf'a yazar, toplam uzunlugu meta_len'e yazar.
 */
static int build_all_metadata(const tarsau_file_entry_t *entries,
                              int count,
                              char *meta_buf, size_t meta_buflen,
                              size_t *meta_len)
{
    size_t offset = 0;
    int i;

    for (i = 0; i < count; i++) {
        int written = build_entry_metadata(
            entries[i].filename,
            entries[i].permissions,
            entries[i].size,
            meta_buf + offset,
            meta_buflen - offset
        );
        if (written < 0) {
            tarsau_error("Metadata olusturma hatasi");
            return TARSAU_ERR_IO;
        }
        offset += (size_t)written;
    }

    *meta_len = offset;
    return TARSAU_SUCCESS;
}

/**
 * Kaynak dosyanin icerigini arsiv fd'sine kopyalar.
 */
static int copy_file_content(int archive_fd, const char *src_path,
                             off_t fsize)
{
    char buf[TARSAU_BUFFER_SIZE];
    int src_fd;
    off_t remaining = fsize;
    ssize_t rd;

    src_fd = fileio_open_read(src_path);
    if (src_fd == -1)
        return TARSAU_ERR_IO;

    while (remaining > 0) {
        size_t to_read = (remaining > (off_t)sizeof(buf))
                             ? sizeof(buf)
                             : (size_t)remaining;

        rd = fileio_read_full(src_fd, buf, to_read);
        if (rd <= 0) {
            tarsau_error("Dosya okuma hatasi: %s", src_path);
            fileio_close(src_fd);
            return TARSAU_ERR_IO;
        }

        if (fileio_write_full(archive_fd, buf, (size_t)rd) != rd) {
            tarsau_error("Arsive yazma hatasi");
            fileio_close(src_fd);
            return TARSAU_ERR_IO;
        }

        remaining -= (off_t)rd;
    }

    fileio_close(src_fd);
    return TARSAU_SUCCESS;
}

int archive_create(const char *archive_name, const char **files,
                   int file_count)
{
    tarsau_file_entry_t entries[TARSAU_MAX_FILES];
    char meta_buf[TARSAU_BUFFER_SIZE * 4];
    char size_header[TARSAU_META_SIZE_LEN + 1];
    size_t meta_len = 0;
    unsigned long total_size = 0;
    int archive_fd;
    int valid_count = 0;
    int i;
    int ret;

    /* --- 1. Giris dosyalarini dogrula ve bilgilerini topla --- */
    for (i = 0; i < file_count; i++) {
        const char *path = files[i];
        const char *bname = get_basename(path);
        off_t fsize;
        mode_t fmode;

        /* Dosya var mi? */
        if (!file_exists(path)) {
            fprintf(stderr,
                    "%s giris dosyasinin formati uyumsuzdur!\n", bname);
            return TARSAU_ERROR;
        }

        /* ASCII metin dosyasi mi? */
        if (!is_ascii_file(path)) {
            fprintf(stderr,
                    "%s giris dosyasinin formati uyumsuzdur!\n", bname);
            return TARSAU_ERROR;
        }

        /* Boyut al */
        fsize = fileio_get_size(path);
        if (fsize < 0) {
            fprintf(stderr,
                    "%s giris dosyasinin formati uyumsuzdur!\n", bname);
            return TARSAU_ERROR;
        }

        /* Izinleri al */
        if (perm_get_file_mode(path, &fmode) != TARSAU_SUCCESS) {
            fprintf(stderr,
                    "%s giris dosyasinin formati uyumsuzdur!\n", bname);
            return TARSAU_ERROR;
        }

        /* Toplam boyut kontrolu */
        total_size += (unsigned long)fsize;
        if (total_size > TARSAU_MAX_TOTAL_SIZE) {
            fprintf(stderr,
                    "Hata: Toplam dosya boyutu 200 MB sinirini asiyor.\n");
            return TARSAU_ERR_TOO_LARGE;
        }

        /* Entry'ye kaydet */
        snprintf(entries[valid_count].filename,
                 TARSAU_MAX_FILENAME, "%s", bname);
        entries[valid_count].size = fsize;
        entries[valid_count].permissions = fmode;
        valid_count++;
    }

    if (valid_count == 0) {
        fprintf(stderr, "Hata: Arsivlenecek gecerli dosya yok.\n");
        return TARSAU_ERROR;
    }

    /* --- 2. Metadata stringini olustur --- */
    ret = build_all_metadata(entries, valid_count,
                             meta_buf, sizeof(meta_buf), &meta_len);
    if (ret != TARSAU_SUCCESS)
        return ret;

    /* --- 3. 10 baytlik boyut basligini olustur --- */
    snprintf(size_header, sizeof(size_header), "%010lu",
             (unsigned long)meta_len);

    /* --- 4. Arsiv dosyasini olustur ve yaz --- */
    archive_fd = fileio_open_write(archive_name, TARSAU_DEFAULT_PERMS);
    if (archive_fd == -1)
        return TARSAU_ERR_IO;

    /* 10 baytlik boyut basligi */
    if (fileio_write_full(archive_fd, size_header,
            TARSAU_META_SIZE_LEN) != TARSAU_META_SIZE_LEN) {
        tarsau_error("Boyut basligi yazma hatasi");
        fileio_close(archive_fd);
        return TARSAU_ERR_IO;
    }

    /* Metadata */
    if (fileio_write_full(archive_fd, meta_buf,
            meta_len) != (ssize_t)meta_len) {
        tarsau_error("Metadata yazma hatasi");
        fileio_close(archive_fd);
        return TARSAU_ERR_IO;
    }

    /* --- 5. Dosya iceriklerini yaz --- */
    for (i = 0; i < valid_count; i++) {
        ret = copy_file_content(archive_fd, files[i],
                                entries[i].size);
        if (ret != TARSAU_SUCCESS) {
            fileio_close(archive_fd);
            return ret;
        }
    }

    fileio_close(archive_fd);
    tarsau_info("Arsiv olusturuldu: %s (%d dosya)",
                archive_name, valid_count);
    return TARSAU_SUCCESS;
}
