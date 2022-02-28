#!/usr/bin/env bash

export HERE=$(realpath $(dirname "$0"))

chmod a+x "$HERE/demo"

export LD_LIBRARY_PATH="$HERE/bin:$HERE"
if [ "$(uname -s)" = "Darwin" ]; then
	export DYLD_LIBRARY_PATH=$LD_LIBRARY_PATH
fi

pushd $HERE
echo "Running from $(pwd)"
"$HERE/demo"
popd

