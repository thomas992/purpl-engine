#!/usr/bin/env sh

HERE=$(dirname "$0")
for i in $HERE/*.py $HERE/*.sh $HERE/dos2unix $HERE/ninja;
do
    echo Setting executable bit on $i
    chmod a+x $i
done

ENGINE="$HERE/../engine"
for i in $(find $ENGINE -type f);
do
    echo Unsetting executable bit on $i
    chmod a-x $i
done

echo Done

