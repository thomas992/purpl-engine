#!/usr/bin/env bash

# Does the same thing as ddos.bat (kills DOS line endings)
$(dirname $0)/dos2unix $(find -type f | grep -Ev "\.git|build|deps")
