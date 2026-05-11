/**
 * @file permissions.h
 * @brief Dosya izinleri yonetimi modulu
 */

#ifndef PERMISSIONS_H
#define PERMISSIONS_H

int  perm_get_file_mode(const char *path, mode_t *mode_out);
int  perm_set_file_mode(const char *path, mode_t mode);
void perm_mode_to_octal(mode_t mode, char *buf, size_t buflen);
mode_t perm_octal_to_mode(const char *str);

#endif /* PERMISSIONS_H */
