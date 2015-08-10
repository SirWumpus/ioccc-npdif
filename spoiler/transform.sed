#
#  Keywords & Phrases
#
/^#ifndef NDEBUG/,/#endif.*NDEBUG/d
/[[:<:]]error(/d
/[[:<:]]EPRINTF/d
/[[:<:]]INFO/d
/[[:<:]]INFO/d
/[[:<:]]DEBUG/d
/[[:<:]]DUMP/d

/#define FAST/d
/^#ifdef FAST/,/#else.*FAST/d
/^#endif.*FAST/d

/^#define EXIT_ERROR/d

/^#ifndef INIT_SIZE/,/#endif/d


# Comment lines
/^\/\//d

# Trailing comments
s/[[:blank:]]*\/\///

#
# Replace macros.
#

s/EXIT_ERROR/2/
s/EXIT_FAILURE/1/
s/EXIT_SUCCESS/0/
s/INIT_SIZE/32/g

#
#  Types
#

s/const //g
s/static //g
s/HashArray/H/g
s/HashLine/h/g
/typedef.*Hash/d
s/Hash/unsigned long/g
s/Edit/E/g
s/Vertex/X/g


#R#s/[[:<:]]int[[:>:]]/I/g
#R#s/[[:<:]]size_t[[:>:]]/I/g
s/[[:<:]]size_t[[:>:]]/unsigned long/g
#R#1i\
#R##define I int
#R#
#R#s/(char[[:blank:]]\*)//g
#R#s/char[[:blank:]]\*/C /g
#R#1i\
#R##define C char *
#R#
#R##2#s/(void) *//g
#R#s/[[:<:]]void[[:>:]]/V/
#3#s/void/V /
#R#1i\
#R##define V void
#R#
#R#s/char \(.*\)\[\] =/C \1 =/

#
# Reserved Words & Phrases
#

#R#s/for[[:blank:]](/F /
#R#1i\
#R##define F for(
#R#
#R#s/return/K/
#R#1i\
#R##define K return
#R#
#R#s/fputc(/T /
#R#1i\
#R##define T fputc(
#R#

#s/stderr/Q/
#1i\
##define Q stderr

#
# Constant expansion
#

s/NULL/0/g
s/'\\0'/0/g

#
#  Functions & Types
#

s/hash_expand/P/
s/hash_file/F/
s/hash_string/S/
s/echoline/W/
s/reverse_script/R/
s/dump_script/D/
s/snake/K/
s/edit_distance/T/

#
#  Variables
#
#  n, m, *s, *t, x, y are used for parameters and local variables.
#

/debug/d
/[[:<:]]print_distance[[:>:]]/d

s/argc/x/g
s/argv/y/g
s/orig/O/g
s/copy/Y/g
s/[[:<:]]ch[[:>:]]/i/g
s/[[:<:]]line[[:>:]]/B/g
s/lineno/L/g
s/length/j/g
s/prev/p/g
s/next/n/g
s/offset/o/g
s/curr/K/g
s/fpA/f/g
s/fpB/g/g
s/optind/1/g
s/[[:<:]]fp[[:>:]]/Q/g

s/[[:<:]]edit[[:>:]]/e/g
s/[[:<:]]seek[[:>:]]/q/g
s/[[:<:]]base[[:>:]]/b/g
s/[[:<:]]size[[:>:]]/z/g
s/hash/h/g

s/zero_delta/zd/g
s/zero/z/g
s/delta/d/g
s/invert/I/g

#
#  Expressions
#

#    (type *) 0		->   0
s/([a-z ]*\*) *0/0/g

#    if (0 == expr)	->  if (!expr)
s/0 == /!/g

#    if (expr != 0)	->  if (expr)
s/ != 0//g

#    if (0 != expr)	->  if (expr)
s/0 != //g

#    (v = func(expr)) == 0	->  !(v = func(expr))
s/\(([a-zA-Z0-9][a-zA-Z0-9]* = .*))\) == 0/!\1/g

#    (expr) == 0	->  !(expr)
s/\(([^()]*)\) == 0/!\1/g

#    expr == 0	->  !expr
s/\([^()]*\) == 0/!\1/g

#    func(expr) == 0	->  !func(expr)
s/\([a-zA-Z0-9][a-zA-Z0-9]*(.*)\) == 0/!\1/g

#    array[0]	->  *array
s/\([^ 	]*[^[]\)\[0\]/*\1/g

#    &array[n]	->  array+n
#N#s/\&\([^[]*\)\[\([^]]*\)\]/\1+\2/g


#
# Handle if() late.
#

#R#s/if[[:blank:]]*(/Q /
#R#1i\
#R##define Q if(
#R#

