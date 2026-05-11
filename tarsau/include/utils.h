/**
 * @file utils.h
 * @brief Yardimci fonksiyonlar modulu
 */

#ifndef UTILS_H
#define UTILS_H

void tarsau_error(const char *fmt, ...);
void tarsau_info(const char *fmt, ...);
int  file_exists(const char *path);
int  is_ascii_file(const char *path);
int  has_extension(const char *filename, const char *ext);
const char *get_basename(const char *path);

#endif /* UTILS_H */
