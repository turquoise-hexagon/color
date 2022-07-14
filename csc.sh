#!/bin/sh

readonly EGGS_DIR=$PWD/eggs

set -e

install -D "$(chicken-install -repository)"/* -t "$EGGS_DIR"

export                                   \
    CHICKEN_INSTALL_REPOSITORY=$EGGS_DIR \
    CHICKEN_REPOSITORY_PATH=$EGGS_DIR    \
    CHICKEN_EGG_CACHE=$EGGS_DIR

while IFS= read -r name; do
    chicken-install "$name"
done < eggs.txt

$CSC $CSCFLAGS "$1" -o "$2"
