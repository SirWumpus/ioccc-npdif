/*
 * npdif.c
 *
 * Wu, Manber, Myers, Miller; August 1989;
 * "An O(NP) Sequence Comparison Algorithm"
 *
 * Public Domain 2015 by Anthony Howe.  No warranty.
 *
 * Synopsis: npdif <file1> <file2>
 *
 * Exit Status
 *	0	no difference
 *	1	difference
 *	2	error
 */

#define FAST

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef INIT_SIZE
#define INIT_SIZE		32	/* Power of two. */
#endif

#define max(a, b)		((a) < (b) ? (b) : (a))

typedef unsigned long Hash;

typedef struct {
	long seek;
	Hash hash;
} HashLine;
		
typedef struct {
	size_t size;
	size_t length;
	HashLine base[1];
} HashArray;

static int debug;

#define EXIT_ERROR		2
#define error(x)		{ fputs("dif: ", stderr); perror(x); }

# define EPRINTF(f, ...)	(void) fprintf(stderr, f, __VA_ARGS__)
# define INFO(f, ...)		{ if (0 < debug) EPRINTF(f, __VA_ARGS__); }
# define DEBUG(f, ...)		{ if (1 < debug) EPRINTF(f, __VA_ARGS__); }
# define DUMP(f, ...)		{ if (2 < debug) EPRINTF(f, __VA_ARGS__); }

/**
 * FNV1a
 *
 * https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 * http://programmers.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed
 * http://www.isthe.com/chongo/tech/comp/fnv/index.html
 *
 *	Hash Size	Prime				Offset
 *	32-bit		16777619	0x01000193	2166136261		0x811C9DC5
 *	64-bit		1099511628211	0x100000001b3	14695981039346656037	0xCBF29CE34244AD25
 */
Hash
hash_string(unsigned char *x)
{
	Hash h = 2166136261UL;

	while (*x != '\0') {
		h ^= *x++;
#ifdef FAST
		/* 0x01000193 */
		h += (h<<1) + (h<<4) + (h<<7) + (h<<8) + (h<<24);
#else
		h *= 16777619UL;
#endif		
	}

	return h;
}

/*
 * Expand hash table.  The hash table holds the hashes for
 * both files, file1 is all even indexes and file2 all odd.
 * Index 0 holds the current size of the table.  Return 1
 * for failure to expand the table and 0 for success.
 */
HashArray *
hash_expand(HashArray *orig)
{
	size_t size;
	HashArray *copy;

	/* Size does not account for the initial base element.
	 * So you have 0 < size with a sentinel or 1 <= size.
	 */
	size = (orig == NULL ? INIT_SIZE : orig->size << 1);

	if (NULL == (copy = realloc(orig, sizeof (*orig) + size * sizeof (orig->base)))) {
		(void) fputs("dif: No memory.\n", stderr);
		free(orig);
		return NULL;
	}

	copy->size = size;
	if (orig == NULL)
		copy->length = 0;
	
	return copy;
}

/*
 * @param fp
 *	File to scan and hash by line.
 *
 * @return 
 *	Pointer to an array of hashes, where hash[0] is
 *	the length of the array and hash[1] is hash of the
 *	first line.
 */
HashArray *
hash_file(FILE *fp)
{
	HashArray *hash;
	long lineno, offset;
	unsigned char line[BUFSIZ];

	hash = hash_expand(NULL);
	
	/* Assume 1-base array. */
	for (lineno = 1; ; lineno++) {
		if (hash->size <= lineno && (hash = hash_expand(hash)) == NULL)
			break;
		offset = ftell(fp);
		if (fgets((char *)line, sizeof (line), fp) == NULL)
			break;		
		hash->base[lineno].hash = hash_string(line);
		hash->base[lineno].seek = offset;
	}

	/* Correct the length, base[1..length] inclusive. */	
	hash->length = lineno-1;
	
	return hash;
}

/*
 * Open file.
 *
 * If filename is the standard input filename -, copy
 * standard input to a temporary file.  Return pointer
 * to the temporary file open for reading.
 *
 * If filename is some other filename then return a
 * pointer to a file stream ready for reading.
 *
 * Any errors, write error message and return NULL.
 */
