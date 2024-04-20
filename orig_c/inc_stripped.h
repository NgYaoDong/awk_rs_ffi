
#include <features.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <math.h>

// /usr/include/x86_64-linux-gnu/bits/types/FILE.h
#include <stdio.h>
// uint32_t
// #include <stddef.h>
#include <stdint.h>

// regex_t
#include <regex.h>

// // autoconf.h
# define IF_FEATURE_AWK_GNU_EXTENSIONS(...) __VA_ARGS__
#define ENABLE_FEATURE_SYSLOG 1
#define ENABLE_FEATURE_AWK_LIBM 1
#define ENABLE_FEATURE_ALLOW_EXEC 1
#define ENABLE_LOCALE_SUPPORT 0
#define ENABLE_FEATURE_AWK_GNU_EXTENSIONS 1
#define ENABLE_DESKTOP 1

// // platform.h
typedef signed char smallint;
typedef unsigned char smalluint;
# define ALIGN1 __attribute__((aligned(1)))
# define ALIGN2 __attribute__((aligned(2)))
# define ALIGN4 __attribute__((aligned(4)))
#define UNUSED_PARAM __attribute__ ((__unused__))
#define NORETURN __attribute__ ((__noreturn__))
#  define FAST_FUNC
# define NOINLINE
# define ALWAYS_INLINE inline
#define RETURNS_MALLOC __attribute__ ((malloc))
#define ALIGN_PTR  __attribute__((aligned(sizeof(void*))))


// // libbb.h
enum {
	LOGMODE_NONE = 0,
	LOGMODE_STDIO = (1 << 0),
	LOGMODE_SYSLOG = (1 << 1) * ENABLE_FEATURE_SYSLOG,
	LOGMODE_BOTH = LOGMODE_SYSLOG + LOGMODE_STDIO,
};
#define GETOPT_RESET() (optind = 0)
char *safe_strncpy(char *dst, const char *src, size_t size) FAST_FUNC;
void overlapping_strcpy(char *dst, const char *src) FAST_FUNC;
#include <errno.h>
// # define errno (*bb_errno)
void bb_die_memory_exhausted(void) NORETURN FAST_FUNC;
void bb_simple_error_msg(const char *s) FAST_FUNC;
void bb_simple_perror_msg_and_die(const char *s) NORETURN FAST_FUNC;
void bb_error_msg_and_die(const char *s, ...) __attribute__ ((noreturn, format (printf, 1, 2))) FAST_FUNC;
void bb_simple_error_msg_and_die(const char *s) NORETURN FAST_FUNC;
void bb_perror_msg_and_die(const char *s, ...) __attribute__ ((noreturn, format (printf, 1, 2))) FAST_FUNC;
void bb_perror_msg(const char *s, ...) __attribute__ ((format (printf, 1, 2))) FAST_FUNC;
void bb_verror_msg(const char *s, va_list p, const char *strerr) FAST_FUNC;
void bb_simple_perror_msg(const char *s) FAST_FUNC;
/* This struct is deliberately not defined. */
/* See docs/keep_data_small.txt */
struct globals;
/* '*const' ptr makes gcc optimize code much better.
 * Magic prevents ptr_to_globals from going into rodata.
 * If you want to assign a value, use SET_PTR_TO_GLOBALS(x) */
extern struct globals *const ptr_to_globals;
#define ARRAY_SIZE(x) ((unsigned)(sizeof(x) / sizeof((x)[0])))
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
char *xstrdup(const char *s) FAST_FUNC RETURNS_MALLOC;
void xsetegid(gid_t egid) FAST_FUNC;
void xseteuid(uid_t euid) FAST_FUNC;
extern ssize_t full_write(int fd, const void *buf, size_t count) FAST_FUNC;
#  define OFF_FMT "l"
void *xmalloc(size_t size) FAST_FUNC RETURNS_MALLOC;
void *xzalloc(size_t size) FAST_FUNC RETURNS_MALLOC;
void *xrealloc(void *old, size_t size) FAST_FUNC;
extern char bb_process_escape_sequence(const char **ptr) FAST_FUNC;
char* strcpy_and_process_escape_sequences(char *dst, const char *src) FAST_FUNC;
char *utoa(unsigned n) FAST_FUNC;
char *itoa(int n) FAST_FUNC;
// /* Some useful definitions */
#undef FALSE
#define FALSE   ((int) 0)
#undef TRUE
#define TRUE    ((int) 1)
#undef SKIP
#define SKIP	((int) 2)
/* These work the same for ASCII and Unicode,
 * assuming no one asks "is this a *Unicode* letter?" using isalpha(letter) */
