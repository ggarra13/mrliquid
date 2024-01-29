#!/bin/bash --norc


for file in *; do
    orig="$MAYA_LOCATION/scripts/others/$file"
    if [ -e $orig ]; then
	xxdiff -w $file $orig

	root=${file%.mel}
	src="$MAYA_LOCATION/scripts/others/$root.res.mel"
	if [ -e $src ]; then
	    dst=$root.res.mel
	    if [ ! -e "$dst" ]; then
		cp $src $dst
	    fi
	fi
    fi

done
