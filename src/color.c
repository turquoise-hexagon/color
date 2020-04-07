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

//static unsigned *
static void
get_rgb(char *str, unsigned *array)
{
    if (strnlen(str, LINE_MAX) != 7 || str[0] != '#')
        errx(EXIT_FAILURE, "'%s' isn't a valid hex value", str);

    errno = 0;
    char *ptr;

    for (unsigned short i = 1; i < 7; i += 2) {
        char tmp[2] = {str[i], str[i + 1]};

        //res[(i - 1) / 2] = strtol(tmp, &ptr, 16);
        array[(i - 1) / 2] = strtol(tmp, &ptr, 16);

        if (errno != 0 || *ptr != 0)
            errx(EXIT_FAILURE, "'%s' isn't a valid hex value", str);
    }
}

static void
make_valid(unsigned *value, long offset)
{
    long tmp = (long)*value + offset;

    if      (tmp   < 0) tmp =   0;
    else if (tmp > 255) tmp = 255;

    *value = tmp;
}

static void
print_color(unsigned *array)
{
    if (isatty(fileno(stdout)) == 1)
        printf(
            "\033[48;2;%u;%u;%um    \033[m ",
            array[0],
            array[1],
            array[2]
        );

    printf(
        "#%02X%02X%02X\n",
        array[0],
        array[1],
        array[2]
    );
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

    unsigned rgb[3];

    if (optind < argc)
        get_rgb(argv[optind], rgb);
    else {
        char input[LINE_MAX];

        if (fgets(input, LINE_MAX, stdin) == NULL)
            errx(EXIT_FAILURE, "failed to read from stdin");

        input[strnlen(input, LINE_MAX) - 1] = 0;

        get_rgb(input, rgb);
    }

    make_valid(&rgb[0], r_num);
    make_valid(&rgb[1], g_num);
    make_valid(&rgb[2], b_num);

    print_color(rgb);

    return EXIT_SUCCESS;
}