#define isascii(a) ((unsigned char)(a) <= 0x7f)
#define isdigit(a) ((unsigned char)((a) - '0') <= 9)
#define isupper(a) ((unsigned char)((a) - 'A') <= ('Z' - 'A'))
#define islower(a) ((unsigned char)((a) - 'a') <= ('z' - 'a'))
#define isalpha(a) ((unsigned char)(((a)|0x20) - 'a') <= ('z' - 'a'))
#define isblank(a) ({ unsigned char bb__isblank = (a); bb__isblank == ' ' || bb__isblank == '\t'; })
#define iscntrl(a) ({ unsigned char bb__iscntrl = (a); bb__iscntrl < ' ' || bb__iscntrl == 0x7f; })
/* In POSIX/C locale isspace is only these chars: "\t\n\v\f\r" and space.
 * "\t\n\v\f\r" happen to have ASCII codes 9,10,11,12,13.
 */
#define isspace(a) ({ unsigned char bb__isspace = (a) - 9; bb__isspace == (' ' - 9) || bb__isspace <= (13 - 9); })
// Unsafe wrt NUL: #define ispunct(a) (strchr("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~", (a)) != NULL)
#define ispunct(a) (strchrnul("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~", (a))[0])
// Bigger code: #define isalnum(a) ({ unsigned char bb__isalnum = (a) - '0'; bb__isalnum <= 9 || ((bb__isalnum - ('A' - '0')) & 0xdf) <= 25; })
#define isalnum(a) bb_ascii_isalnum(a)
static ALWAYS_INLINE int bb_ascii_isalnum(unsigned char a)
{
	unsigned char b = a - '0';
	if (b <= 9)
		return (b <= 9);
	b = (a|0x20) - 'a';
	return b <= 'z' - 'a';
}
#define isxdigit(a) bb_ascii_isxdigit(a)
static ALWAYS_INLINE int bb_ascii_isxdigit(unsigned char a)
{
	unsigned char b = a - '0';
	if (b <= 9)
		return (b <= 9);
	b = (a|0x20) - 'a';
	return b <= 'f' - 'a';
}
#define toupper(a) bb_ascii_toupper(a)
static ALWAYS_INLINE unsigned char bb_ascii_toupper(unsigned char a)
{
	unsigned char b = a - 'a';
	if (b <= ('z' - 'a'))
		a -= 'a' - 'A';
	return a;
}
#define tolower(a) bb_ascii_tolower(a)
static ALWAYS_INLINE unsigned char bb_ascii_tolower(unsigned char a)
{
	unsigned char b = a - 'A';
	if (b <= ('Z' - 'A'))
		a += 'a' - 'A';
	return a;
}

