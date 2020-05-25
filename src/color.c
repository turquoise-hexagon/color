#include <err.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void
print_usage(char *program_name)
{
    fprintf(
        stderr,
        "usage : %s [option] <parameters>\n\n"
        "options :\n"
        "    -r <amount> <string>    change red value of all hex colors in <string> by <amount>\n"
        "    -g <amount> <string>    change green value of all hex colors in <string> by <amount>\n"
        "    -b <amount> <string>    change blue value of all hex colors in <string> by <amount>\n"
        "    -a <amount> <string>    change overall value of all hex colors in <string> by <amount>\n",
        basename(program_name)
    );

    exit(EXIT_FAILURE);
}

static long
convert_to_number(const char *str)
{
    errno = 0;
    char *ptr;

    long number = strtol(str, &ptr, 10);

    if (errno != 0 || *ptr != 0)
        errx(EXIT_FAILURE, "'%s' isn't a valid amount", str);

    return number;
}

static bool
convert_to_rgb(const char *str, unsigned rgb[3])
{
    errno = 0;
    char *ptr;

    uint32_t tmp = strtoul(str, &ptr, 16);

    if (errno != 0 || *ptr != 0)
        return 0;

    /* obtain rgb values from tmp */
    rgb[0] = (tmp >> 16) & 0xFF;
    rgb[1] = (tmp >>  8) & 0xFF;
    rgb[2] = (tmp >>  0) & 0xFF;

    return 1;
}

static void
make_value_valid(unsigned *value, long offset)
{
    /*
     * out rgb values can only be between 0 and 255
     * fix values accordingly
     */
    long tmp = (long)*value + offset;

    if (tmp <   0) tmp =   0;
    if (tmp > 255) tmp = 255;

    *value = tmp;
}

static void
parse_string(const char *str, const long offsets[3])
{
    unsigned rgb[3] = {0};
    char hex[7] = {0}; /* will contain hex strings */

    size_t length = strnlen(str, LINE_MAX);

    for (size_t i = 0; i < length; ++i) {
        putchar(str[i]);

        /* check if we can still fit a hex string */
        if (str[i] == '#' && i + 6 < length) {
            /* copy over hex string */
            for (unsigned short j = 0; j < 6; ++j)
                hex[j] = str[i + j + 1];

            if (convert_to_rgb(hex, rgb) == 0)
                continue;

            make_value_valid(&rgb[0], offsets[0]);
            make_value_valid(&rgb[1], offsets[1]);
            make_value_valid(&rgb[2], offsets[2]);

            printf("%02X%02X%02X", rgb[0], rgb[1], rgb[2]);

            i += 6;
        }
    }

    putchar('\n');
}

int
main(int argc, char **argv)
{
    if (argc < 2)
        print_usage(argv[0]);

    long tmp;
    long offsets[3] = {0};

    /* argument parsing */
    for (int arg; (arg = getopt(argc, argv, ":r:g:b:a:")) != -1;)
        switch (arg) {
            case 'r': offsets[0] += convert_to_number(optarg); break;
            case 'g': offsets[1] += convert_to_number(optarg); break;
            case 'b': offsets[2] += convert_to_number(optarg); break;
            case 'a':
                tmp = convert_to_number(optarg);

                offsets[0] += tmp;
                offsets[1] += tmp;
                offsets[2] += tmp;

                break;
            default : print_usage(argv[0]);
        }

    if (optind < argc)
        for (unsigned i = optind; i < (unsigned)argc; ++i)
            parse_string(argv[i], offsets);
    else {
        char line[LINE_MAX] = {0};

        while (fgets(line, LINE_MAX, stdin) != NULL) {
            /* fix string */
            line[strnlen(line, LINE_MAX) - 1] = 0;

            parse_string(line, offsets);
        }
    }

    return EXIT_SUCCESS;
}
