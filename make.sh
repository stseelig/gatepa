#!/bin/sh -

readonly ROOT="$(realpath "$(dirname "$0")")";
readonly SRC="$ROOT/src";

readonly CC='cc';

CFLAGS=;

CFLAGS="$CFLAGS -std=c11";
CFLAGS="$CFLAGS -Wall";
CFLAGS="$CFLAGS -Wextra";
CFLAGS="$CFLAGS -Wpedantic";

CFLAGS="$CFLAGS -DNDEBUG";
#CFLAGS="$CFLAGS -gdwarf";

CFLAGS="$CFLAGS -Os";
CFLAGS="$CFLAGS -march=native";
CFLAGS="$CFLAGS -mtune=native";

readonly CFLAGS;

"$CC" $CFLAGS -I"$SRC" "$SRC/build_gatepa.c" "$SRC/build_libs.c" -o gatepa;