char *is_prefixed_with(const char *string, const char *key) FAST_FUNC;
char *is_suffixed_with(const char *string, const char *key) FAST_FUNC;
extern char *skip_whitespace(const char *) FAST_FUNC;
extern ssize_t safe_read(int fd, void *buf, size_t count) FAST_FUNC;
char *xstrndup(const char *s, int n) FAST_FUNC RETURNS_MALLOC;
char *xasprintf(const char *format, ...) __attribute__ ((format(printf, 1, 2))) FAST_FUNC RETURNS_MALLOC;
const char* endofname(const char *name) FAST_FUNC;
int xopen_stdin(const char *pathname) FAST_FUNC;
int open_or_warn(const char *pathname, int flags) FAST_FUNC;
FILE* fopen_or_warn(const char *filename, const char *mode) FAST_FUNC;
FILE* xfopen_stdin(const char *filename) FAST_FUNC;
FILE* xfopen(const char *filename, const char *mode) FAST_FUNC;
int fflush_all(void) FAST_FUNC;
#define MAIN_EXTERNALLY_VISIBLE
typedef struct llist_t {
	struct llist_t *link;
	char *data;
} llist_t;
void llist_add_to(llist_t **old_head, void *data) FAST_FUNC;
void llist_add_to_end(llist_t **list_head, void *data) FAST_FUNC;
void *llist_pop(llist_t **elm) FAST_FUNC;
void llist_unlink(llist_t **head, llist_t *elm) FAST_FUNC;
void llist_free(llist_t *elm, void (*freeit)(void *data)) FAST_FUNC;
uint32_t getopt32(char **argv, const char *applet_opts, ...) FAST_FUNC;
int getoptind();
void bb_show_usage(void) NORETURN FAST_FUNC;
extern void *xmalloc_read(int fd, size_t *maxsz_p) FAST_FUNC RETURNS_MALLOC;


/* At least gcc 3.4.6 on mipsel system needs optimization barrier */
// #define barrier() __asm__ __volatile__("":::"memory")
// translate barrier to a function
#define barrier() barrier_func()
static void barrier_func(void) {
	__asm__ __volatile__("":::"memory");
}


/* Clang/llvm drops assignment to "constant" storage. Silently.
 * Needs serious convincing to not eliminate the store.
 */
static ALWAYS_INLINE void* not_const_pp(const void *p)
{
	void *pp;
	__asm__ __volatile__(
		"# forget that p points to const"
		: /*outputs*/ "=r" (pp)
		: /*inputs*/ "0" (p)
	);
	return pp;
}

// #define SET_PTR_TO_GLOBALS(x) do { \
// 	(*(struct globals**)not_const_pp(&ptr_to_globals)) = (void*)(x); \
// 	barrier(); \
// } while (0)
// translate SET_PTR_TO_GLOBALS to a function
#define SET_PTR_TO_GLOBALS(x) set_ptr_to_globals(x)
static void set_ptr_to_globals(void *x) {
	(*(struct globals**)not_const_pp(&ptr_to_globals)) = (void*)(x);
	barrier_func();
}

void xfunc_die(void) NORETURN FAST_FUNC;
FILE* fopen_for_read(const char *path) FAST_FUNC;

extern ssize_t full_read(int fd, void *buf, size_t count) FAST_FUNC;
/* Last element is marked by mult == 0 */
struct suffix_mult {
	char suffix[4];
	uint32_t mult;
};
#define ALIGN_SUFFIX ALIGN4
/* Using xatoi() instead of naive atoi() is not always convenient -
 * in many places people want *non-negative* values, but store them
 * in signed int. Therefore we need this one:
 * dies if input is not in [0, INT_MAX] range. Also will reject '-0' etc.
 * It should really be named xatoi_nonnegative (since it allows 0),
 * but that would be too long.
 */
int xatoi_positive(const char *numstr) FAST_FUNC;
extern const char *applet_name;
#define NOT_LONE_DASH(s) ((s)[0] != '-' || (s)[1])
# define setlocale(x,y) ((void)0)
#define ENABLE_DEBUG 1

// ///// The awk_main
int awk_main(int argc, char **argv);

// ///// xregex.h
char* regcomp_or_errmsg(regex_t *preg, const char *regex, int cflags) FAST_FUNC;
void xregcomp(regex_t *preg, const char *regex, int cflags) FAST_FUNC;


#include <bits/types.h>


// moved from awk.c start
/* If you comment out one of these below, it will be #defined later
 * to perform debug printfs to stderr: */
#define debug_printf_walker(...)  do {} while (0)
#define debug_printf_eval(...)  do {} while (0)
#define debug_printf_parse(...)  do {} while (0)

