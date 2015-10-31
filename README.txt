README
------

To Build
--------

		$ cc -o prog prog.c


Usage Examples
--------------

		$ ./prog file1 file2

		$ ./prog -d file1 file2

		$ ./prog-test.sh


Features
--------

* "An O(NP) Sequence Comparison Algorithm" by Wu, Manber, Myers, Miller.

* Output compatible with patch(1).

* Strokes one of the judge's ego (I couldn't email chocolate or curry).


Description
-----------

This is a functioning micro diff tool using an [O(NP) algorithm][Wu+89], compared to the older [O(ND) difference algorithm][Mye86] used by some versions of diff.  Its output is based on the default diff(1) output described by POSIX and [The Open Group Base Specifications][SUSV7].  The output is suitable for use with patch(1).

The -d option simply writes the edit distance between file1 and file2.


Observations
------------

The FNV1a hash is a little slow compared to the trival hash GNU Diff uses.  I downloaded a plain text copy of "War And Peace" from Project Gutenberg, used ``makeholes.c`` to make 1000 random changes, then profiled and timed the program verses GNU Diff.  The bottle neck appears to be in the file I/O and line hashing with an average +0.05s slower. 

There is no hash collision checking, partly because FNV1a appears to generate very [few collisions][HshCmp] and an assumption that localised collisions within a region of edits are highly unlikely.


Support Files
-------------

* ``prog-test.sh``  
Basic test program verifies known test edit distances and patch support.

* ``avgtime.sh``

* ``makeholes.c``  
Random edits (holes) made to a file in-place.


References
----------

Wu, Manber, Myers, and Miller; August 1989;  
"An O(NP) Sequence Comparison Algorithm";  
<http://myerslab.mpi-cbg.de/wp-content/uploads/2014/06/np_diff.pdf>
	
Fowler, Noll, Vo; 1994  
<http://www.isthe.com/chongo/tech/comp/fnv/index.html>

Fowler, Noll, Vo on Wikipedia  
<https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function>

"diff", The Open Group Base Specifications Issue 7  
IEEE Std 1003.1, 2013 Edition  
<http://pubs.opengroup.org/onlinepubs/9699919799/utilities/diff.html>

Eugene W. Myers; "An O(ND) Difference Algorithm and Its Variations";  
Algorithmica, 1986, pages. 251-266  
<http://www.xmailserver.org/diff2.pdf>

Webb Miller and Eugene W. Myers; "A File Comparison Program";  
Software-Practice And Experience, Vol. 15(11). 1025-1040 (November 1985)  
<http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.189.70&rep=rep1&type=pdf>

D.S. Hirschberg, "A linear space algorithm for computing maximal common subsequence problem";  
Comm. of ACM, Vol. 18, June 1975, pages 341-343  
<http://www.mathcs.emory.edu/~cheung/Courses/323/Syllabus/DynProg/Docs/Hirschberg-LCS-1975.pdf>

Which hashing algorithm is best for uniqueness and speed?  
<http://programmers.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed>

[Wu+89]: http://myerslab.mpi-cbg.de/wp-content/uploads/2014/06/np_diff.pdf

[FNV94]: http://www.isthe.com/chongo/tech/comp/fnv/index.html

[FNVWi]: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

[Mye86]: http://www.xmailserver.org/diff2.pdf

[SUSV7]: http://pubs.opengroup.org/onlinepubs/9699919799/utilities/diff.html

[Mil85]: http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.189.70&rep=rep1&type=pdf

[Hir75]: http://www.mathcs.emory.edu/~cheung/Courses/323/Syllabus/DynProg/Docs/Hirschberg-LCS-1975.pdf

[HshCmp]: http://programmers.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed
