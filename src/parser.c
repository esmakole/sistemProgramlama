/**
 * @file parser.c
 * @brief Komut satiri arguman ayristirma
 *
 * Desteklenen kullanim:
 *   tarsau -b dosya1 dosya2 ... [-o arsiv.sau]
 *   tarsau -a arsiv.sau [dizin]
 *   tarsau -h
 */

#include "tarsau.h"

int parser_parse_args(int argc, char *argv[], tarsau_args_t *args)
{
    int i;

    if (argc < 2)
        return TARSAU_ERROR;

    memset(args, 0, sizeof(tarsau_args_t));

    /* -h : Yardim */
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        args->mode = MODE_HELP;
        return TARSAU_SUCCESS;
    }

    /* -b : Arsiv olusturma */
    if (strcmp(argv[1], "-b") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Hata: -b secenegi icin en az bir dosya gerekli.\n");
            return TARSAU_ERROR;
        }

        args->mode = MODE_CREATE;

        /* -o'ya kadar veya arguman sonuna kadar dosyalari topla */
        i = 2;
        args->input_files = (const char **)&argv[i];
        args->input_count = 0;

        while (i < argc && strcmp(argv[i], "-o") != 0) {
            args->input_count++;
            i++;
        }

        if (args->input_count == 0) {
            fprintf(stderr, "Hata: Giris dosyasi belirtilmedi.\n");
            return TARSAU_ERROR;
        }

        if (args->input_count > TARSAU_MAX_FILES) {
            fprintf(stderr, "Hata: En fazla %d dosya arsivlenebilir.\n",
                    TARSAU_MAX_FILES);
            return TARSAU_ERROR;
        }

        /* -o varsa cikti adini al, yoksa varsayilan */
        if (i < argc && strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Hata: -o secenegi icin dosya adi gerekli.\n");
                return TARSAU_ERROR;
            }
            snprintf(args->archive_name, TARSAU_MAX_PATH, "%s", argv[i + 1]);
        } else {
            snprintf(args->archive_name, TARSAU_MAX_PATH, "%s",
                     TARSAU_DEFAULT_OUTPUT);
        }

        /* .sau uzanti kontrolu */
        if (!has_extension(args->archive_name, TARSAU_EXTENSION)) {
            fprintf(stderr,
                    "Hata: Cikti dosyasi .sau uzantili olmalidir.\n");
            return TARSAU_ERROR;
        }

        return TARSAU_SUCCESS;
    }

    /* -a : Arsiv acma */
    if (strcmp(argv[1], "-a") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Hata: -a secenegi icin arsiv dosyasi gerekli.\n");
            return TARSAU_ERROR;
        }
        /* -a'dan sonra en fazla 2 parametre */
        if (argc > 4) {
            fprintf(stderr,
                    "Hata: -a seceneginden sonra en fazla 2 parametre.\n");
            return TARSAU_ERROR;
        }

        args->mode = MODE_EXTRACT;
        snprintf(args->archive_name, TARSAU_MAX_PATH, "%s", argv[2]);

        /* Opsiyonel dizin adi */
        if (argc == 4) {
            snprintf(args->output_dir, TARSAU_MAX_PATH, "%s", argv[3]);
        }
        /* output_dir[0] == '\0' ise mevcut dizine acilacak */

        return TARSAU_SUCCESS;
    }

    fprintf(stderr, "Hata: Bilinmeyen secenek: %s\n", argv[1]);
    return TARSAU_ERROR;
}

void parser_print_usage(void)
{
    fprintf(stdout,
        "\nKullanim:\n"
        "  tarsau -b dosya1 dosya2 ... [-o arsiv.sau]\n"
        "  tarsau -a arsiv.sau [dizin]\n"
        "  tarsau -h\n"
        "\nSecenekler:\n"
        "  -b   Arsiv olustur (birden fazla dosya belirtilebilir)\n"
        "  -o   Cikti arsiv dosyasi adi (varsayilan: a.sau)\n"
        "  -a   Arsivden dosyalari cikar\n"
        "  -h   Bu yardim mesajini goster\n"
        "\nOrnekler:\n"
        "  tarsau -b not1.txt not2.txt -o belgeler.sau\n"
        "  tarsau -a belgeler.sau cikti_dizini\n\n"
    );
}
