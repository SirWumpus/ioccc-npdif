README.TXT
==========

To Build
--------

        $ cc -o prog prog.c


Usage Examples
--------------

	$ ./prog file1 file2

	$ ./prog -d file1 file2

	$ sh ./prog-test.sh | more


Features
--------

* "An O(NP) Sequence Comparison Algorithm" by Wu, Manber, Myers, Miller.

* Output compatible with patch(1).

* Strokes one of the judge's ego (I couldn't email chocolate or curry).


Description
-----------

This is a functioning micro diff tool using an [O(NP) algorithm][Wu+89],
compared to the older [O(ND) difference algorithm][Mye86] used by some
versions of diff.  Its output is based on the default diff(1) output
described by POSIX and [The Open Group Base Specifications][SUSV7].  The
output is suitable for use with patch(1).

The -d option simply writes the edit distance between file1 and file2.  


References
----------

[Wu+89] Wu, Manber, Myers, and Miller; August 1989; 
	"An O(NP) Sequence Comparison Algorithm"; 
	http://myerslab.mpi-cbg.de/wp-content/uploads/2014/06/np_diff.pdf

[FNV94]	Fowler, Noll, Vo; 1994
	http://www.isthe.com/chongo/tech/comp/fnv/index.html
	https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

[SUSV7]	"diff", The Open Group Base Specifications Issue 7
	IEEE Std 1003.1, 2013 Edition
	http://pubs.opengroup.org/onlinepubs/9699919799/utilities/diff.html

[Mye86] Eugene W. Myers, "An O(ND) Difference Algorithm and Its
        Variations", Algorithmica, 1986, pages. 251-266

[Mil85]	"A File Comparison Program”, Webb Miller and Eugene W. Myers
	Software-Practice And Experience, Vol. 15(11). 1025-1040 (November 1985)
	http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.189.70&rep=rep1&type=pdf

[Hir75] D.S. Hirschberg, "A linear space algorithm for
        computing maximal common subsequence problem",
        Comm. of ACM, Vol. 18, June 1975, pages 341-343