#ifndef debug_printf_walker
# define debug_printf_walker(...) (fprintf(stderr, __VA_ARGS__))
#endif
#ifndef debug_printf_eval
# define debug_printf_eval(...) (fprintf(stderr, __VA_ARGS__))
#endif
#ifndef debug_printf_parse
# define debug_printf_parse(...) (fprintf(stderr, __VA_ARGS__))
#endif


/* "+": stop on first non-option:
 * $ awk 'BEGIN { for(i=1; i<ARGC; ++i) { print i ": " ARGV[i] }}' -argz
 * 1: -argz
 */
#define OPTSTR_AWK "+" \
	"F:v:*f:*" \
	IF_FEATURE_AWK_GNU_EXTENSIONS("e:*") \
	"W:"
enum {
	OPTBIT_F,	/* define field separator */
	OPTBIT_v,	/* define variable */
	OPTBIT_f,	/* pull in awk program from file */
	IF_FEATURE_AWK_GNU_EXTENSIONS(OPTBIT_e,) /* -e AWK_PROGRAM */
	OPTBIT_W,	/* -W ignored */
	OPT_F = 1 << OPTBIT_F,
	OPT_v = 1 << OPTBIT_v,
	OPT_f = 1 << OPTBIT_f,
	OPT_e = IF_FEATURE_AWK_GNU_EXTENSIONS((1 << OPTBIT_e)) + 0,
	OPT_W = 1 << OPTBIT_W
};

#define	MAXVARFMT       240
#define	MINNVBLOCK      64

/* variable flags */
#define	VF_NUMBER       0x0001	/* 1 = primary type is number */
#define	VF_ARRAY        0x0002	/* 1 = it's an array */

#define	VF_CACHED       0x0100	/* 1 = num/str value has cached str/num eq */
#define	VF_USER         0x0200	/* 1 = user input (may be numeric string) */
#define	VF_SPECIAL      0x0400	/* 1 = requires extra handling when changed */
#define	VF_WALK         0x0800	/* 1 = variable has alloc'd x.walker list */
#define	VF_FSTR         0x1000	/* 1 = var::string points to fstring buffer */
#define	VF_CHILD        0x2000	/* 1 = function arg; x.parent points to source */
#define	VF_DIRTY        0x4000	/* 1 = variable was set explicitly */

/* these flags are static, don't change them when value is changed */
#define	VF_DONTTOUCH    (VF_ARRAY | VF_SPECIAL | VF_WALK | VF_CHILD | VF_DIRTY)

typedef struct walker_list {
	char *end;
	char *cur;
	struct walker_list *prev;
	char wbuf[1];
} walker_list;

/* Variable */
typedef struct var_s {
	unsigned type;            /* flags */
	double number;
	char *string;
	union {
		int aidx;               /* func arg idx (for compilation stage) */
		struct xhash_s *array;  /* array ptr */
		struct var_s *parent;   /* for func args, ptr to actual parameter */
		walker_list *walker;    /* list of array elements (for..in) */
	} x;
} var;

/* Node chain (pattern-action chain, BEGIN, END, function bodies) */
typedef struct chain_s {
	struct node_s *first;
	struct node_s *last;
	const char *programname;
} chain;

/* Function */
typedef struct func_s {
	unsigned nargs;
	struct chain_s body;
} func;

/* I/O stream */
typedef struct rstream_s {
	FILE *F;
	char *buffer;
	int adv;
	int size;
	int pos;
	smallint is_pipe;
} rstream;

typedef struct hash_item_s {
	union {
		struct var_s v;         /* variable/array hash */
		struct rstream_s rs;    /* redirect streams hash */
		struct func_s f;        /* functions hash */
	} data;
	struct hash_item_s *next;       /* next in chain */
	char name[1];                   /* really it's longer */
} hash_item;

typedef struct xhash_s {
	unsigned nel;           /* num of elements */
	unsigned csize;         /* current hash size */
	unsigned nprime;        /* next hash size in PRIMES[] */
	unsigned glen;          /* summary length of item names */
	struct hash_item_s **items;
} xhash;

