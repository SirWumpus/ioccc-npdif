#!/bin/sh
function distance
{
	typeset A="$1"; shift
	typeset B="$1"; shift
	typeset expect="$1"; shift

	printf "$A" | sed -e 's/./&\n/g' >tmp$$.A
	printf "$B" | sed -e 's/./&\n/g' >tmp$$.B

	echo "<<< A='$A' B='$B' $@"
	dist=$(./npdif $@ tmp$$.A tmp$$.B | tr -d '\r')

	printf '>>> got=%d expect=%d ' "$dist" "$expect"
	rm tmp$$.A tmp$$.B

	if [ "$dist" -ne "$expect" ]; then
		echo FAIL
		return 1
	fi

	echo OK
	return 0
}

if ! make npdif ; then
	exit 1
fi

distance "1" "A" 2
distance "123" "ABCDE" 8
distance "ABD" "ABCD" 1
distance "ABCD" "ACDBECFD" 4
distance "ABCDEF" "ABXYEFCD" 6
distance "ABCDEFGHIJK" "ABCEFGIJKDEFGHIJK" 6
distance "abcabba" "cbabac" 5
distance "acebdabbabed" "acbdeacbed" 6
