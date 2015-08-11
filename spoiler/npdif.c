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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef INIT_SIZE
#define INIT_SIZE		32	/* Power of two. */
#endif

typedef struct edit {
	int op;			/* 1 = insert, 0 = delete */
	int x;			/* Index into A. */
	int y;			/* Index into B. */
	long a;			/* seek into A */
	long b;			/* seek into B */
	struct edit *next;
} Edit;

typedef struct {
	int y;
	Edit *edit;
} Vertex;
	
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
static int invert;
static int print_distance;

#define EXIT_ERROR		2
#define error(x)		{ (void) fputs("dif: ", stderr); perror(x); }

#define EPRINTF(...)		(void) fprintf(stderr, __VA_ARGS__)
#define INFO(...)		{ if (0 < debug) EPRINTF(__VA_ARGS__); }
#define DEBUG(...)		{ if (1 < debug) EPRINTF(__VA_ARGS__); }
#define DUMP(...)		{ if (2 < debug) EPRINTF(__VA_ARGS__); }

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
#else /* FAST */
		h *= 16777619UL;
#endif /* FAST */
	}

	return h;
}

/*
 * Expand hash table.  
 */
HashArray *
hash_expand(HashArray *orig)
{
	size_t size;
	HashArray *copy;

	/* Size does not account for the initial base element.
	 * So you have 0 < size with a sentinel or 1 <= size.
	 */
	size = orig != NULL ? orig->size << 1 : INIT_SIZE;

	if (NULL == (copy = realloc(orig, sizeof (*orig) + size * sizeof (orig->base)))) {
		(void) fputs("No memory.\n", stderr);
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
 *	Pointer to an 1-based array of hashed lines.
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
		if (NULL == fgets((char *)line, sizeof (line), fp))
			break;		
		hash->base[lineno].hash = hash_string(line);
		hash->base[lineno].seek = offset;
	}

	/* Correct the length, base[1..length] inclusive. */	
	hash->length = lineno-1;
	rewind(fp);
	
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

	if (NULL == fp) error(u);

	return fp;
}

void
echoline(FILE *fp)
{
	int ch;
	while ((ch = fgetc(fp)) != EOF) {
		(void) putchar(ch);						
		if (ch == '\n')
			break;
	}
}

Edit *
reverse_script(Edit *curr)
{
	Edit *a, *b;
	
	b = NULL;
	while (curr != NULL) {
		a = curr->next;
		curr->next = b;
		b = curr;
		curr = a;
	}
	
	return b;
}

void
dump_script(FILE *fpA, FILE *fpB, Edit *curr)
{
	Edit *a, *b;

	for (curr = reverse_script(curr); curr != NULL; ) {
		/* Determine range. */
		b = curr;
		do {
			a = b;
DEBUG("curr.x=%d curr.y=%d a.x=%d a.y=%d b.x=%d b.y=%d\n", curr->x, curr->y, a->x, a->y, b->x, b->y);			
			b = b->next;
		} while (b != NULL && a->x == b->x);

		if (curr->op) {
			if (curr->y < a->y)
				(void) printf("%da%d,%d\n", curr->x, curr->y, a->y);
			else
				(void) printf("%da%d\n", curr->x, curr->y);
			(void) fseek(fpB, curr->b, SEEK_SET);
		
			for ( ; curr != b; curr = curr->next) {
				(void) printf("> ");
				echoline(fpB);
			}
		} else {
			if (curr->y < a->y)
				printf("%d,%dd%d\n", curr->y, a->y, curr->x);
			else
				printf("%dd%d\n", curr->y, curr->x);
			(void) fseek(fpA, curr->a, SEEK_SET);

			for ( ; curr != b; curr = curr->next) {
				(void) printf("< ");
				echoline(fpA);
			}
		}
	}	
}