/* Tree node */
typedef struct node_s {
	uint32_t info;
	unsigned lineno;
	union {
		struct node_s *n;
		var *v;
		int aidx;
		char *new_progname;
		regex_t *re;
	} l;
	union {
		struct node_s *n;
		regex_t *ire;
		func *f;
	} r;
	union {
		struct node_s *n;
	} a;
} node;

/* Block of temporary variables */
typedef struct nvblock_s {
	int size;
	var *pos;
	struct nvblock_s *prev;
	struct nvblock_s *next;
	var nv[];
} nvblock;

typedef struct tsplitter_s {
	node n;
	regex_t re[2];
} tsplitter;

/* simple token classes */
/* Order and hex values are very important!!!  See next_token() */
#define	TC_SEQSTART	(1 << 0)		/* ( */
#define	TC_SEQTERM	(1 << 1)		/* ) */
#define	TC_REGEXP	(1 << 2)		/* /.../ */
#define	TC_OUTRDR	(1 << 3)		/* | > >> */
#define	TC_UOPPOST	(1 << 4)		/* unary postfix operator */
#define	TC_UOPPRE1	(1 << 5)		/* unary prefix operator */
#define	TC_BINOPX	(1 << 6)		/* two-opnd operator */
#define	TC_IN		(1 << 7)
#define	TC_COMMA	(1 << 8)
#define	TC_PIPE		(1 << 9)		/* input redirection pipe */
#define	TC_UOPPRE2	(1 << 10)		/* unary prefix operator */
#define	TC_ARRTERM	(1 << 11)		/* ] */
#define	TC_GRPSTART	(1 << 12)		/* { */
#define	TC_GRPTERM	(1 << 13)		/* } */
#define	TC_SEMICOL	(1 << 14)
#define	TC_NEWLINE	(1 << 15)
#define	TC_STATX	(1 << 16)		/* ctl statement (for, next...) */
#define	TC_WHILE	(1 << 17)
#define	TC_ELSE		(1 << 18)
#define	TC_BUILTIN	(1 << 19)
/* This costs ~50 bytes of code.
 * A separate class to support deprecated "length" form. If we don't need that
 * (i.e. if we demand that only "length()" with () is valid), then TC_LENGTH
 * can be merged with TC_BUILTIN:
 */
#define	TC_LENGTH	(1 << 20)
#define	TC_GETLINE	(1 << 21)
#define	TC_FUNCDECL	(1 << 22)		/* 'function' 'func' */
#define	TC_BEGIN	(1 << 23)
#define	TC_END		(1 << 24)
#define	TC_EOF		(1 << 25)
#define	TC_VARIABLE	(1 << 26)
#define	TC_ARRAY	(1 << 27)
#define	TC_FUNCTION	(1 << 28)
#define	TC_STRING	(1 << 29)
#define	TC_NUMBER	(1 << 30)

#define	TC_UOPPRE  (TC_UOPPRE1 | TC_UOPPRE2)

/* combined token classes */
#define	TC_BINOP   (TC_BINOPX | TC_COMMA | TC_PIPE | TC_IN)
//#define	TC_UNARYOP (TC_UOPPRE | TC_UOPPOST)
#define	TC_OPERAND (TC_VARIABLE | TC_ARRAY | TC_FUNCTION \
                   | TC_BUILTIN | TC_LENGTH | TC_GETLINE \
                   | TC_SEQSTART | TC_STRING | TC_NUMBER)

#define	TC_STATEMNT (TC_STATX | TC_WHILE)
#define	TC_OPTERM  (TC_SEMICOL | TC_NEWLINE)

/* word tokens, cannot mean something else if not expected */
#define	TC_WORD    (TC_IN | TC_STATEMNT | TC_ELSE \
                   | TC_BUILTIN | TC_LENGTH | TC_GETLINE \
                   | TC_FUNCDECL | TC_BEGIN | TC_END)

/* discard newlines after these */
#define	TC_NOTERM  (TC_COMMA | TC_GRPSTART | TC_GRPTERM \
                   | TC_BINOP | TC_OPTERM)