FILE *
file(char *u)
{
	int i;
	FILE *fp;

	if (NULL == (fp = fopen(u, "r")) && 0 == strcmp("-", u)) {
		if ((fp = tmpfile()) != NULL) {
			while ((i = getchar()) != EOF)
				(void) fputc(i, fp);
			rewind(fp);
		}
	}

	if (NULL == fp)
		error(u);

	return fp;
}

int
snake(int k, int y, HashArray *A, HashArray *B)
{
	int x;
	
	/*** NOTE: no hash collision check. ***/
	x = y - k;
	DUMP("snake in k=%d y=%d x=%d\n", k, y, x);

	/* The algorithm assumes 1-based indexing, A[1..M] and B[1..N]. */
	while (x < A->length && y < B->length && A->base[x+1].hash == B->base[y+1].hash) {
		x++;
		y++;
	}
	DUMP("snake out k=%d y=%d x=%d\n", k, y, x);
	
	return y;
}

int
edit_distance(HashArray *A, HashArray *B)
{
	int k, p, delta, *fp, zero, zero_delta;

	/* Swap A and B if N < M.  Edit distance will be the same,
	 * but the edit-script will not!
	 */
	if (A->length > B->length) {
		HashArray *tmp = A;
		A = B;
		B = tmp;
	}

	/* From -(M+1).. 0 .. (N+1); up & lower sentinels and zero. */
	if (NULL == (fp = malloc((A->length + B->length + 3) * sizeof (*fp))))
		return 0;		
		
	/* fp[-(M+1)..(N+1)] := -1 */		
	for (k = 0; k < A->length + B->length + 3; k++)
		fp[k] = -1;

	/* delta = N - M where A[1..M], B[1..N], and N >= M. */
	delta = B->length - A->length;
	
	/* Axis shift from -(M+1)..(N+1) => 0 .. zero .. (M+N+3). */
	zero = A->length + 2;
	zero_delta = zero + delta;
		
	p = -1;	

	DEBUG("delta=%d M=%d N=%d \n", delta, A->length, B->length);		
	do {
		p++;
		for (k = zero -p; k < zero_delta; k++) {
			fp[k] = snake(k -zero, max(fp[k-1] + 1, fp[k+1]), A, B);
			DEBUG("1st fp[%d]=%d p=%d \n", k, fp[k], p);		
		}
		for (k = zero_delta + p; zero_delta < k; k--) {
			fp[k] = snake(k -zero, max(fp[k-1] + 1, fp[k+1]), A, B);
			DEBUG("2nd fp[%d]=%d p=%d \n", k, fp[k], p);		
		}
		fp[zero_delta] = snake(delta, max(fp[zero_delta-1] + 1, fp[zero_delta+1]), A, B);
		DEBUG("3rd fp[%d]=%d p=%d \n", zero_delta, fp[zero_delta], p);		
	} while (fp[zero_delta] != B->length);

	free(fp);

	INFO("delta=%d p=%d M=%d N=%d \n", delta, p, A->length, B->length);		

	return delta + 2 * p;
}

int
main(int argc, char **argv)
{
	int ch, s;
	FILE *fp1, *fp2;
	HashArray *lines1, *lines2;

	while ((ch = getopt(argc, argv, "v")) != -1) {
		switch (ch) {
		case 'v':
			debug++;
			break;
		default:
			optind = argc;
			break;
		}
	}

	if (argc <= optind) {
		(void) fprintf(stderr, "usage: %s [-v] file1 file2\n", argv[0]);
		return EXIT_ERROR;
	}

	/* If file names are identical, assume identity result. */
	if (0 == strcmp(argv[optind], argv[optind+1]))
		return EXIT_SUCCESS;

	if (NULL == (fp1 = file(argv[optind])))
		return EXIT_ERROR;
	if (NULL == (fp2 = file(argv[optind+1])))
		return EXIT_ERROR;

	if (NULL == (lines1 = hash_file(fp1))) {
		error(argv[optind]);
		return EXIT_ERROR;
	}
	if (NULL == (lines2 = hash_file(fp2))) {
		error(argv[optind+1]);
		return EXIT_ERROR;
	}
		
	rewind(fp1);
	rewind(fp2);
		
	s = edit_distance(lines1, lines2);
	(void) printf("%d\n", s);

	free(lines1);
	free(lines2);

	if (ferror(fp1)) {
		error(argv[optind]);
		return EXIT_ERROR;
	}
	if (ferror(fp2)) {
		error(argv[optind+1]);
		return EXIT_ERROR;
	}

	return EXIT_SUCCESS;
}
