#!/usr/bin/env sh

HERE=$(dirname "$0")

for i in $HERE/*.py $HERE/*.sh $HERE/dos2unix $HERE/glslc $HERE/ninja;
do
	echo Setting executable bit on $i
	chmod a+x $i
done

for i in demo cmake engine;
do
	for j in $(find "$HERE/../$i" -type f);
	do
		echo Unsetting executable bit on $j
		chmod a-x $j
	done
done

echo Done