/* what can expression begin with */
#define	TC_OPSEQ   (TC_OPERAND | TC_UOPPRE | TC_REGEXP)
/* what can group begin with */
#define	TC_GRPSEQ  (TC_OPSEQ | TC_OPTERM | TC_STATEMNT | TC_GRPSTART)

/* if previous token class is CONCAT1 and next is CONCAT2, concatenation */
/* operator is inserted between them */
#define	TC_CONCAT1 (TC_VARIABLE | TC_ARRTERM | TC_SEQTERM \
                   | TC_STRING | TC_NUMBER | TC_UOPPOST \
                   | TC_LENGTH)
#define	TC_CONCAT2 (TC_OPERAND | TC_UOPPRE)

#define	OF_RES1     0x010000
#define	OF_RES2     0x020000
#define	OF_STR1     0x040000
#define	OF_STR2     0x080000
#define	OF_NUM1     0x100000
#define	OF_CHECKED  0x200000
#define	OF_REQUIRED 0x400000


/* combined operator flags */
#define	xx	0
#define	xV	OF_RES2
#define	xS	(OF_RES2 | OF_STR2)
#define	Vx	OF_RES1
#define	Rx	(OF_RES1 | OF_NUM1 | OF_REQUIRED)
#define	VV	(OF_RES1 | OF_RES2)
#define	Nx	(OF_RES1 | OF_NUM1)
#define	NV	(OF_RES1 | OF_NUM1 | OF_RES2)
#define	Sx	(OF_RES1 | OF_STR1)
#define	SV	(OF_RES1 | OF_STR1 | OF_RES2)
#define	SS	(OF_RES1 | OF_STR1 | OF_RES2 | OF_STR2)

#define	OPCLSMASK 0xFF00
#define	OPNMASK   0x007F

/* operator priority is a highest byte (even: r->l, odd: l->r grouping)
 * For builtins it has different meaning: n n s3 s2 s1 v3 v2 v1,
 * n - min. number of args, vN - resolve Nth arg to var, sN - resolve to string
 */
#undef P
#undef PRIMASK
#undef PRIMASK2
#define P(x)      (x << 24)
#define PRIMASK   0x7F000000
#define PRIMASK2  0x7E000000

/* Operation classes */

#define	SHIFT_TIL_THIS	0x0600
#define	RECUR_FROM_THIS	0x1000

enum {
	OC_DELETE = 0x0100,     OC_EXEC = 0x0200,       OC_NEWSOURCE = 0x0300,
	OC_PRINT = 0x0400,      OC_PRINTF = 0x0500,     OC_WALKINIT = 0x0600,

	OC_BR = 0x0700,         OC_BREAK = 0x0800,      OC_CONTINUE = 0x0900,
	OC_EXIT = 0x0a00,       OC_NEXT = 0x0b00,       OC_NEXTFILE = 0x0c00,
	OC_TEST = 0x0d00,       OC_WALKNEXT = 0x0e00,

	OC_BINARY = 0x1000,     OC_BUILTIN = 0x1100,    OC_COLON = 0x1200,
	OC_COMMA = 0x1300,      OC_COMPARE = 0x1400,    OC_CONCAT = 0x1500,
	OC_FBLTIN = 0x1600,     OC_FIELD = 0x1700,      OC_FNARG = 0x1800,
	OC_FUNC = 0x1900,       OC_GETLINE = 0x1a00,    OC_IN = 0x1b00,
	OC_LAND = 0x1c00,       OC_LOR = 0x1d00,        OC_MATCH = 0x1e00,
	OC_MOVE = 0x1f00,       OC_PGETLINE = 0x2000,   OC_REGEXP = 0x2100,
	OC_REPLACE = 0x2200,    OC_RETURN = 0x2300,     OC_SPRINTF = 0x2400,
	OC_TERNARY = 0x2500,    OC_UNARY = 0x2600,      OC_VAR = 0x2700,
	OC_DONE = 0x2800,

	ST_IF = 0x3000,         ST_DO = 0x3100,         ST_FOR = 0x3200,
	ST_WHILE = 0x3300
};

