/**
 * @file permissions.c
 * @brief Dosya izinleri yonetimi - stat/chmod ve octal donusum
 */

#include "tarsau.h"

int perm_get_file_mode(const char *path, mode_t *mode_out)
{
    struct stat st;
    if (stat(path, &st) == -1) {
        tarsau_error("stat() basarisiz: %s", path);
        return TARSAU_ERR_NOT_FOUND;
    }
    *mode_out = st.st_mode & 0777;
    return TARSAU_SUCCESS;
}

int perm_set_file_mode(const char *path, mode_t mode)
{
    if (chmod(path, mode) == -1) {
        tarsau_error("chmod() basarisiz: %s", path);
        return TARSAU_ERR_PERMISSION;
    }
    return TARSAU_SUCCESS;
}

/* mode_t -> "644" gibi octal string */
void perm_mode_to_octal(mode_t mode, char *buf, size_t buflen)
{
    snprintf(buf, buflen, "%o", (unsigned int)(mode & 0777));
}

/* "644" gibi octal string -> mode_t */
mode_t perm_octal_to_mode(const char *str)
{
    return (mode_t)strtoul(str, NULL, 8);
}
