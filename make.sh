#!/usr/bin/env bash

CC=${CC:-gcc}
CFLAGS="-pedantic -Wall -Wextra -O2 $CFLAGS"

# check for compiler
type "$CC" &> /dev/null || {
    echo "error : install $CC first" >&2
    exit 1
}

printf -v src src/*.c

bin=${src##*/}
bin=bin/${bin%.c}

[[ -e $bin ]] && {
    {
        read -r time_src
        read -r time_bin
    } < \
        <(stat -c '%Y' -- "$src" "$bin")

    ((time_src > time_bin)) || exit 0
}

mkdir -p bin

{
    set -x
    $CC $CFLAGS "$src" -o "$bin"
    set +x
} |&
    while IFS= read -r line; do
        if [[ $line =~ ^\++\ ([^'set'].*) ]]; then
            printf '%s\n' "${BASH_REMATCH[1]}"
        fi
    done