/* simple builtins */
enum {
	F_in,	F_rn,	F_co,	F_ex,	F_lg,	F_si,	F_sq,	F_sr,
	F_ti,	F_le,	F_sy,	F_ff,	F_cl
};

/* builtins */
enum {
	B_a2,	B_ix,	B_ma,	B_sp,	B_ss,	B_ti,   B_mt,	B_lo,	B_up,
	B_ge,	B_gs,	B_su,
	B_an,	B_co,	B_ls,	B_or,	B_rs,	B_xo,
};

/* tokens and their corresponding info values */

#define NTC     "\377"  /* switch to next token class (tc<<1) */
#define NTCC    '\377'
#define OC_B  OC_BUILTIN

extern const char tokenlist[] ALIGN1;
extern const uint32_t tokeninfo[] ALIGN4;

/* internal variable names and their initial values       */
/* asterisk marks SPECIAL vars; $ is just no-named Field0 */
enum {
	CONVFMT,    OFMT,       FS,         OFS,
	ORS,        RS,         RT,         FILENAME,
	SUBSEP,     F0,         ARGIND,     ARGC,
	ARGV,       ERRNO,      FNR,        NR,
	NF,         IGNORECASE, ENVIRON,    NUM_INTERNAL_VARS
};

extern const char vNames[] ALIGN1;
extern const char vValues[] ALIGN1;
extern const uint16_t PRIMES[] ALIGN2;

/* hash size may grow to these values */
#define FIRST_PRIME 61

/* Globals. Split in two parts so that first one is addressed
 * with (mostly short) negative offsets.
 * NB: it's unsafe to put members of type "double"
 * into globals2 (gcc may fail to align them).
 */
struct globals {
	double t_double;
	chain beginseq, mainseq, endseq;
	chain *seq;
	node *break_ptr, *continue_ptr;
	rstream *iF;
	xhash *vhash, *ahash, *fdhash, *fnhash;
	const char *g_progname;
	int g_lineno;
	int nfields;
	int maxfields; /* used in fsrealloc() only */
	var *Fields;
	nvblock *g_cb;
	char *g_pos;
	char *g_buf;
	smallint icase;
	smallint exiting;
	smallint nextrec;
	smallint nextfile;
	smallint is_f0_split;
	smallint t_rollback;
};
struct globals2 {
	uint32_t t_info; /* often used */
	uint32_t t_tclass;
	char *t_string;
	int t_lineno;

	var *intvar[NUM_INTERNAL_VARS]; /* often used */

	/* former statics from various functions */
	char *split_f0__fstrings;

	uint32_t next_token__save_tclass;
	uint32_t next_token__save_info;
	uint32_t next_token__ltclass;
	smallint next_token__concat_inserted;

	smallint next_input_file__files_happen;
	rstream next_input_file__rsm;

	var *evaluate__fnargs;
	unsigned evaluate__seed;
	regex_t evaluate__sreg;

	var ptest__v;

	tsplitter exec_builtin__tspl;

