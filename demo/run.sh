#!/usr/bin/env sh

HERE=$(dirname "$0")
LD_LIBRARY_PATH="$HERE/bin:$LD_LIBRARY_PATH" DYLD_LIBRARY_PATH="$HERE/bin:$DYLD_LIBRARY_PATH" "$HERE/demo" "$*"

