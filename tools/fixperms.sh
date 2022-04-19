#!/usr/bin/env sh

HERE=$(dirname "$0")
for i in $HERE/*.py $HERE/*.sh $HERE/dos2unix $HERE/ninja;
do
	echo Setting executable bit on $i
	chmod a+x $i
done

DEMO="$HERE/../demo"
ENGINE="$HERE/../engine"
for i in $DEMO $ENGINE;
do
	for j in $(find $i -type f);
	do
		echo Unsetting executable bit on $j
		chmod a-x $j
	done
done

echo Done

