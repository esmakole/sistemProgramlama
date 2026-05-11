/**
 * @file extract.h
 * @brief Arsiv cikarma modulu baslik dosyasi
 */

#ifndef EXTRACT_H
#define EXTRACT_H

/**
 * @brief .sau arsivinden dosyalari cikarir
 *
 * @param archive_name  Arsiv dosyasi adi
 * @param output_dir    Hedef dizin (NULL ise mevcut dizin)
 * @return TARSAU_SUCCESS veya hata kodu
 */
int extract_all(const char *archive_name, const char *output_dir);

#endif /* EXTRACT_H */
