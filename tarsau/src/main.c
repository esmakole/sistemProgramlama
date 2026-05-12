/**
 * @file main.c
 * @brief tarsau - Ana program giris noktasi
 */

#include "tarsau.h"

int main(int argc, char *argv[])
{
    tarsau_args_t args;
    int ret;

    ret = parser_parse_args(argc, argv, &args);
    if (ret != TARSAU_SUCCESS) {
        parser_print_usage();
        return EXIT_FAILURE;
    }

    switch (args.mode) {
        case MODE_CREATE:
            ret = archive_create(args.archive_name,
                                 args.input_files,
                                 args.input_count);
            break;

        case MODE_EXTRACT:
            ret = extract_all(args.archive_name,
                              args.output_dir[0] != '\0'
                                  ? args.output_dir : NULL);
            break;

        case MODE_HELP:
            parser_print_usage();
            ret = TARSAU_SUCCESS;
            break;

        default:
            parser_print_usage();
            ret = TARSAU_ERROR;
            break;
    }

    return (ret == TARSAU_SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE;
}
