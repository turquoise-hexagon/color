#!/usr/bin/env bash

readonly DIR=$PWD/eggs
readonly INSTALL=$(chicken-install -repository)

set -e

mapfile -t eggs < eggs.txt

mkdir -p "$DIR"
cp "$INSTALL"/* "$DIR"

export CHICKEN_INSTALL_REPOSITORY=$DIR \
       CHICKEN_REPOSITORY_PATH=$DIR    \
       CHICKEN_EGG_CACHE=$DIR

chicken-install "${eggs[@]}"

$CSC $CSCFLAGS "$1" -o "$2"
