#include <err.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void
usage(char *name)
{
    fprintf (
        stderr,
        "usage : %s [option] <parameters>\n\n"
        "options : \n"
        "    -r <amount> <string w/ hex>    modify red value of hex in <string> by <amount>\n"
        "    -g <amount> <string w/ hex>    modify green value of hex in <string> by <amount>\n"
        "    -b <amount> <string w/ hex>    modify blue value of hex in <string> by <amount>\n"
        "    -a <amount> <string w/ hex>    modify all values of hex in <string> by <amount>\n",
        basename(name)
    );

    exit(EXIT_FAILURE);
}

static long
get_num(char *str)
{
    errno = 0;
    char *ptr;

    long tmp = strtol(str, &ptr, 10);

    if (errno != 0 || *ptr != 0)
        errx(EXIT_FAILURE, "'%s' isn't a valid amount", str);

    return tmp;
}

static unsigned short
get_rgb(char *str, unsigned rgb[3])
{
    errno = 0;
    char *ptr;

    for (size_t i = 0; i < 6; i += 2) {
        char tmp[3] = {str[i], str[i + 1], 0};

        rgb[i / 2] = strtoul(tmp, &ptr, 16);

        if (errno != 0 || *ptr != 0)
            return 0;
    }

    return 1;
}

static void
make_valid(unsigned *value, long offset)
{
    long tmp = (long)*value + offset;

    if      (tmp <   0) tmp =   0;
    else if (tmp > 255) tmp = 255;

    *value = tmp;
}

static void
color(char *str, long num[3])
{
    unsigned rgb[3];

    size_t len = strnlen(str, LINE_MAX);

    for (size_t i = 0; i < len; ++i) {
        putchar(str[i]);

        if (str[i] == '#' && i + 6 < len && get_rgb(str + i + 1, rgb) == 1) {
            for (unsigned short j = 0; j < 3; ++j)
                make_valid(&rgb[j], num[j]);

            printf("%02X%02X%02X", rgb[0], rgb[1], rgb[2]);

            i += 6;
        }
    }

    putchar('\n');
}

int
main(int argc, char **argv)
{
    long tmp;
    long num[3] = {0};

    for (int arg; (arg = getopt(argc, argv, ":r:g:b:a:")) != -1;)
        switch (arg) {
            case 'r' : num[0] += get_num(optarg); break;
            case 'b' : num[1] += get_num(optarg); break;
            case 'g' : num[2] += get_num(optarg); break;
            case 'a' :
                tmp = get_num(optarg);

                for (unsigned short i = 0; i < 3; ++i)
                    num[i] += tmp;

                break;
            default : usage(argv[0]);
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
