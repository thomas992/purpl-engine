#!/usr/bin/env sh

HERE=$(dirname "$0")
for i in $HERE/*.py $HERE/*.sh $HERE/dos2unix $HERE/ninja;
do
    echo Making $i executable
    chmod a+x $i
done

echo Done

