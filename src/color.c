#include <err.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

static void
usage(char *name)
{
    fprintf(
        stderr,
        "usage : %s [option] <parameters>\n"
        "options :\n\n"
        "    -r <amount> <color>    modify red value of <hex> by <amount>\n"
        "    -g <amount> <color>    modify green value of <hex> by <amount>\n"
        "    -b <amount> <color>    modify blue value of <hex> by <amount>\n"
        "    -a <amount> <color>    modify all values of <hex> by <amount>\n",
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
        errx(EXIT_FAILURE, "'%s' isn't a valid numeric value", str);

    return tmp;
}

static unsigned *
get_rgb(char *str)
{
    if (strnlen(str, LINE_MAX) != 7 || str[0] != '#')
        errx(EXIT_FAILURE, "'%s' isn't a valid hex value", str);

    unsigned *res = malloc(3 * sizeof *res);

    if (res == NULL)
        errx(EXIT_FAILURE, "failed to allocate memory");

    errno = 0;
    char *ptr;

    for (unsigned short i = 1; i < 7; i += 2) {
        char tmp[2] = {str[i], str[i + 1]};

        res[(i - 1) / 2] = strtol(tmp, &ptr, 16);

        if (errno != 0 || *ptr != 0)
            errx(EXIT_FAILURE, "'%s' isn't a valid hex value", str);
    }

    return res;
}

static char *
get_hex(unsigned *array)
{
    char *res = malloc(8 * sizeof *res);

    if (res == NULL)
        errx(EXIT_FAILURE, "failed to allocate memory");

    if (snprintf(res, 8, "#%02X%02X%02X", array[0], array[1], array[2]) < 0)
        errx(EXIT_FAILURE, "failed to convert value");

    return res;
}

static unsigned
make_valid(unsigned value, long offset)
{
    long tmp = (long)value + offset;

    if (tmp <   0) return   0;
    if (tmp > 255) return 255;

    return tmp;
}

static void
print_color(unsigned *array)
{
    char *tmp = get_hex(array);

    if (isatty(fileno(stdout)) == 1)
        printf(
            "\033[48;2;%u;%u;%um    \033[m %s\n",
            array[0],
            array[1],
            array[2],
            tmp
        );
    else
        printf("%s\n", tmp);

    free(tmp);
}

int
main(int argc, char **argv)
{
    if (argc < 2)
        usage(argv[0]);

    long r_num = 0;
    long g_num = 0;
    long b_num = 0;

    int arg;

    while ((arg = getopt(argc, argv, ":r:g:b:a:")) != -1)
        switch (arg) {
            case 'r' : r_num += get_num(optarg); break;
            case 'g' : g_num += get_num(optarg); break;
            case 'b' : b_num += get_num(optarg); break;
            case 'a' :;
                const long tmp = get_num(optarg);

                r_num += tmp;
                g_num += tmp;
                b_num += tmp;

                break;
            default  : usage(argv[0]);
        }

    unsigned *rgb;

    if (optind < argc)
        rgb = get_rgb(argv[optind]);
    else {
        char input[LINE_MAX];

        if (fgets(input, LINE_MAX, stdin) == NULL)
            errx(EXIT_FAILURE, "failed to read from stdin");

        input[strnlen(input, LINE_MAX) - 1] = 0;

        rgb = get_rgb(input);
    }

    rgb[0] = make_valid(rgb[0], r_num);
    rgb[1] = make_valid(rgb[1], g_num);
    rgb[2] = make_valid(rgb[2], b_num);

    print_color(rgb);

    free(rgb);

    return EXIT_SUCCESS;
}
