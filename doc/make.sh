#!/usr/bin/env bash

cd "${0%/*}"

# check for rst2man
_() { command -v "$1"; }

{ r2m=$(_ rst2man.py) || r2m=$(_ rst2man); } || {
    echo 'error : install docutils first' >&2
    exit 1
}

printf -v rst *.rst

{
    gzip -9 > "${rst%.rst}.1.gz" < <(
        # trim down manpages
        while IFS= read -r line; do
            [[ $line =~ ^'.\"' ]] ||
                printf '%s\n' "$line"
        done < <(
            set -x
            "$r2m" "$rst"
            set +x
        )
    )
} |&
    while IFS= read -r line; do
        if [[ $line =~ ^\++\ ([^'set'].*) ]]; then
            printf '%s\n' "${BASH_REMATCH[1]}"
        fi
    done
