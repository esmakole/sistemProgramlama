/**
 * @file parser.h
 * @brief Komut satiri arguman ayristirma modulu
 */

#ifndef PARSER_H
#define PARSER_H

#define MODE_CREATE     1
#define MODE_EXTRACT    2
#define MODE_HELP       3

typedef struct {
    int         mode;
    char        archive_name[TARSAU_MAX_PATH];
    const char  **input_files;
    int         input_count;
    char        output_dir[TARSAU_MAX_PATH];
} tarsau_args_t;

int  parser_parse_args(int argc, char *argv[], tarsau_args_t *args);
void parser_print_usage(void);

#endif /* PARSER_H */
