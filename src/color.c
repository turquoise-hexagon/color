#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <unistd.h>

struct color {
    long r;
    long g;
    long b;
};

static noreturn void
usage(char *name)
{
    fprintf(
        stderr,
        "usage : %s [-rgba <amount>] <string>\n"
        "\n"
        "options :\n"
        "    -r <amount> <string>    change red value of all hex color codes in <string> by <amount>\n"
        "    -g <amount> <string>    change green value of all hex color codes in <string> by <amount>\n"
        "    -b <amount> <string>    change blue value of all hex color codes in <string> by <amount>\n"
        "    -a <amount> <string>    change overall value of all hex color codes in <string> by <amount>\n",
        basename(name));

    exit(1);
}

static void *
allocate(size_t size)
{
    void *ptr;

    if (! (ptr = malloc(size))) {
        fprintf(stderr, "error : failed to allocate memory\n");

        exit(1);
    }

    return ptr;
}

static long
convert_to_number(const char *str)
{
    errno = 0;
    long num;

    {
        char *ptr;

        num = strtol(str, &ptr, 10);

        if (errno != 0 || *ptr != 0) {
            fprintf(stderr, "error : '%s' isn't a valid amount\n", str);

            exit(1);
        }
    }

    return num;
}

static struct color *
init_color(long r, long g, long b)
{
    struct color *color;

    color = allocate(sizeof(*color));

    color->r = r;
    color->g = g;
    color->b = b;

    return color;
}

static void
print_color(struct color *color)
{
    printf("%02lX%02lX%02lX", color->r, color->g, color->b);
}

static void
cleanup_color(struct color *color)
{
    free(color);
}

static void
make_valid_value(long *value, const long *offset)
{
    *value += *offset;

    /* keep rgb values between 0 and 255 */
    if (*value <   0) *value =   0;
    if (*value > 255) *value = 255;
}

static void
make_valid_color(struct color *color, const struct color *offset)
{
    make_valid_value(&color->r, &offset->r);
    make_valid_value(&color->g, &offset->g);
    make_valid_value(&color->b, &offset->b);
}

static struct color *
convert_to_color(const char *str)
{
    errno = 0;

    {
        uint64_t num;
        char *ptr;

        num = strtoul(str, &ptr, 16);

        return errno != 0 || *ptr != 0
            ? NULL
            : init_color(
                (num >> 16) & 0xFF,
                (num >>  8) & 0xFF,
                (num >>  0) & 0xFF);
    }
}

static void
parse_string(const char *str, const struct color *offset)
{
    size_t len;

    len = strnlen(str, LINE_MAX);

    char hex[7] = {0};

    for (size_t i = 0; i < len; ++i) {
        putchar(str[i]);

        if (str[i] == '#' && i + 6 < len) {
            /* copy over the hex code */
            strncpy(hex, str + i + 1, 6);

            struct color *color;

            if (! (color = convert_to_color(hex)))
                continue;

            make_valid_color(color, offset);
            print_color(color);
            cleanup_color(color);

            i += 6;
        }
    }

    putchar('\n');
}

int
main(int argc, char **argv)
{
    if (argc < 2)
        usage(argv[0]);

    struct color *offset;

    offset = init_color(0, 0, 0);

    for (int arg; (arg = getopt(argc, argv, ":r:g:b:a:")) != -1;)
        switch (arg) {
            case 'r': offset->r += convert_to_number(optarg); break;
            case 'g': offset->g += convert_to_number(optarg); break;
            case 'b': offset->b += convert_to_number(optarg); break;
            case 'a':;
                long tmp;

                tmp = convert_to_number(optarg);

                offset->r += tmp;
                offset->g += tmp;
                offset->b += tmp;

                break;
            default :
                usage(argv[0]);
        }

    if (optind < argc)
        for (int i = optind; i < argc; ++i)
            parse_string(argv[i], offset);
    else {
        char input[LINE_MAX] = {0};

        while (fgets(input, LINE_MAX, stdin)) {
            /* fix string */
            input[strnlen(input, LINE_MAX) - 1] = 0;

            parse_string(input, offset);
        }
    }

    cleanup_color(offset);

    return 0;
}
