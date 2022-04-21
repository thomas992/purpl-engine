#!/usr/bin/env sh

# Does the same thing as ddos.bat (kills DOS line endings)
for i in $(find . -type f | grep -Ev "\.git|build|deps"); do
	if [ "$(file -k $i | grep text)" != "" ]; then
		$(dirname $0)/dos2unix $i
	else
		echo Skipping $i
	fi
done
