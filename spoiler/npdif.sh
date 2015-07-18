#!/bin/sh

function distance
{
	printf "$1" | sed -e 's/./&\n/g' >tmp$$.A
	printf "$2" | sed -e 's/./&\n/g' >tmp$$.B

	echo "<<< A=$1 B=$2"
	dist=$(./npdif -vvv tmp$$.A tmp$$.B)

	printf ">>> got=%d expect=%d " "$dist" "$3"
	rm tmp$$.A tmp$$.B

	if [ $dist -ne $3 ]; then
		echo FAIL
		return 1
	fi

	echo OK
	return 0
}

if ! make npdif ; then
	exit 1
fi

distance "ABD" "ABCD" 1
distance "ABCD" "ACDBECFD" 4
distance "ABCDEF" "ABXYEFCD" 6
distance "ABCDEFGHIJK" "ABCEFGIJKDEFGHIJK" 6
distance "abcabba" "cbabac" 5
distance "acebdabbabed" "acbdeacbed" 6
