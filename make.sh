#!/usr/bin/env bash

readonly DIR=$PWD/eggs
readonly INSTALL=$(csi -R 'chicken.platform' -p '(installation-repository)')

set -e

mapfile -t eggs < eggs.txt

mkdir -p "$DIR"
cp "$INSTALL"/* "$DIR"

export CHICKEN_INSTALL_REPOSITORY=$DIR
export CHICKEN_REPOSITORY_PATH=$DIR

chicken-install "${eggs[@]}" > /dev/null

$CSC $CSCFLAGS "$1" -o "$2"
