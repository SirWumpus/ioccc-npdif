#!/bin/ksh

flags="$@"

function distance
{
	typeset A="$1"; shift
	typeset B="$1"; shift
	typeset expect="$1"; shift

	printf "$A" | sed -e 's/./&\
/g' >tmp$$.A
	printf "$B" | sed -e 's/./&\
/g' >tmp$$.B

	echo "<<< A='$A' B='$B' $@"
	typeset dist=$(./npdif -d $@ tmp$$.A tmp$$.B | tr -d '\r')

	printf '>>> got=%d expect=%d ' "$dist" "$expect"
	rm tmp$$.A tmp$$.B

	if [ "$dist" -ne "$expect" ]; then
		echo FAIL
		return 1
	fi

	echo -OK-
	return 0
}

if ! make npdif ; then
	exit 1
fi

distance "1" "A" 2 $flags
distance "123" "ABCDE" 8 $flags
distance "ABCDE" "123" 8 $flags
distance "ABD" "ABCD" 1 $flags
distance "ABCD" "ABD" 1 $flags
distance "ABCD" "ACDBECFD" 4 $flags
distance "ABCDEF" "ABXYEFCD" 6 $flags
distance "ABCDEFGHIJK" "ABCEFGIJKDEFGHIJK" 6 $flags
distance "ABCABBA" "CBABAC" 5 $flags
distance "ACEBDABBABED" "ACBDEACBED" 6 $flags
