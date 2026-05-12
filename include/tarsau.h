/**
 * @file tarsau.h
 * @brief Ana proje baslik dosyasi - Ortak tanimlamalar ve sabitler
 */

#ifndef TARSAU_H
#define TARSAU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

/* Arsiv sabitleri */
#define TARSAU_EXTENSION        ".sau"
#define TARSAU_DEFAULT_OUTPUT   "a.sau"
#define TARSAU_MAX_FILENAME     256
#define TARSAU_MAX_PATH         4096
#define TARSAU_MAX_FILES        32
#define TARSAU_BUFFER_SIZE      4096
#define TARSAU_DEFAULT_PERMS    0644
#define TARSAU_DEFAULT_DIR_PERMS 0755
#define TARSAU_META_SIZE_LEN    10
#define TARSAU_MAX_TOTAL_SIZE   (200UL * 1024UL * 1024UL)

/* Hata kodlari */
#define TARSAU_SUCCESS           0
#define TARSAU_ERROR            -1
#define TARSAU_ERR_NOT_FOUND    -2
#define TARSAU_ERR_PERMISSION   -3
#define TARSAU_ERR_INVALID_FMT  -4
#define TARSAU_ERR_MEMORY       -5
#define TARSAU_ERR_IO           -6
#define TARSAU_ERR_NOT_ASCII    -7
#define TARSAU_ERR_TOO_MANY     -8
#define TARSAU_ERR_TOO_LARGE    -9

/* Dosya giris bilgisi */
typedef struct {
    char  filename[TARSAU_MAX_FILENAME];
    off_t size;
    mode_t permissions;
} tarsau_file_entry_t;

#include "archive.h"
#include "extract.h"
#include "parser.h"
#include "utils.h"
#include "permissions.h"
#include "fileio.h"

#endif /* TARSAU_H */
