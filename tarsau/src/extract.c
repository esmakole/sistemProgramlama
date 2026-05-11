/**
 * @file extract.c
 * @brief Arsiv cikarma modulu
 *
 * .sau arsiv dosyasini acar, metadata'yi parse eder,
 * dosyalari cikarir ve izinleri uygular.
 */

#include "tarsau.h"

#define ERR_CORRUPT "Arsiv dosyasi uygunsuz veya bozuk!\n"

/**
 * Metadata stringini parse ederek entry dizisine doldurur.
 * Format: |dosya_adi,izin,boyut||dosya_adi2,izin2,boyut2|...
 * Donen: parse edilen dosya sayisi, hata durumunda -1.
 */
static int parse_metadata(const char *meta, size_t meta_len,
                          tarsau_file_entry_t *entries, int max_entries)
{
    int count = 0;
    size_t pos = 0;

    while (pos < meta_len && count < max_entries) {
        const char *rec_start;
        const char *rec_end;
        char record[TARSAU_BUFFER_SIZE];
        size_t rec_len;
        char *comma1;
        char *comma2;

        /* '|' karakterini bekle */
        if (meta[pos] != '|')
            return -1;
        pos++;

        if (pos >= meta_len)
            break;

        /* || ayiricisi -> bos kayit, atla */
        if (meta[pos] == '|')
            continue;

        rec_start = &meta[pos];

        /* Kayit sonunu bul */
        rec_end = (const char *)memchr(rec_start, '|',
                                       meta_len - pos);
        if (rec_end == NULL)
            return -1;

        rec_len = (size_t)(rec_end - rec_start);
        if (rec_len == 0 || rec_len >= sizeof(record))
            return -1;

        memcpy(record, rec_start, rec_len);
        record[rec_len] = '\0';

        /* "dosya_adi,izin,boyut" parse */
        comma1 = strchr(record, ',');
        if (comma1 == NULL)
            return -1;
        *comma1 = '\0';
        comma1++;

        comma2 = strchr(comma1, ',');
        if (comma2 == NULL)
            return -1;
        *comma2 = '\0';
        comma2++;

        /* Dosya adi uzunluk kontrolu */
        {
            size_t name_len = strlen(record);
            if (name_len == 0 || name_len >= TARSAU_MAX_FILENAME)
                return -1;
            memcpy(entries[count].filename, record, name_len + 1);
        }
        entries[count].permissions = perm_octal_to_mode(comma1);
        entries[count].size = (off_t)strtol(comma2, NULL, 10);

        if (entries[count].size < 0)
            return -1;

        count++;
        pos = (size_t)(rec_end - meta) + 1;
    }

    return count;
}

int extract_all(const char *archive_name, const char *output_dir)
{
    int archive_fd;
    char size_buf[TARSAU_META_SIZE_LEN + 1];
    char *meta_buf = NULL;
    unsigned long meta_size;
    char *endptr;
    tarsau_file_entry_t entries[TARSAU_MAX_FILES];
    int file_count;
    int i;
    ssize_t rd;

    /* --- 1. Arsiv dosyasini ac --- */
    if (!file_exists(archive_name)) {
        fprintf(stderr, ERR_CORRUPT);
        return TARSAU_ERR_INVALID_FMT;
    }

    archive_fd = fileio_open_read(archive_name);
    if (archive_fd == -1) {
        fprintf(stderr, ERR_CORRUPT);
        return TARSAU_ERR_IO;
    }

    /* --- 2. 10 baytlik boyut basligini oku --- */
    rd = fileio_read_full(archive_fd, size_buf,
                          TARSAU_META_SIZE_LEN);
    if (rd != TARSAU_META_SIZE_LEN) {
        fprintf(stderr, ERR_CORRUPT);
        fileio_close(archive_fd);
        return TARSAU_ERR_INVALID_FMT;
    }
    size_buf[TARSAU_META_SIZE_LEN] = '\0';

    /* Baslik yalnizca rakam olmali */
    meta_size = strtoul(size_buf, &endptr, 10);
    if (*endptr != '\0' || meta_size == 0) {
        fprintf(stderr, ERR_CORRUPT);
        fileio_close(archive_fd);
        return TARSAU_ERR_INVALID_FMT;
    }

    /* --- 3. Metadata'yi oku --- */
    meta_buf = malloc(meta_size + 1);
    if (meta_buf == NULL) {
        fprintf(stderr, ERR_CORRUPT);
        fileio_close(archive_fd);
        return TARSAU_ERR_MEMORY;
    }

    rd = fileio_read_full(archive_fd, meta_buf, meta_size);
    if (rd < 0 || (unsigned long)rd != meta_size) {
        fprintf(stderr, ERR_CORRUPT);
        free(meta_buf);
        fileio_close(archive_fd);
        return TARSAU_ERR_INVALID_FMT;
    }
    meta_buf[meta_size] = '\0';

    /* --- 4. Metadata'yi parse et --- */
    file_count = parse_metadata(meta_buf, meta_size,
                                entries, TARSAU_MAX_FILES);
    free(meta_buf);

    if (file_count <= 0) {
        fprintf(stderr, ERR_CORRUPT);
        fileio_close(archive_fd);
        return TARSAU_ERR_INVALID_FMT;
    }

    /* --- 5. Cikti dizinini olustur (gerekirse) --- */
    if (output_dir != NULL) {
        if (fileio_mkdir(output_dir, TARSAU_DEFAULT_DIR_PERMS)
                != TARSAU_SUCCESS) {
            fileio_close(archive_fd);
            return TARSAU_ERR_IO;
        }
    }

    /* --- 6. Dosyalari cikar --- */
    for (i = 0; i < file_count; i++) {
        char out_path[TARSAU_MAX_PATH];
        char content_buf[TARSAU_BUFFER_SIZE];
        int out_fd;
        off_t remaining;

        if (output_dir != NULL) {
            snprintf(out_path, sizeof(out_path), "%s/%s",
                     output_dir, entries[i].filename);
        } else {
            snprintf(out_path, sizeof(out_path), "%s",
                     entries[i].filename);
        }

        out_fd = fileio_open_write(out_path,
                                   entries[i].permissions);
        if (out_fd == -1) {
            fileio_close(archive_fd);
            return TARSAU_ERR_IO;
        }

        remaining = entries[i].size;
        while (remaining > 0) {
            size_t to_read;
            to_read = (remaining > (off_t)sizeof(content_buf))
                          ? sizeof(content_buf)
                          : (size_t)remaining;

            rd = fileio_read_full(archive_fd, content_buf,
                                  to_read);
            if (rd <= 0) {
                fprintf(stderr, ERR_CORRUPT);
                fileio_close(out_fd);
                fileio_close(archive_fd);
                return TARSAU_ERR_INVALID_FMT;
            }

            if (fileio_write_full(out_fd, content_buf,
                                  (size_t)rd) != rd) {
                tarsau_error("Dosya yazma hatasi: %s",
                             out_path);
                fileio_close(out_fd);
                fileio_close(archive_fd);
                return TARSAU_ERR_IO;
            }

            remaining -= (off_t)rd;
        }

        fileio_close(out_fd);

        /* Izinleri uygula */
        perm_set_file_mode(out_path, entries[i].permissions);
    }

    fileio_close(archive_fd);
    tarsau_info("Arsiv acildi: %d dosya cikarildi.", file_count);
    return TARSAU_SUCCESS;
}
