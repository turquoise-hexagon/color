#include <err.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void
usage(char *name)
{
    fprintf(
        stderr,
        "usage : %s [option] <parameters>\n\n"
        "options :\n"
        "    -r <amount> <string w/ hex>    change red value of <hex> by <amount>\n"
        "    -g <amount> <string w/ hex>    change green value of <hex> by <amount>\n"
        "    -b <amount> <string w/ hex>    change blue value of <hex> by <amount>\n"
        "    -a <amount> <string w/ hex>    change overall value of <hex> by <amount>\n",
        basename(name)
    );

    exit(EXIT_FAILURE);
}

static long
get_num(const char *str)
{
    errno = 0;
    char *ptr;

    long num = strtol(str, &ptr, 10);

    if (errno != 0 || *ptr != 0)
        errx(EXIT_FAILURE, "'%s' isn't a valid amount", str);

    return num;
}

static unsigned short
get_rgb(const char *str, unsigned rgb[3])
{
    errno = 0;
    char *ptr;

    uint32_t tmp = strtoul(str, &ptr, 16);
    
    if (errno != 0 || *ptr != 0)
        return 0;

    rgb[0] = (tmp >> 16) & 0xFF;
    rgb[1] = (tmp >>  8) & 0xFF;
    rgb[2] = (tmp >>  0) & 0xFF;

    return 1;
}

static void
make_valid(unsigned *value, long offset)
{
    long tmp = (long)*value + offset;

    if (tmp <   0) tmp =   0;
    if (tmp > 255) tmp = 255;

    *value = tmp;
}

static void
color(const char *str, const long num[3])
{
    unsigned rgb[3];
    char tmp[7] = {0};

    size_t len = strnlen(str, LINE_MAX);

    for (size_t i = 0; i < len; ++i) {
        putchar(str[i]);

        if (str[i] == '#' && i + 6 < len) {
            for (unsigned short j = 0; j < 6; ++j)
                tmp[j] = str[i + j + 1];

            if (get_rgb(tmp, rgb) != 1)
                continue;

            make_valid(&rgb[0], num[0]);
            make_valid(&rgb[1], num[1]);
            make_valid(&rgb[2], num[2]);

            printf("%02X%02X%02X", rgb[0], rgb[1], rgb[2]);

            i += 6;
        }
    }

    putchar('\n');
}

int
main(int argc, char **argv)
{
    if (argc == 1)
        usage(argv[0]);

    long tmp;
    long num[3] = {0};

    for (int arg; (arg = getopt(argc, argv, ":r:g:b:a:")) != -1;)
        switch (arg) {
            case 'r': num[0] += get_num(optarg); break;
            case 'g': num[1] += get_num(optarg); break;
            case 'b': num[2] += get_num(optarg); break;
            case 'a':
                tmp = get_num(optarg);

                num[0] += tmp;
                num[1] += tmp;
                num[2] += tmp;

                break;
            default: usage(argv[0]);
        }

    if (optind < argc)
        for (unsigned i = optind; i < (unsigned)argc; ++i)
            color(argv[i], num);
    else {
        char input[LINE_MAX];

        while (fgets(input, LINE_MAX, stdin) != NULL) {
            input[strnlen(input, LINE_MAX) - 1] = 0;

            color(input, num);
        }
    }

    return EXIT_SUCCESS;
}
