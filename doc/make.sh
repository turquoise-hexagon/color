#!/usr/bin/env bash

cd "${0%/*}"

# check for rst2man
_() { command -v "$1"; }

{ r2m=$(_ rst2man.py) || r2m=$(_ rst2man); } || {
    echo 'error : install docutils first' >&2
    exit 1
}

printf -v rst *.rst

# trim down manpages
gzip -9 > "${rst%.rst}.1.gz" < <(
    while IFS= read -r line; do
        [[ $line =~ ^'.\"' ]] ||
            printf '%s\n' "$line"
    done < <("$r2m" "$rst")
)
