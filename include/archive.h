/**
 * @file archive.h
 * @brief Arsiv olusturma modulu baslik dosyasi
 */

#ifndef ARCHIVE_H
#define ARCHIVE_H

/**
 * @brief Dosyalari .sau arsivine birlestirir
 *
 * @param archive_name  Olusturulacak arsiv dosyasi adi
 * @param files         Arsivlenecek dosya adlari dizisi
 * @param file_count    Dosya sayisi
 * @return TARSAU_SUCCESS veya hata kodu
 */
int archive_create(const char *archive_name, const char **files, int file_count);

#endif /* ARCHIVE_H */
