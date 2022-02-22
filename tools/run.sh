#!/usr/bin/env bash

export HERE=$(dirname $(realpath "$0"))

chmod a+x "$HERE/demo"

export LD_LIBRARY_PATH="$HERE/bin:$HERE"
if [ "$(uname -s)" = "Darwin" ]; then
	export DYLD_LIBRARY_PATH=$LD_LIBRARY_PATH
fi

"$HERE/demo"