	/* biggest and least used members go last */
	tsplitter fsplitter, rsplitter;
};
#define G1 (ptr_to_globals[-1])
#define G (*(struct globals2 *)ptr_to_globals)
/* For debug. nm --size-sort awk.o | grep -vi ' [tr] ' */
/*char G1size[sizeof(G1)]; - 0x74 */
/*char Gsize[sizeof(G)]; - 0x1c4 */
/* Trying to keep most of members accessible with short offsets: */
/*char Gofs_seed[offsetof(struct globals2, evaluate__seed)]; - 0x90 */
#define t_double     (G1.t_double    )
#define beginseq     (G1.beginseq    )
#define mainseq      (G1.mainseq     )
#define endseq       (G1.endseq      )
#define seq          (G1.seq         )
#define break_ptr    (G1.break_ptr   )
#define continue_ptr (G1.continue_ptr)
#define iF           (G1.iF          )
#define vhash        (G1.vhash       )
#define ahash        (G1.ahash       )
#define fdhash       (G1.fdhash      )
#define fnhash       (G1.fnhash      )
#define g_progname   (G1.g_progname  )
#define g_lineno     (G1.g_lineno    )
#define nfields      (G1.nfields     )
#define maxfields    (G1.maxfields   )
#define Fields       (G1.Fields      )
#define g_cb         (G1.g_cb        )
#define g_pos        (G1.g_pos       )
#define g_buf        (G1.g_buf       )
#define icase        (G1.icase       )
#define exiting      (G1.exiting     )
#define nextrec      (G1.nextrec     )
#define nextfile     (G1.nextfile    )
#define is_f0_split  (G1.is_f0_split )
#define t_rollback   (G1.t_rollback  )
#define t_info       (G.t_info      )
#define t_tclass     (G.t_tclass    )
#define t_string     (G.t_string    )
#define t_lineno     (G.t_lineno    )
#define intvar       (G.intvar      )
#define fsplitter    (G.fsplitter   )
#define rsplitter    (G.rsplitter   )
// #define INIT_G() do { \
// 	SET_PTR_TO_GLOBALS((char*)xzalloc(sizeof(G1)+sizeof(G)) + sizeof(G1)); \
// 	G.next_token__ltclass = TC_OPTERM; \
// 	G.evaluate__seed = 1; \
// } while (0)
// translate INIT_G to a function
#define INIT_G() init_g()
void init_g(void);



// exposed from awk.c for trans
void zero_out_var(var *vp);
void syntax_error(const char *message);
unsigned hashidx(const char *name);
xhash *hash_init(void);
void *hash_search(xhash *hash, const char *name);
void hash_rebuild(xhash *hash);
void *hash_find(xhash *hash, const char *name);
void hash_remove(xhash *hash, const char *name);
char *skip_spaces(char *p);
char *nextword(char **s);
char nextchar(char **s);
void unescape_string_in_place(char *s1);
int isalnum_(int c);
double my_strtod(char **pp);
xhash *iamarray(var *v);
void clear_array(xhash *array);
var *clrvar(var *v);
var *setvar_p(var *v, char *value);
var *setvar_s(var *v, const char *value);
var *setvar_u(var *v, const char *value);
void setari_u(var *a, int idx, const char *s);
var *setvar_i(var *v, double value);
const char *getvar_s(var *v);
double getvar_i(var *v);
unsigned long getvar_i_int(var *v);
var *copyvar(var *dest, const var *src);
var *incvar(var *v);
int is_numeric(var *v);
int istrue(var *v);
var *nvalloc(int n);
void nvfree(var *v);
uint32_t next_token(uint32_t expected);
void rollback_token(void);
node *new_node(uint32_t info);
void mk_re_node(const char *s, node *n, regex_t *re);
node *condition(void);
node *parse_expr(uint32_t iexp);
node *chain_node(uint32_t info);
void chain_expr(uint32_t info);
node *chain_loop(node *nn);
void chain_group(void);
void parse_program(char *p);
node *mk_splitter(const char *s, tsplitter *spl);
regex_t *as_regex(node *op, regex_t *preg);
char* qrealloc(char *b, int n, int *size);
void fsrealloc(int size);
int regexec1_nonempty(const regex_t *preg, const char *s, regmatch_t pmatch[]);
int awk_split(const char *s, node *spl, char **slist);
void split_f0(void);
void handle_special(var *v);
node *nextarg(node **pn);
void hashwalk_init(var *v, xhash *array);
int hashwalk_next(var *v);
int ptest(node *pattern);
int awk_getline(rstream *rsm, var *v);
int fmt_num(char *b, int size, const char *format, double n, int int_as_int);
char *awk_printf(node *n);
int awk_sub(node *rn, const char *repl, int nm, var *src, var *dest, int subexp);
NOINLINE int do_mktime(const char *ds);
NOINLINE var *exec_builtin(node *op, var *res);
var *evaluate(node *op, var *res);
int awk_exit(int r);
int is_assignment(const char *expr);
rstream *next_input_file(void);
int awk_main(int argc, char **argv);

void print_intvar(void);
void print_var(struct var_s *v);
void print_rstream(struct rstream_s *rs);