void
snake(int zero_k, Vertex *fp, HashArray *A, HashArray *B)
{
	int k = zero_k - A->length - 1;
	
	/* fp[] holds the furthest y along diagonal k. */
	Vertex h = fp[zero_k-1];
	Vertex v = fp[zero_k+1];
	
	/* max(fp[zero_k-1] + 1, fp[zero_k+1]) */
	int y, op;
	Edit *prev;
	if (v.y < h.y+1) {
		op = 1;
		y = h.y+1;
		prev = h.edit;
	} else {
		op = 0;
		y = v.y;
		prev = v.edit;
	}

	/* Diagonal k = y - x of matrix [-(M+1), (N-1)], where
	 * row 0 and column 0 are initialised with sentenial -1.
	 *
	 * k-diagonal	= A and B match
	 * y-horizontal	= insert from B
	 * x-vertical	= delete from A
	 */
	int x = y - k;
	DUMP("snake in k=%d y=%d x=%d\n", k, y, x);

	if (0 < y || 0 < x) {
		Edit *edit = malloc(sizeof (*edit));	
		edit->x = x;
		edit->y = y;		
		edit->op = op ^ invert;
		edit->next = prev;
		edit->a = A->base[edit->x].seek;
		edit->b = B->base[edit->y].seek;
		
		if (invert) {
			long c = edit->a;
			edit->a = edit->b;
			edit->b = c;
		}
		
		fp[zero_k].edit = edit;
		DUMP("edit op=%d x=%d y=%d\n", edit->op, edit->x, edit->y);
	}

	/* The algorithm assumes 1-based indexing, A[1..M] and B[1..N]. 
	 * NOTE no hash collision checking yet.
	 */
	while (x < A->length && y < B->length && A->base[x+1].hash == B->base[y+1].hash) {
		x++;
		y++;
	}
	
	fp[zero_k].y = y;
	
	DUMP("snake out k=%d y=%d x=%d\n", k, y, x);
}

int
edit_distance(FILE *fpA, FILE *fpB, HashArray *A, HashArray *B)
{
	Vertex *fp;
	int k, p, delta, zero, zero_delta;

	/* Swap A and B if N < M.  The edit distance will be the
	 * same, but edit script operations will be reversed.
	 */
	if (A->length > B->length) {
		void *tmp = A;
		A = B;
		B = tmp;
		invert = 1;
	}

	/* delta = N - M where A[1..M], B[1..N], and N >= M. */
	delta = B->length - A->length;
	
	/* Axis shift from -(M+1)..(N+1) => 0 .. (M+1) .. (M+N+3). */
	zero = A->length + 1;
	zero_delta = zero + delta;

	/* From -(M+1).. 0 .. (N+1); up & lower sentinels and zero. */
	if (NULL == (fp = calloc(A->length + B->length + 3, sizeof (*fp))))
		return -1;		
		
	/* fp[-(M+1)..(N+1)] := -1 */		
	for (k = 0; k < A->length + B->length + 3; k++)
		fp[k].y = -1;

	DEBUG("delta=%d M=%zu N=%zu fp.len=%zu zero=%d zero_delta=%d\n", delta, A->length, B->length, (A->length + B->length + 3), zero, zero_delta);		

	p = -1;
	do {
		p++;
		for (k = zero - p; k < zero_delta; k++) {
			snake(k, fp, A, B);
			DEBUG("1st fp[%d]=%d p=%d \n", k, fp[k].y, p);		
		}
		for (k = zero_delta + p; zero_delta < k; k--) {
			snake(k, fp, A, B);
			DEBUG("2nd fp[%d]=%d p=%d \n", k, fp[k].y, p);		
		}
		snake(zero_delta, fp, A, B);
		DEBUG("3rd fp[%d]=%d p=%d \n", zero_delta, fp[zero_delta].y, p);		
	} while (fp[zero_delta].y != B->length);

	if (!print_distance)
		dump_script(fpA, fpB, fp[zero_delta].edit);
	free(fp);

	DEBUG("dist=%d delta=%d p=%d M=%zu N=%zu \n", delta + 2 * p, delta, p, A->length, B->length);		

	return delta + 2 * p;
}

int
main(int argc, char **argv)
{
	int ch;
	FILE *fpA, *fpB;
	HashArray *A, *B;

#ifndef NDEBUG
#include <getopt.h>

	while ((ch = getopt(argc, argv, "dv")) != -1) {
		switch (ch) {
		case 'd':
			print_distance = 1;
			break;
		case 'v':
			debug++;
			break;
		default:
			optind = argc;
			break;
		}
	}

	if (argc <= optind) {
		(void) fprintf(stderr, "usage: %s [-dv] file1 file2\n", argv[0]);
		return EXIT_ERROR;
	}
#endif /* NDEBUG */

	if (NULL == (fpA = file(argv[optind])))
		return EXIT_ERROR;
	if (NULL == (fpB = file(argv[optind+1])))
		return EXIT_ERROR;

	if (NULL == (A = hash_file(fpA))) {
		error(argv[optind]);
		return EXIT_ERROR;
	}
	if (NULL == (B = hash_file(fpB))) {
		error(argv[optind+1]);
		return EXIT_ERROR;
	}

	ch = edit_distance(fpA, fpB, A, B);
	if (print_distance) printf("%d\n", ch);

	free(A);
	free(B);

	if (ferror(fpA)) {
		error(argv[optind]);
		return EXIT_ERROR;
	}
	fclose(fpA);

	if (ferror(fpB)) {
		error(argv[optind+1]);
		return EXIT_ERROR;
	}	
	fclose(fpB);

	return 0 < ch;
}
