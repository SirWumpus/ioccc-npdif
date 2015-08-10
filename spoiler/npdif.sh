#!/bin/ksh

flags="$@"

function distance
{
	typeset A="$1"; shift
	typeset B="$1"; shift
	typeset expect="$1"; shift

	printf "$A" | sed -e 's/./&\
/g' >$A.tmp
	printf "$B" | sed -e 's/./&\
/g' >$B.tmp

	echo "<<< A='$A' B='$B' $@"
	typeset dist=$(./npdif -d $@ $A.tmp $B.tmp | tr -d '\r')

	printf '>>> got=%d expect=%d ' "$dist" "$expect"

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

distance "1" "1" 0 $flags
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

echo "===="
./npdif 1.tmp 1.tmp
echo "===="
./npdif ABCD.tmp ACDBECFD.tmp
echo "===="
./npdif ACDBECFD.tmp ABCD.tmp
echo "===="
./npdif 123.tmp ABCDE.tmp
echo "===="
./npdif ABCDE.tmp 123.tmp

