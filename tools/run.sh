#!/usr/bin/env bash

here = $(dirname $0)
LD_LIBRARY_PATH=$here/bin:$here
if [ $(uname -k) == "Darwin" ]; then
	DYLD_LIBRARY_PATH=$LD_LIBRARY_PATH
fi

$here/demo
