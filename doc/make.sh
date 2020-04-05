#!/usr/bin/env bash

cd "${0%/*}"

# check for rst2man
type rst2man.py &> /dev/null || {
    echo 'error : install docutils first' >&2
    exit 1
}

printf -v rst *.rst

# trim down manpages
gzip -9 > "${rst%.rst}.1.gz" < <(
    while IFS= read -r line; do
        [[ $line =~ ^'.\"' ]] ||
            printf '%s\n' "$line"
    done < <(rst2man.py "$rst")
)
