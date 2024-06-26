/* vi: set sw=4 ts=4: */
/*
 * awk implementation for busybox
 *
 * Copyright (C) 2002 by Dmitry Zakharov <dmit@crp.bank.gov.ua>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */
//config:config AWK
//config:	bool "awk (23 kb)"
//config:	default y
//config:	help
//config:	Awk is used as a pattern scanning and processing language.
//config:
//config:config FEATURE_AWK_LIBM
//config:	bool "Enable math functions (requires libm)"
//config:	default y
//config:	depends on AWK
//config:	help
//config:	Enable math functions of the Awk programming language.
//config:	NOTE: This requires libm to be present for linking.
//config:
//config:config FEATURE_AWK_GNU_EXTENSIONS
//config:	bool "Enable a few GNU extensions"
//config:	default y
//config:	depends on AWK
//config:	help
//config:	Enable a few features from gawk:
//config:	* command line option -e AWK_PROGRAM
//config:	* simultaneous use of -f and -e on the command line.
//config:	This enables the use of awk library files.
//config:	Example: awk -f mylib.awk -e '{print myfunction($1);}' ...

//applet:IF_AWK(APPLET_NOEXEC(awk, awk, BB_DIR_USR_BIN, BB_SUID_DROP, awk))

//kbuild:lib-$(CONFIG_AWK) += awk.o

//usage:#define awk_trivial_usage
//usage:       "[OPTIONS] [AWK_PROGRAM] [FILE]..."
//usage:#define awk_full_usage "\n\n"
//usage:       "	-v VAR=VAL	Set variable"
//usage:     "\n	-F SEP		Use SEP as field separator"
//usage:     "\n	-f FILE		Read program from FILE"
//usage:	IF_FEATURE_AWK_GNU_EXTENSIONS(
//usage:     "\n	-e AWK_PROGRAM"
//usage:	)

// #include "libbb.h"
// #include "xregex.h"
#include "inc_stripped.h"
#include <math.h>
#include <stdio.h>
#include <unistd.h>

/* This is a NOEXEC applet. Be very careful! */

const char tokenlist[] ALIGN1 =
	"\1("         NTC                                   /* TC_SEQSTART */
	"\1)"         NTC                                   /* TC_SEQTERM */
	"\1/"         NTC                                   /* TC_REGEXP */
	"\2>>"        "\1>"         "\1|"       NTC         /* TC_OUTRDR */
	"\2++"        "\2--"        NTC                     /* TC_UOPPOST */
	"\2++"        "\2--"        "\1$"       NTC         /* TC_UOPPRE1 */
	"\2=="        "\1="         "\2+="      "\2-="      /* TC_BINOPX */
	"\2*="        "\2/="        "\2%="      "\2^="
	"\1+"         "\1-"         "\3**="     "\2**"
	"\1/"         "\1%"         "\1^"       "\1*"
	"\2!="        "\2>="        "\2<="      "\1>"
	"\1<"         "\2!~"        "\1~"       "\2&&"
	"\2||"        "\1?"         "\1:"       NTC
	"\2in"        NTC                                   /* TC_IN */
	"\1,"         NTC                                   /* TC_COMMA */
	"\1|"         NTC                                   /* TC_PIPE */
	"\1+"         "\1-"         "\1!"       NTC         /* TC_UOPPRE2 */
	"\1]"         NTC                                   /* TC_ARRTERM */
	"\1{"         NTC                                   /* TC_GRPSTART */
	"\1}"         NTC                                   /* TC_GRPTERM */
	"\1;"         NTC                                   /* TC_SEMICOL */
	"\1\n"        NTC                                   /* TC_NEWLINE */
	"\2if"        "\2do"        "\3for"     "\5break"   /* TC_STATX */
	"\10continue" "\6delete"    "\5print"
	"\6printf"    "\4next"      "\10nextfile"
	"\6return"    "\4exit"      NTC
	"\5while"     NTC                                   /* TC_WHILE */
	"\4else"      NTC                                   /* TC_ELSE */
	"\3and"       "\5compl"     "\6lshift"  "\2or"      /* TC_BUILTIN */
	"\6rshift"    "\3xor"
	"\5close"     "\6system"    "\6fflush"  "\5atan2"
	"\3cos"       "\3exp"       "\3int"     "\3log"
	"\4rand"      "\3sin"       "\4sqrt"    "\5srand"
	"\6gensub"    "\4gsub"      "\5index"	/* "\6length" was here */
	"\5match"     "\5split"     "\7sprintf" "\3sub"
	"\6substr"    "\7systime"   "\10strftime" "\6mktime"
	"\7tolower"   "\7toupper"   NTC
	"\6length"    NTC                                   /* TC_LENGTH */
	"\7getline"   NTC                                   /* TC_GETLINE */
	"\4func"      "\10function" NTC                     /* TC_FUNCDECL */
	"\5BEGIN"     NTC                                   /* TC_BEGIN */
	"\3END"                                             /* TC_END */
	/* compiler adds trailing "\0" */
	;

// #define OC_B  OC_BUILTIN

const uint32_t tokeninfo[] ALIGN4 = {
	0,
	0,
	OC_REGEXP,
	xS|'a',                  xS|'w',                  xS|'|',
	OC_UNARY|xV|P(9)|'p',    OC_UNARY|xV|P(9)|'m',
	OC_UNARY|xV|P(9)|'P',    OC_UNARY|xV|P(9)|'M',    OC_FIELD|xV|P(5),
	OC_COMPARE|VV|P(39)|5,   OC_MOVE|VV|P(74),        OC_REPLACE|NV|P(74)|'+', OC_REPLACE|NV|P(74)|'-',
	OC_REPLACE|NV|P(74)|'*', OC_REPLACE|NV|P(74)|'/', OC_REPLACE|NV|P(74)|'%', OC_REPLACE|NV|P(74)|'&',
	OC_BINARY|NV|P(29)|'+',  OC_BINARY|NV|P(29)|'-',  OC_REPLACE|NV|P(74)|'&', OC_BINARY|NV|P(15)|'&',
	OC_BINARY|NV|P(25)|'/',  OC_BINARY|NV|P(25)|'%',  OC_BINARY|NV|P(15)|'&',  OC_BINARY|NV|P(25)|'*',
	OC_COMPARE|VV|P(39)|4,   OC_COMPARE|VV|P(39)|3,   OC_COMPARE|VV|P(39)|0,   OC_COMPARE|VV|P(39)|1,
	OC_COMPARE|VV|P(39)|2,   OC_MATCH|Sx|P(45)|'!',   OC_MATCH|Sx|P(45)|'~',   OC_LAND|Vx|P(55),
	OC_LOR|Vx|P(59),         OC_TERNARY|Vx|P(64)|'?', OC_COLON|xx|P(67)|':',
	OC_IN|SV|P(49), /* TC_IN */
	OC_COMMA|SS|P(80),
	OC_PGETLINE|SV|P(37),
	OC_UNARY|xV|P(19)|'+',   OC_UNARY|xV|P(19)|'-',   OC_UNARY|xV|P(19)|'!',
	0, /* ] */
	0,
	0,
	0,
	0, /* \n */
	ST_IF,        ST_DO,        ST_FOR,      OC_BREAK,
	OC_CONTINUE,  OC_DELETE|Rx, OC_PRINT,
	OC_PRINTF,    OC_NEXT,      OC_NEXTFILE,
	OC_RETURN|Vx, OC_EXIT|Nx,
	ST_WHILE,
	0, /* else */
	OC_B|B_an|P(0x83), OC_B|B_co|P(0x41), OC_B|B_ls|P(0x83), OC_B|B_or|P(0x83),
	OC_B|B_rs|P(0x83), OC_B|B_xo|P(0x83),
	OC_FBLTIN|Sx|F_cl, OC_FBLTIN|Sx|F_sy, OC_FBLTIN|Sx|F_ff, OC_B|B_a2|P(0x83),
	OC_FBLTIN|Nx|F_co, OC_FBLTIN|Nx|F_ex, OC_FBLTIN|Nx|F_in, OC_FBLTIN|Nx|F_lg,
	OC_FBLTIN|F_rn,    OC_FBLTIN|Nx|F_si, OC_FBLTIN|Nx|F_sq, OC_FBLTIN|Nx|F_sr,
	OC_B|B_ge|P(0xd6), OC_B|B_gs|P(0xb6), OC_B|B_ix|P(0x9b), /* OC_FBLTIN|Sx|F_le, was here */
	OC_B|B_ma|P(0x89), OC_B|B_sp|P(0x8b), OC_SPRINTF,        OC_B|B_su|P(0xb6),
	OC_B|B_ss|P(0x8f), OC_FBLTIN|F_ti,    OC_B|B_ti|P(0x0b), OC_B|B_mt|P(0x0b),
	OC_B|B_lo|P(0x49), OC_B|B_up|P(0x49),
	OC_FBLTIN|Sx|F_le, /* TC_LENGTH */
	OC_GETLINE|SV|P(0),
	0,                 0,
	0,
	0 /* TC_END */
};

const char vNames[] ALIGN1 =
	"CONVFMT\0" "OFMT\0"    "FS\0*"     "OFS\0"
	"ORS\0"     "RS\0*"     "RT\0"      "FILENAME\0"
	"SUBSEP\0"  "$\0*"      "ARGIND\0"  "ARGC\0"
	"ARGV\0"    "ERRNO\0"   "FNR\0"     "NR\0"
	"NF\0*"     "IGNORECASE\0*" "ENVIRON\0" "\0";

const char vValues[] ALIGN1 =
	"%.6g\0"    "%.6g\0"    " \0"       " \0"
	"\n\0"      "\n\0"      "\0"        "\0"
	"\034\0"    "\0"        "\377";

// /* hash size may grow to these values */
// #define FIRST_PRIME 61
const uint16_t PRIMES[] ALIGN2 = { 251, 1021, 4093, 16381, 65521 };


/* ---- error handling ---- */

const char EMSG_INTERNAL_ERROR[] ALIGN1 = "Internal error";
const char EMSG_UNEXP_EOS[] ALIGN1 = "Unexpected end of string";
const char EMSG_UNEXP_TOKEN[] ALIGN1 = "Unexpected token";
const char EMSG_DIV_BY_ZERO[] ALIGN1 = "Division by zero";
const char EMSG_INV_FMT[] ALIGN1 = "Invalid format specifier";
const char EMSG_TOO_FEW_ARGS[] ALIGN1 = "Too few arguments";
const char EMSG_NOT_ARRAY[] ALIGN1 = "Not an array";
const char EMSG_POSSIBLE_ERROR[] ALIGN1 = "Possible syntax error";
const char EMSG_UNDEF_FUNC[] ALIGN1 = "Call to undefined function";
const char EMSG_NO_MATH[] ALIGN1 = "Math support is not compiled in";
const char EMSG_NEGATIVE_FIELD[] ALIGN1 = "Access to negative field";

void zero_out_var(var *vp)
{
	memset(vp, 0, sizeof(*vp));
}

// static void syntax_error(const char *message) NORETURN;
void syntax_error(const char *message)
{
	bb_error_msg_and_die("%s:%i: %s", g_progname, g_lineno, message);
}

/* ---- hash stuff ---- */

unsigned hashidx(const char *name)
{
	unsigned idx = 0;

	while (*name)
		idx = *name++ + (idx << 6) - idx;
	return idx;
}

/* create new hash */
xhash *hash_init(void)
{
	xhash *newhash;

	newhash = xzalloc(sizeof(*newhash));
	newhash->csize = FIRST_PRIME;
	newhash->items = xzalloc(FIRST_PRIME * sizeof(newhash->items[0]));

	return newhash;
}

/* find item in hash, return ptr to data, NULL if not found */
void *hash_search(xhash *hash, const char *name)
{
	hash_item *hi;

	hi = hash->items[hashidx(name) % hash->csize];
	while (hi) {
		if (strcmp(hi->name, name) == 0)
			return &hi->data;
		hi = hi->next;
	}
	return NULL;
}

/* grow hash if it becomes too big */
void hash_rebuild(xhash *hash)
{
	unsigned newsize, i, idx;
	hash_item **newitems, *hi, *thi;

	if (hash->nprime == ARRAY_SIZE(PRIMES))
		return;

	newsize = PRIMES[hash->nprime++];
	newitems = xzalloc(newsize * sizeof(newitems[0]));

	for (i = 0; i < hash->csize; i++) {
		hi = hash->items[i];
		while (hi) {
			thi = hi;
			hi = thi->next;
			idx = hashidx(thi->name) % newsize;
			thi->next = newitems[idx];
			newitems[idx] = thi;
		}
	}

	free(hash->items);
	hash->csize = newsize;
	hash->items = newitems;
}

/* find item in hash, add it if necessary. Return ptr to data */
void *hash_find(xhash *hash, const char *name)
{
	hash_item *hi;
	unsigned idx;
	int l;

	hi = hash_search(hash, name);
	if (!hi) {
		if (++hash->nel / hash->csize > 10)
			hash_rebuild(hash);

		l = strlen(name) + 1;
		hi = xzalloc(sizeof(*hi) + l);
		strcpy(hi->name, name);

		idx = hashidx(name) % hash->csize;
		hi->next = hash->items[idx];
		hash->items[idx] = hi;
		hash->glen += l;
	}
	return &hi->data;
}

#define findvar(hash, name) ((var*)    hash_find((hash), (name)))
#define newvar(name)        ((var*)    hash_find(vhash, (name)))
#define newfile(name)       ((rstream*)hash_find(fdhash, (name)))
#define newfunc(name)       ((func*)   hash_find(fnhash, (name)))

void hash_remove(xhash *hash, const char *name)
{
	hash_item *hi, **phi;

	phi = &hash->items[hashidx(name) % hash->csize];
	while (*phi) {
		hi = *phi;
		if (strcmp(hi->name, name) == 0) {
			hash->glen -= (strlen(name) + 1);
			hash->nel--;
			*phi = hi->next;
			free(hi);
			break;
		}
		phi = &hi->next;
	}
}

/* ------ some useful functions ------ */

char *skip_spaces(char *p)
{
	while (1) {
		if (*p == '\\' && p[1] == '\n') {
			p++;
			t_lineno++;
		} else if (*p != ' ' && *p != '\t') {
			break;
		}
		p++;
	}
	return p;
}

/* returns old *s, advances *s past word and terminating NUL */
char *nextword(char **s)
{
	char *p = *s;
	while (*(*s)++ != '\0')
		continue;
	return p;
}

char nextchar(char **s)
{
	char c, *pps;

	c = *(*s)++;
	pps = *s;
	if (c == '\\')
		c = bb_process_escape_sequence((const char**)s);
	/* Example awk statement:
	 * s = "abc\"def"
	 * we must treat \" as "
	 */
	if (c == '\\' && *s == pps) { /* unrecognized \z? */
		c = *(*s); /* yes, fetch z */
		if (c)
			(*s)++; /* advance unless z = NUL */
	}
	return c;
}

/* TODO: merge with strcpy_and_process_escape_sequences()?
 */
void unescape_string_in_place(char *s1)
{
	char *s = s1;
	while ((*s1 = nextchar(&s)) != '\0')
		s1++;
}

int isalnum_(int c)
{
	return (isalnum(c) || c == '_');
}

double my_strtod(char **pp)
{
	char *cp = *pp;
	if (ENABLE_DESKTOP && cp[0] == '0') {
		/* Might be hex or octal integer: 0x123abc or 07777 */
		char c = (cp[1] | 0x20);
		if (c == 'x' || isdigit(cp[1])) {
			unsigned long long ull = strtoull(cp, pp, 0);
			if (c == 'x')
				return ull;
			c = **pp;
			if (!isdigit(c) && c != '.')
				return ull;
			/* else: it may be a floating number. Examples:
			 * 009.123 (*pp points to '9')
			 * 000.123 (*pp points to '.')
			 * fall through to strtod.
			 */
		}
	}
	return strtod(cp, pp);
}

/* -------- working with variables (set/get/copy/etc) -------- */

xhash *iamarray(var *v)
{
	var *a = v;

	while (a->type & VF_CHILD)
		a = a->x.parent;

	if (!(a->type & VF_ARRAY)) {
		a->type |= VF_ARRAY;
		a->x.array = hash_init();
	}
	return a->x.array;
}

void clear_array(xhash *array)
{
	unsigned i;
	hash_item *hi, *thi;

	for (i = 0; i < array->csize; i++) {
		hi = array->items[i];
		while (hi) {
			thi = hi;
			hi = hi->next;
			free(thi->data.v.string);
			free(thi);
		}
		array->items[i] = NULL;
	}
	array->glen = array->nel = 0;
}

/* clear a variable */
var *clrvar(var *v)
{
	if (!(v->type & VF_FSTR))
		free(v->string);

	v->type &= VF_DONTTOUCH;
	v->type |= VF_DIRTY;
	v->string = NULL;
	return v;
}

/* assign string value to variable */
var *setvar_p(var *v, char *value)
{
	clrvar(v);
	v->string = value;
	handle_special(v);
	return v;
}

/* same as setvar_p but make a copy of string */
var *setvar_s(var *v, const char *value)
{
	return setvar_p(v, (value && *value) ? xstrdup(value) : NULL);
}

/* same as setvar_s but sets USER flag */
var *setvar_u(var *v, const char *value)
{
	v = setvar_s(v, value);
	// printf("setvar_u: %s\n", value);
	v->type |= VF_USER;
	return v;
}

/* set array element to user string */
void setari_u(var *a, int idx, const char *s)
{
	var *v;
	// printf("setari_u: %s\n", s);
	v = findvar(iamarray(a), itoa(idx));
	setvar_u(v, s);
}

/* assign numeric value to variable */
var *setvar_i(var *v, double value)
{
	clrvar(v);
	v->type |= VF_NUMBER;
	v->number = value;
	handle_special(v);
	return v;
}

const char *getvar_s(var *v)
{
	/* if v is numeric and has no cached string, convert it to string */
	if ((v->type & (VF_NUMBER | VF_CACHED)) == VF_NUMBER) {
		fmt_num(g_buf, MAXVARFMT, getvar_s(intvar[CONVFMT]), v->number, TRUE);
		v->string = xstrdup(g_buf);
		v->type |= VF_CACHED;
	}
	return (v->string == NULL) ? "" : v->string;
}

double getvar_i(var *v)
{
	char *s;

	if ((v->type & (VF_NUMBER | VF_CACHED)) == 0) {
		v->number = 0;
		s = v->string;
		if (s && *s) {
			debug_printf_eval("getvar_i: '%s'->", s);
			v->number = my_strtod(&s);
			debug_printf_eval("%f (s:'%s')\n", v->number, s);
			if (v->type & VF_USER) {
				s = skip_spaces(s);
				if (*s != '\0')
					v->type &= ~VF_USER;
			}
		} else {
			debug_printf_eval("getvar_i: '%s'->zero\n", s);
			v->type &= ~VF_USER;
		}
		v->type |= VF_CACHED;
	}
	debug_printf_eval("getvar_i: %f\n", v->number);
	return v->number;
}

/* Used for operands of bitwise ops */
unsigned long getvar_i_int(var *v)
{
	double d = getvar_i(v);

	/* Casting doubles to longs is undefined for values outside
	 * of target type range. Try to widen it as much as possible */
	if (d >= 0)
		return (unsigned long)d;
	/* Why? Think about d == -4294967295.0 (assuming 32bit longs) */
	return - (long) (unsigned long) (-d);
}

var *copyvar(var *dest, const var *src)
{
	if (dest != src) {
		clrvar(dest);
		dest->type |= (src->type & ~(VF_DONTTOUCH | VF_FSTR));
		debug_printf_eval("copyvar: number:%f string:'%s'\n", src->number, src->string);
		dest->number = src->number;
		if (src->string)
			dest->string = xstrdup(src->string);
	}
	handle_special(dest);
	return dest;
}

var *incvar(var *v)
{
	return setvar_i(v, getvar_i(v) + 1.0);
}

/* return true if v is number or numeric string */
int is_numeric(var *v)
{
	getvar_i(v);
	return ((v->type ^ VF_DIRTY) & (VF_NUMBER | VF_USER | VF_DIRTY));
}

/* return 1 when value of v corresponds to true, 0 otherwise */
int istrue(var *v)
{
	if (is_numeric(v))
		return (v->number != 0);
	return (v->string && v->string[0]);
}

/* temporary variables allocator. Last allocated should be first freed */
var *nvalloc(int n)
{
	nvblock *pb = NULL;
	var *v, *r;
	int size;

	while (g_cb) {
		pb = g_cb;
		if ((g_cb->pos - g_cb->nv) + n <= g_cb->size)
			break;
		g_cb = g_cb->next;
	}

	if (!g_cb) {
		size = (n <= MINNVBLOCK) ? MINNVBLOCK : n;
		g_cb = xzalloc(sizeof(nvblock) + size * sizeof(var));
		g_cb->size = size;
		g_cb->pos = g_cb->nv;
		g_cb->prev = pb;
		/*g_cb->next = NULL; - xzalloc did it */
		if (pb)
			pb->next = g_cb;
	}

	v = r = g_cb->pos;
	g_cb->pos += n;

	while (v < g_cb->pos) {
		v->type = 0;
		v->string = NULL;
		v++;
	}

	return r;
}

void nvfree(var *v)
{
	var *p;

	if (v < g_cb->nv || v >= g_cb->pos)
		syntax_error(EMSG_INTERNAL_ERROR);

	for (p = v; p < g_cb->pos; p++) {
		if ((p->type & (VF_ARRAY | VF_CHILD)) == VF_ARRAY) {
			clear_array(iamarray(p));
			free(p->x.array->items);
			free(p->x.array);
		}
		if (p->type & VF_WALK) {
			walker_list *n;
			walker_list *w = p->x.walker;
			debug_printf_walker("nvfree: freeing walker @%p\n", &p->x.walker);
			p->x.walker = NULL;
			while (w) {
				n = w->prev;
				debug_printf_walker(" free(%p)\n", w);
				free(w);
				w = n;
			}
		}
		clrvar(p);
	}

	g_cb->pos = v;
	while (g_cb->prev && g_cb->pos == g_cb->nv) {
		g_cb = g_cb->prev;
	}
}

/* ------- awk program text parsing ------- */

/* Parse next token pointed by global pos, place results into global ttt.
 * If token isn't expected, give away. Return token class
 */
uint32_t next_token(uint32_t expected)
{
#define concat_inserted (G.next_token__concat_inserted)
#define save_tclass     (G.next_token__save_tclass)
#define save_info       (G.next_token__save_info)
/* Initialized to TC_OPTERM: */
#define ltclass         (G.next_token__ltclass)

	char *p, *s;
	const char *tl;
	uint32_t tc;
	const uint32_t *ti;

	if (t_rollback) {
		debug_printf_parse("%s: using rolled-back token\n", __func__);
		t_rollback = FALSE;
	} else if (concat_inserted) {
		debug_printf_parse("%s: using concat-inserted token\n", __func__);
		concat_inserted = FALSE;
		t_tclass = save_tclass;
		t_info = save_info;
	} else {
		p = g_pos;
 readnext:
		p = skip_spaces(p);
		g_lineno = t_lineno;
		if (*p == '#')
			while (*p != '\n' && *p != '\0')
				p++;

		if (*p == '\n')
			t_lineno++;

		if (*p == '\0') {
			tc = TC_EOF;
			debug_printf_parse("%s: token found: TC_EOF\n", __func__);
		} else if (*p == '\"') {
			/* it's a string */
			t_string = s = ++p;
			while (*p != '\"') {
				char *pp;
				if (*p == '\0' || *p == '\n')
					syntax_error(EMSG_UNEXP_EOS);
				pp = p;
				*s++ = nextchar(&pp);
				p = pp;
			}
			p++;
			*s = '\0';
			tc = TC_STRING;
			debug_printf_parse("%s: token found:'%s' TC_STRING\n", __func__, t_string);
		} else if ((expected & TC_REGEXP) && *p == '/') {
			/* it's regexp */
			t_string = s = ++p;
			while (*p != '/') {
				if (*p == '\0' || *p == '\n')
					syntax_error(EMSG_UNEXP_EOS);
				*s = *p++;
				if (*s++ == '\\') {
					char *pp = p;
					s[-1] = bb_process_escape_sequence((const char **)&pp);
					if (*p == '\\')
						*s++ = '\\';
					if (pp == p)
						*s++ = *p++;
					else
						p = pp;
				}
			}
			p++;
			*s = '\0';
			tc = TC_REGEXP;
			debug_printf_parse("%s: token found:'%s' TC_REGEXP\n", __func__, t_string);

		} else if (*p == '.' || isdigit(*p)) {
			/* it's a number */
			char *pp = p;
			t_double = my_strtod(&pp);
			p = pp;
			if (*p == '.')
				syntax_error(EMSG_UNEXP_TOKEN);
			tc = TC_NUMBER;
			debug_printf_parse("%s: token found:%f TC_NUMBER\n", __func__, t_double);
		} else {
			/* search for something known */
			tl = tokenlist;
			tc = 0x00000001;
			ti = tokeninfo;
			while (*tl) {
				int l = (unsigned char) *tl++;
				if (l == (unsigned char) NTCC) {
					tc <<= 1;
					continue;
				}
				/* if token class is expected,
				 * token matches,
				 * and it's not a longer word,
				 */
				int cond1 = (tc & (expected | TC_WORD | TC_NEWLINE));
				int cond2 = strncmp(p, tl, l) == 0;
				int cond3 = !((tc & TC_WORD) && isalnum_(p[l]));
				int cond = cond1 && cond2 && cond3;
				if (cond
				) {
					/* then this is what we are looking for */
					t_info = *ti;
					debug_printf_parse("%s: token found:'%.*s' t_info:%x\n", __func__, l, p, t_info);
					p += l;
					goto token_found;
				}
				ti++;
				tl += l;
			}
			/* not a known token */

			/* is it a name? (var/array/function) */
			int token_found = isalnum_(*p);
			if (!token_found)
				syntax_error(EMSG_UNEXP_TOKEN); /* no */
			/* yes */
			t_string = --p;
			while (isalnum_(*++p)) {
				p[-1] = *p;
			}
			p[-1] = '\0';
			tc = TC_VARIABLE;
			/* also consume whitespace between functionname and bracket */
			if (!(expected & TC_VARIABLE) || (expected & TC_ARRAY))
				p = skip_spaces(p);
			if (*p == '(') {
				tc = TC_FUNCTION;
				debug_printf_parse("%s: token found:'%s' TC_FUNCTION\n", __func__, t_string);
			} else {
				if (*p == '[') {
					p++;
					tc = TC_ARRAY;
					debug_printf_parse("%s: token found:'%s' TC_ARRAY\n", __func__, t_string);
				} else
					debug_printf_parse("%s: token found:'%s' TC_VARIABLE\n", __func__, t_string);
			}
		}
 token_found:
		g_pos = p;

		/* skipping newlines in some cases */
		if ((ltclass & TC_NOTERM) && (tc & TC_NEWLINE))
			goto readnext;

		/* insert concatenation operator when needed */
		debug_printf_parse("%s: %x %x %x concat_inserted?\n", __func__,
			(ltclass & TC_CONCAT1), (tc & TC_CONCAT2), (expected & TC_BINOP));
		if ((ltclass & TC_CONCAT1) && (tc & TC_CONCAT2) && (expected & TC_BINOP)
		 && !(ltclass == TC_LENGTH && tc == TC_SEQSTART) /* but not for "length(..." */
		) {
			concat_inserted = TRUE;
			save_tclass = tc;
			save_info = t_info;
			tc = TC_BINOP;
			t_info = OC_CONCAT | SS | P(35);
		}

		debug_printf_parse("%s: t_tclass=tc=%x\n", __func__, t_tclass);
		t_tclass = tc;
	}
	ltclass = t_tclass;

	/* Are we ready for this? */
	if (!(ltclass & expected)) {
		syntax_error((ltclass & (TC_NEWLINE | TC_EOF)) ?
				EMSG_UNEXP_EOS : EMSG_UNEXP_TOKEN);
	}

	debug_printf_parse("%s: returning, ltclass:%x t_double:%f\n", __func__, ltclass, t_double);
	return ltclass;
#undef concat_inserted
#undef save_tclass
#undef save_info
#undef ltclass
}

void rollback_token(void)
{
	t_rollback = TRUE;
}

node *new_node(uint32_t info)
{
	node *n;

	n = xzalloc(sizeof(node));
	n->info = info;
	n->lineno = g_lineno;
	return n;
}

void mk_re_node(const char *s, node *n, regex_t *re)
{
	n->info = OC_REGEXP;
	n->l.re = re;
	n->r.ire = re + 1;
	xregcomp(re, s, REG_EXTENDED);
	xregcomp(re + 1, s, REG_EXTENDED | REG_ICASE);
}

node *condition(void)
{
	next_token(TC_SEQSTART);
	return parse_expr(TC_SEQTERM);
}

/* parse expression terminated by given argument, return ptr
 * to built subtree. Terminator is eaten by parse_expr */
node *parse_expr(uint32_t iexp)
{
	node sn;
	node *cn = &sn;
	node *vn, *glptr;
	uint32_t tc, xtc;
	var *v;

	// printf("parse_expr: %x\n", iexp);
	debug_printf_parse("%s(%x)\n", __func__, iexp);

	sn.info = PRIMASK;
	sn.r.n = sn.a.n = glptr = NULL;
	xtc = TC_OPERAND | TC_UOPPRE | TC_REGEXP | iexp;

	tc = next_token(xtc);
	while (!(tc & iexp)) {
		// printf("tc: %x\n", tc);

		if (glptr && (t_info == (OC_COMPARE | VV | P(39) | 2))) {
			/* input redirection (<) attached to glptr node */
			debug_printf_parse("%s: input redir\n", __func__);
			cn = glptr->l.n = new_node(OC_CONCAT | SS | P(37));
			// printf("gltpr->l.n addr: %p\n", glptr->l.n);
			cn->a.n = glptr;
			xtc = TC_OPERAND | TC_UOPPRE;
			glptr = NULL;

		} else if (tc & (TC_BINOP | TC_UOPPOST)) {
			debug_printf_parse("%s: TC_BINOP | TC_UOPPOST tc:%x\n", __func__, tc);
			/* for binary and postfix-unary operators, jump back over
			 * previous operators with higher priority */
			vn = cn;
			while (((t_info & PRIMASK) > (vn->a.n->info & PRIMASK2))
			    || ((t_info == vn->info) && ((t_info & OPCLSMASK) == OC_COLON))
			) {
				vn = vn->a.n;
				if (!vn->a.n) syntax_error(EMSG_UNEXP_TOKEN);
			}
			if ((t_info & OPCLSMASK) == OC_TERNARY)
				t_info += P(6);
			cn = vn->a.n->r.n = new_node(t_info);
			cn->a.n = vn->a.n;
			if (tc & TC_BINOP) {
				// printf("tc & TC_BINOP\n");
				cn->l.n = vn;
				xtc = TC_OPERAND | TC_UOPPRE | TC_REGEXP;
				if ((t_info & OPCLSMASK) == OC_PGETLINE) {
					/* it's a pipe */
					next_token(TC_GETLINE);
					/* give maximum priority to this pipe */
					cn->info &= ~PRIMASK;
					xtc = TC_OPERAND | TC_UOPPRE | TC_BINOP | iexp;
				}
			} else {
				cn->r.n = vn;
				xtc = TC_OPERAND | TC_UOPPRE | TC_BINOP | iexp;
			}
			vn->a.n = cn;

		} else {
			debug_printf_parse("%s: other\n", __func__);
			/* for operands and prefix-unary operators, attach them
			 * to last node */
			vn = cn;
			cn = vn->r.n = new_node(t_info);
			cn->a.n = vn;
			xtc = TC_OPERAND | TC_UOPPRE | TC_REGEXP;
			if (tc & (TC_OPERAND | TC_REGEXP)) {
				debug_printf_parse("%s: TC_OPERAND | TC_REGEXP\n", __func__);
				xtc = TC_UOPPRE | TC_UOPPOST | TC_BINOP | TC_OPERAND | iexp;
				/* one should be very careful with switch on tclass -
				 * only simple tclasses should be used! */
				switch (tc) {
				case TC_VARIABLE:
				case TC_ARRAY:
					debug_printf_parse("%s: TC_VARIABLE | TC_ARRAY\n", __func__);
					cn->info = OC_VAR;
					// printf("t_string: %s\n", t_string);
					v = hash_search(ahash, t_string);
					// printf("v: %p\n", v);
					if (v != NULL) {
						cn->info = OC_FNARG;
						cn->l.aidx = v->x.aidx;
					} else {
						cn->l.v = newvar(t_string);
						// printf("cn->l.v: %p\n", cn->l.v);
					}
					if (tc & TC_ARRAY) {
						cn->info |= xS;
						cn->r.n = parse_expr(TC_ARRTERM);
					}
					break;

				case TC_NUMBER:
				case TC_STRING:
					debug_printf_parse("%s: TC_NUMBER | TC_STRING\n", __func__);
					cn->info = OC_VAR;
					v = cn->l.v = xzalloc(sizeof(var));
					if (tc & TC_NUMBER)
						setvar_i(v, t_double);
					else {
						setvar_s(v, t_string);
						xtc &= ~TC_UOPPOST; /* "str"++ is not allowed */
					}
					break;

				case TC_REGEXP:
					debug_printf_parse("%s: TC_REGEXP\n", __func__);
					mk_re_node(t_string, cn, xzalloc(sizeof(regex_t)*2));
					break;

				case TC_FUNCTION:
					// printf("TC_FUNCTION\n");
					debug_printf_parse("%s: TC_FUNCTION\n", __func__);
					cn->info = OC_FUNC;
					cn->r.f = newfunc(t_string);
					cn->l.n = condition();
					break;

				case TC_SEQSTART:
					debug_printf_parse("%s: TC_SEQSTART\n", __func__);
					cn = vn->r.n = parse_expr(TC_SEQTERM);
					if (!cn)
						syntax_error("Empty sequence");
					cn->a.n = vn;
					break;

				case TC_GETLINE:
					debug_printf_parse("%s: TC_GETLINE\n", __func__);
					glptr = cn;
					xtc = TC_OPERAND | TC_UOPPRE | TC_BINOP | iexp;
					break;

				case TC_BUILTIN:
					debug_printf_parse("%s: TC_BUILTIN\n", __func__);
					cn->l.n = condition();
					break;

				case TC_LENGTH:
					debug_printf_parse("%s: TC_LENGTH\n", __func__);
					next_token(TC_SEQSTART /* length(...) */
						| TC_OPTERM    /* length; (or newline)*/
						| TC_GRPTERM   /* length } */
						| TC_BINOPX    /* length <op> NUM */
						| TC_COMMA     /* print length, 1 */
					);
					rollback_token();
					if (t_tclass & TC_SEQSTART) {
						/* It was a "(" token. Handle just like TC_BUILTIN */
						cn->l.n = condition();
					}
					break;
				}
			}
		}
		tc = next_token(xtc);
	}

	debug_printf_parse("%s() returns %p\n", __func__, sn.r.n);
	return sn.r.n;
}

/* add node to chain. Return ptr to alloc'd node */
node *chain_node(uint32_t info)
{
	node *n;

	if (!seq->first) {
		// printf("seq->first is NULL\n");
		seq->first = seq->last = new_node(0);
	}
	if (seq->programname != g_progname) {
		// printf("seq->programname != g_progname\n");
		seq->programname = g_progname;
		n = chain_node(OC_NEWSOURCE);
		n->l.new_progname = xstrdup(g_progname);
	}

	// printf("mainseq->first->info modded: %d\n", mainseq.first->info);
	n = seq->last;
	n->info = info;
	seq->last = n->a.n = new_node(OC_DONE);

	return n;
}

void chain_expr(uint32_t info)
{
	node *n;

	n = chain_node(info);

	n->l.n = parse_expr(TC_OPTERM | TC_GRPTERM);
	if ((info & OF_REQUIRED) && !n->l.n)
		syntax_error(EMSG_TOO_FEW_ARGS);

	if (t_tclass & TC_GRPTERM)
		rollback_token();
}

node *chain_loop(node *nn)
{
	node *n, *n2, *save_brk, *save_cont;

	save_brk = break_ptr;
	save_cont = continue_ptr;

	n = chain_node(OC_BR | Vx);
	continue_ptr = new_node(OC_EXEC);
	break_ptr = new_node(OC_EXEC);
	chain_group();
	n2 = chain_node(OC_EXEC | Vx);
	n2->l.n = nn;
	n2->a.n = n;
	continue_ptr->a.n = n2;
	break_ptr->a.n = n->r.n = seq->last;

	continue_ptr = save_cont;
	break_ptr = save_brk;

	return n;
}

/* parse group and attach it to chain */
void chain_group(void)
{
	uint32_t c;
	node *n, *n2, *n3;

	do {
		c = next_token(TC_GRPSEQ);
	} while (c & TC_NEWLINE);

	// printf("c: %d\n", c);

	if (c & TC_GRPSTART) {
		// printf("c & TC_GRPSTART\n");
		debug_printf_parse("%s: TC_GRPSTART\n", __func__);
		while (next_token(TC_GRPSEQ | TC_GRPTERM) != TC_GRPTERM) {
			// printf("%s: !TC_GRPTERM\n", __func__);
			debug_printf_parse("%s: !TC_GRPTERM\n", __func__);
			if (t_tclass & TC_NEWLINE)
				continue;
			rollback_token();
			// printf("chainc ald\n");
			chain_group();
		}
		// printf("done grp\n");
		// printf("mainseq->first->info aft: %d\n", mainseq.first->info);
		debug_printf_parse("%s: TC_GRPTERM\n", __func__);
	} else if (c & (TC_OPSEQ | TC_OPTERM)) {
		// printf("%s: TC_OPSEQ | TC_OPTERM\n", __func__);
		debug_printf_parse("%s: TC_OPSEQ | TC_OPTERM\n", __func__);
		rollback_token();
		chain_expr(OC_EXEC | Vx);
	} else {
		/* TC_STATEMNT */
		// printf("%s: TC_STATEMNT(?)\n", __func__);
		debug_printf_parse("%s: TC_STATEMNT(?)\n", __func__);
		switch (t_info & OPCLSMASK) {
		case ST_IF:
			debug_printf_parse("%s: ST_IF\n", __func__);
			n = chain_node(OC_BR | Vx);
			n->l.n = condition();
			chain_group();
			n2 = chain_node(OC_EXEC);
			n->r.n = seq->last;
			if (next_token(TC_GRPSEQ | TC_GRPTERM | TC_ELSE) == TC_ELSE) {
				chain_group();
				n2->a.n = seq->last;
			} else {
				rollback_token();
			}
			break;

		case ST_WHILE:
			// printf("%s: ST_WHILE\n", __func__);
			debug_printf_parse("%s: ST_WHILE\n", __func__);
			n2 = condition();
			n = chain_loop(NULL);
			n->l.n = n2;
			break;

		case ST_DO:
			// printf("%s: ST_DO\n", __func__);
			debug_printf_parse("%s: ST_DO\n", __func__);
			n2 = chain_node(OC_EXEC);
			n = chain_loop(NULL);
			n2->a.n = n->a.n;
			next_token(TC_WHILE);
			n->l.n = condition();
			break;

		case ST_FOR:
			// printf("%s: ST_FOR\n", __func__);
			debug_printf_parse("%s: ST_FOR\n", __func__);
			next_token(TC_SEQSTART);
			n2 = parse_expr(TC_SEMICOL | TC_SEQTERM);
			if (t_tclass & TC_SEQTERM) {	/* for-in */
				if (!n2 || (n2->info & OPCLSMASK) != OC_IN)
					syntax_error(EMSG_UNEXP_TOKEN);
				n = chain_node(OC_WALKINIT | VV);
				n->l.n = n2->l.n;
				n->r.n = n2->r.n;
				n = chain_loop(NULL);
				n->info = OC_WALKNEXT | Vx;
				n->l.n = n2->l.n;
			} else {			/* for (;;) */
				n = chain_node(OC_EXEC | Vx);
				n->l.n = n2;
				n2 = parse_expr(TC_SEMICOL);
				n3 = parse_expr(TC_SEQTERM);
				n = chain_loop(n3);
				n->l.n = n2;
				if (!n2)
					n->info = OC_EXEC;
			}
			break;

		case OC_PRINT:
		case OC_PRINTF:
			// printf("%s: OC_PRINT | OC_PRINTF\n", __func__);
			debug_printf_parse("%s: OC_PRINT[F]\n", __func__);
			// printf("n: %p\n", n);
			// printf("t_info: %d\n", t_info);
			n = chain_node(t_info);
			// printf("mainseq->first->info bef: %d\n", mainseq.first->info);
			// printf("n: %p\n", n);
			// printf("n->info: %d\n", n->info);
			// printf("intvar[NF] address: %p\n", intvar[NF]);
			n->l.n = parse_expr(TC_OPTERM | TC_OUTRDR | TC_GRPTERM);
			// printf("intvar[NF] address: %p\n", intvar[NF]);
			// printf("n->l.n: %p\n", n->l.n);
			if (t_tclass & TC_OUTRDR) {
				// printf("t_tclass & TC_OUTRDR\n");
				n->info |= t_info;
				n->r.n = parse_expr(TC_OPTERM | TC_GRPTERM);
			}
			if (t_tclass & TC_GRPTERM) {
				rollback_token();
				// printf("rollback_token\n");
				// printf("n->info: %d\n", n->info);
			}
			break;

		case OC_BREAK:
			debug_printf_parse("%s: OC_BREAK\n", __func__);
			n = chain_node(OC_EXEC);
			n->a.n = break_ptr;
			chain_expr(t_info);
			break;

		case OC_CONTINUE:
			debug_printf_parse("%s: OC_CONTINUE\n", __func__);
			n = chain_node(OC_EXEC);
			n->a.n = continue_ptr;
			chain_expr(t_info);
			break;

		/* delete, next, nextfile, return, exit */
		default:
			// printf("%s: default\n", __func__);
			debug_printf_parse("%s: default\n", __func__);
			chain_expr(t_info);
		}
		// printf("n->info: %d\n", n->info);
	}
}

void parse_program(char *p)
{
	uint32_t tclass;
	node *cn;
	func *f;
	var *v;
	// printf("p in parse_prog %s\n", p);
	g_pos = p;
	t_lineno = 1;
	while ((tclass = next_token(TC_EOF | TC_OPSEQ | TC_GRPSTART |
			TC_OPTERM | TC_BEGIN | TC_END | TC_FUNCDECL)) != TC_EOF) {

		if (tclass & TC_OPTERM) {
			// printf("%s: TC_OPTERM\n", __func__);
			debug_printf_parse("%s: TC_OPTERM\n", __func__);
			continue;
		}
		// printf("ahash memory address: %p\n", ahash);
  		// printf("vhash memory address: %p\n", vhash);
  		// printf("ptr_to_globals ahash address in C: %p\n", &ahash);
		// print ptr to globals ahash address
  		// printf("ptr_to_globals address in C: %p\n", ptr_to_globals);
  		// printf("mainseq address: %p\n", &mainseq);
		seq = &mainseq;
		if (tclass & TC_BEGIN) {
			// printf("%s: TC_BEGIN\n", __func__);
			debug_printf_parse("%s: TC_BEGIN\n", __func__);
			seq = &beginseq;
			// printf("seq->first: %p\n", seq->first);
			// printf("beginseq.first: %p\n", beginseq.first);
			chain_group();
			// printf("seq->first: %p\n", seq->first);
			// printf("beginseq.first: %p\n", beginseq.first);
		} else if (tclass & TC_END) {
			// printf("%s: TC_END\n", __func__);
			debug_printf_parse("%s: TC_END\n", __func__);
			seq = &endseq;
			chain_group();
		} else if (tclass & TC_FUNCDECL) {
			// printf("%s: TC_FUNCDECL\n", __func__);
			debug_printf_parse("%s: TC_FUNCDECL\n", __func__);
			next_token(TC_FUNCTION);
			g_pos++;
			f = newfunc(t_string);
			f->body.first = NULL;
			f->nargs = 0;
			/* Match func arg list: a comma sep list of >= 0 args, and a close paren */
			while (next_token(TC_VARIABLE | TC_SEQTERM | TC_COMMA)) {
				/* Either an empty arg list, or trailing comma from prev iter
				 * must be followed by an arg */
				if (f->nargs == 0 && t_tclass == TC_SEQTERM)
					break;

				/* TC_SEQSTART/TC_COMMA must be followed by TC_VARIABLE */
				if (t_tclass != TC_VARIABLE)
					syntax_error(EMSG_UNEXP_TOKEN);

				v = findvar(ahash, t_string);
				v->x.aidx = f->nargs++;

				/* Arg followed either by end of arg list or 1 comma */
				if (next_token(TC_COMMA | TC_SEQTERM) & TC_SEQTERM)
					break;
				if (t_tclass != TC_COMMA)
					syntax_error(EMSG_UNEXP_TOKEN);
			}
			seq = &f->body;
			chain_group();
			clear_array(ahash);
		} else if (tclass & TC_OPSEQ) {
			// printf("%s: TC_OPSEQ\n", __func__);
			debug_printf_parse("%s: TC_OPSEQ\n", __func__);
			rollback_token();
			cn = chain_node(OC_TEST);
			cn->l.n = parse_expr(TC_OPTERM | TC_EOF | TC_GRPSTART);
			if (t_tclass & TC_GRPSTART) {
				// printf("%s: TC_GRPSTART\n", __func__);
				debug_printf_parse("%s: TC_GRPSTART\n", __func__);
				rollback_token();
				chain_group();
			} else {
				// printf("%s: !TC_GRPSTART\n", __func__);
				debug_printf_parse("%s: !TC_GRPSTART\n", __func__);
				chain_node(OC_PRINT);
			}
			cn->r.n = mainseq.last;
		} else /* if (tclass & TC_GRPSTART) */ {
			// printf("%s: TC_GRPSTART(?)\n", __func__);
			debug_printf_parse("%s: TC_GRPSTART(?)\n", __func__);
			rollback_token();
			// printf("seq->first: %p\n", seq->first);
			chain_group();
		}
	}
	// printf("%s: TC_EOF\n", __func__);
	debug_printf_parse("%s: TC_EOF\n", __func__);
}


/* -------- program execution part -------- */

node *mk_splitter(const char *s, tsplitter *spl)
{
	regex_t *re, *ire;
	node *n;

	re = &spl->re[0];
	ire = &spl->re[1];
	n = &spl->n;
	if ((n->info & OPCLSMASK) == OC_REGEXP) {
		regfree(re);
		regfree(ire); // TODO: nuke ire, use re+1?
	}
	if (s[0] && s[1]) { /* strlen(s) > 1 */
		mk_re_node(s, n, re);
	} else {
		n->info = (uint32_t) s[0];
	}

	return n;
}

/* use node as a regular expression. Supplied with node ptr and regex_t
 * storage space. Return ptr to regex (if result points to preg, it should
 * be later regfree'd manually
 */
regex_t *as_regex(node *op, regex_t *preg)
{
	int cflags;
	var *v;
	const char *s;

	if ((op->info & OPCLSMASK) == OC_REGEXP) {
		return icase ? op->r.ire : op->l.re;
	}
	v = nvalloc(1);
	s = getvar_s(evaluate(op, v));

	cflags = icase ? REG_EXTENDED | REG_ICASE : REG_EXTENDED;
	/* Testcase where REG_EXTENDED fails (unpaired '{'):
	 * echo Hi | awk 'gsub("@(samp|code|file)\{","");'
	 * gawk 3.1.5 eats this. We revert to ~REG_EXTENDED
	 * (maybe gsub is not supposed to use REG_EXTENDED?).
	 */
	if (regcomp(preg, s, cflags)) {
		cflags &= ~REG_EXTENDED;
		xregcomp(preg, s, cflags);
	}
	nvfree(v);
	return preg;
}

/* gradually increasing buffer.
 * note that we reallocate even if n == old_size,
 * and thus there is at least one extra allocated byte.
 */
char* qrealloc(char *b, int n, int *size)
{
	if (!b || n >= *size) {
		*size = n + (n>>1) + 80;
		b = xrealloc(b, *size);
	}
	return b;
}

/* resize field storage space */
void fsrealloc(int size)
{
	int i;

	if (size >= maxfields) {
		i = maxfields;
		maxfields = size + 16;
		Fields = xrealloc(Fields, maxfields * sizeof(Fields[0]));
		for (; i < maxfields; i++) {
			Fields[i].type = VF_SPECIAL;
			Fields[i].string = NULL;
		}
	}
	/* if size < nfields, clear extra field variables */
	for (i = size; i < nfields; i++) {
		clrvar(Fields + i);
	}
	nfields = size;
}

int regexec1_nonempty(const regex_t *preg, const char *s, regmatch_t pmatch[])
{
	int r = regexec(preg, s, 1, pmatch, 0);
	if (r == 0 && pmatch[0].rm_eo == 0) {
		/* For example, happens when FS can match
		 * an empty string (awk -F ' *'). Logically,
		 * this should split into one-char fields.
		 * However, gawk 5.0.1 searches for first
		 * _non-empty_ separator string match:
		 */
		size_t ofs = 0;
		do {
			ofs++;
			if (!s[ofs])
				return REG_NOMATCH;
			regexec(preg, s + ofs, 1, pmatch, 0);
		} while (pmatch[0].rm_eo == 0);
		pmatch[0].rm_so += ofs;
		pmatch[0].rm_eo += ofs;
	}
	return r;
}

int awk_split(const char *s, node *spl, char **slist)
{
	int n;
	char c[4];
	char *s1;

	/* in worst case, each char would be a separate field */
	*slist = s1 = xzalloc(strlen(s) * 2 + 3);
	strcpy(s1, s);

	c[0] = c[1] = (char)spl->info;
	c[2] = c[3] = '\0';
	if (*getvar_s(intvar[RS]) == '\0')
		c[2] = '\n';

	n = 0;
	if ((spl->info & OPCLSMASK) == OC_REGEXP) {  /* regex split */
		if (!*s)
			return n; /* "": zero fields */
		n++; /* at least one field will be there */
		do {
			int l;
			regmatch_t pmatch[2]; // TODO: why [2]? [1] is enough...

			l = strcspn(s, c+2); /* len till next NUL or \n */
			if (regexec1_nonempty(icase ? spl->r.ire : spl->l.re, s, pmatch) == 0
			 && pmatch[0].rm_so <= l
			) {
				/* if (pmatch[0].rm_eo == 0) ... - impossible */
				l = pmatch[0].rm_so;
				n++; /* we saw yet another delimiter */
			} else {
				pmatch[0].rm_eo = l;
				if (s[l])
					pmatch[0].rm_eo++;
			}
			s1 = mempcpy(s1, s, l);
			*s1++ = '\0';
			s += pmatch[0].rm_eo;
		} while (*s);

		/* echo a-- | awk -F-- '{ print NF, length($NF), $NF }'
		 * should print "2 0 ":
		 */
		*s1 = '\0';

		return n;
	}
	if (c[0] == '\0') {  /* null split */
		while (*s) {
			*s1++ = *s++;
			*s1++ = '\0';
			n++;
		}
		return n;
	}
	if (c[0] != ' ') {  /* single-character split */
		if (icase) {
			c[0] = toupper(c[0]);
			c[1] = tolower(c[1]);
		}
		if (*s1)
			n++;
		while ((s1 = strpbrk(s1, c)) != NULL) {
			*s1++ = '\0';
			n++;
		}
		return n;
	}
	/* space split */
	while (*s) {
		s = skip_whitespace(s);
		if (!*s)
			break;
		n++;
		while (*s && !isspace(*s))
			*s1++ = *s++;
		*s1++ = '\0';
	}
	return n;
}

void split_f0(void)
{
/* static char *fstrings; */
#define fstrings (G.split_f0__fstrings)

	int i, n;
	char *s;

	// printf("split_f0 called\n");

	if (is_f0_split)
		return;

	is_f0_split = TRUE;
	// printf("getvar_s(intvar[F0]): %s\n", getvar_s(intvar[F0]));
	free(fstrings);
	fsrealloc(0);
	n = awk_split(getvar_s(intvar[F0]), &fsplitter.n, &fstrings);
	fsrealloc(n);
	s = fstrings;
	for (i = 0; i < n; i++) {
		Fields[i].string = nextword(&s);
		Fields[i].type |= (VF_FSTR | VF_USER | VF_DIRTY);
	}

	/* set NF manually to avoid side effects */
	clrvar(intvar[NF]);
	intvar[NF]->type = VF_NUMBER | VF_SPECIAL;
	intvar[NF]->number = nfields;
#undef fstrings
}

/* perform additional actions when some internal variables changed */
void handle_special(var *v)
{
	int n;
	char *b;
	const char *sep, *s;
	int sl, l, len, i, bsize;

	if (!(v->type & VF_SPECIAL)) {
		// printf("returning\n");
		return;
	}
	if (v == intvar[NF]) {
		// printf("v == NF\n");
		n = (int)getvar_i(v);
		if (n < 0)
			syntax_error("NF set to negative value");
		fsrealloc(n);

		/* recalculate $0 */
		sep = getvar_s(intvar[OFS]);
		sl = strlen(sep);
		b = NULL;
		len = 0;
		for (i = 0; i < n; i++) {
			s = getvar_s(&Fields[i]);
			l = strlen(s);
			if (b) {
				memcpy(b+len, sep, sl);
				len += sl;
			}
			b = qrealloc(b, len+l+sl, &bsize);
			memcpy(b+len, s, l);
			len += l;
		}
		if (b)
			b[len] = '\0';
		setvar_p(intvar[F0], b);
		is_f0_split = TRUE;

	} else if (v == intvar[F0]) {
		// printf("v == F0\n");
		is_f0_split = FALSE;

	} else if (v == intvar[FS]) {
		/*
		 * The POSIX-2008 standard says that changing FS should have no effect on the
		 * current input line, but only on the next one. The language is:
		 *
		 * > Before the first reference to a field in the record is evaluated, the record
		 * > shall be split into fields, according to the rules in Regular Expressions,
		 * > using the value of FS that was current at the time the record was read.
		 *
		 * So, split up current line before assignment to FS:
		 */
		//  printf("v == FS\n");
		split_f0();

		mk_splitter(getvar_s(v), &fsplitter);
	} else if (v == intvar[RS]) {
		// printf("v == RS\n");
		mk_splitter(getvar_s(v), &rsplitter);
	} else if (v == intvar[IGNORECASE]) {
		// printf("v == IGNORECASE\n");
		icase = istrue(v);
	} else {				/* $n */
		// printf("else.....\n");
		n = getvar_i(intvar[NF]);
		setvar_i(intvar[NF], n > v-Fields ? n : v-Fields+1);
		/* right here v is invalid. Just to note... */
	}
}

/* step through func/builtin/etc arguments */
node *nextarg(node **pn)
{
	node *n;

	// printf("current node: %p\n", *pn);

	n = *pn;
	if (n && (n->info & OPCLSMASK) == OC_COMMA) {
		*pn = n->r.n;
		n = n->l.n;
	} else {
		*pn = NULL;
	}
	// printf("next node: %p\n", *pn);
	return n;
}

void hashwalk_init(var *v, xhash *array)
{
	hash_item *hi;
	unsigned i;
	walker_list *w;
	walker_list *prev_walker;

	if (v->type & VF_WALK) {
		prev_walker = v->x.walker;
	} else {
		v->type |= VF_WALK;
		prev_walker = NULL;
	}
	debug_printf_walker("hashwalk_init: prev_walker:%p\n", prev_walker);

	w = v->x.walker = xzalloc(sizeof(*w) + array->glen + 1); /* why + 1? */
	debug_printf_walker(" walker@%p=%p\n", &v->x.walker, w);
	w->cur = w->end = w->wbuf;
	w->prev = prev_walker;
	for (i = 0; i < array->csize; i++) {
		hi = array->items[i];
		while (hi) {
			strcpy(w->end, hi->name);
			nextword(&w->end);
			hi = hi->next;
		}
	}
}

int hashwalk_next(var *v)
{
	walker_list *w = v->x.walker;

	if (w->cur >= w->end) {
		walker_list *prev_walker = w->prev;

		debug_printf_walker("end of iteration, free(walker@%p:%p), prev_walker:%p\n", &v->x.walker, w, prev_walker);
		free(w);
		v->x.walker = prev_walker;
		return FALSE;
	}

	setvar_s(v, nextword(&w->cur));
	return TRUE;
}

/* evaluate node, return 1 when result is true, 0 otherwise */
int ptest(node *pattern)
{
	/* ptest__v is "static": to save stack space? */
	return istrue(evaluate(pattern, &G.ptest__v));
}

/* read next record from stream rsm into a variable v */
int awk_getline(rstream *rsm, var *v)
{
	char *b;
	regmatch_t pmatch[2]; // TODO: why [2]? [1] is enough...
	int size, a, p, pp = 0;
	int fd, so, eo, r, rp;
	char c, *m, *s;

	debug_printf_eval("entered %s()\n", __func__);

	/* we're using our own buffer since we need access to accumulating
	 * characters
	 */
	fd = fileno(rsm->F);
	m = rsm->buffer;
	a = rsm->adv;
	p = rsm->pos;
	size = rsm->size;
	c = (char) rsplitter.n.info;
	rp = 0;

	if (!m)
		m = qrealloc(m, 256, &size);

	do {
		b = m + a;
		so = eo = p;
		r = 1;
		if (p > 0) {
			if ((rsplitter.n.info & OPCLSMASK) == OC_REGEXP) {
				if (regexec(icase ? rsplitter.n.r.ire : rsplitter.n.l.re,
							b, 1, pmatch, 0) == 0) {
					so = pmatch[0].rm_so;
					eo = pmatch[0].rm_eo;
					if (b[eo] != '\0')
						break;
				}
			} else if (c != '\0') {
				s = strchr(b+pp, c);
				if (!s)
					s = memchr(b+pp, '\0', p - pp);
				if (s) {
					so = eo = s-b;
					eo++;
					break;
				}
			} else {
				while (b[rp] == '\n')
					rp++;
				s = strstr(b+rp, "\n\n");
				if (s) {
					so = eo = s-b;
					while (b[eo] == '\n')
						eo++;
					if (b[eo] != '\0')
						break;
				}
			}
		}

		if (a > 0) {
			memmove(m, m+a, p+1);
			b = m;
			a = 0;
		}

		m = qrealloc(m, a+p+128, &size);
		b = m + a;
		pp = p;
		p += safe_read(fd, b+p, size-p-1);
		if (p < pp) {
			p = 0;
			r = 0;
			setvar_i(intvar[ERRNO], errno);
		}
		b[p] = '\0';

	} while (p > pp);

	if (p == 0) {
		r--;
	} else {
		c = b[so]; b[so] = '\0';
		setvar_s(v, b+rp);
		v->type |= VF_USER;
		b[so] = c;
		c = b[eo]; b[eo] = '\0';
		setvar_s(intvar[RT], b+so);
		b[eo] = c;
	}

	rsm->buffer = m;
	rsm->adv = a + eo;
	rsm->pos = p - eo;
	rsm->size = size;

	debug_printf_eval("returning from %s(): %d\n", __func__, r);

	return r;
}

int fmt_num(char *b, int size, const char *format, double n, int int_as_int)
{
	int r = 0;
	char c;
	const char *s = format;

	if (int_as_int && n == (long long)n) {
		r = snprintf(b, size, "%lld", (long long)n);
	} else {
		do { c = *s; } while (c && *++s);
		if (strchr("diouxX", c)) {
			r = snprintf(b, size, format, (int)n);
		} else if (strchr("eEfgG", c)) {
			r = snprintf(b, size, format, n);
		} else {
			syntax_error(EMSG_INV_FMT);
		}
	}
	return r;
}

/* formatted output into an allocated buffer, return ptr to buffer */
char *awk_printf(node *n)
{
	char *b = NULL;
	char *fmt, *s, *f;
	const char *s1;
	int i, j, incr, bsize;
	char c, c1;
	var *v, *arg;

	v = nvalloc(1);
	fmt = f = xstrdup(getvar_s(evaluate(nextarg(&n), v)));

	i = 0;
	while (*f) {
		s = f;
		while (*f && (*f != '%' || *++f == '%'))
			f++;
		while (*f && !isalpha(*f)) {
			if (*f == '*')
				syntax_error("%*x formats are not supported");
			f++;
		}

		incr = (f - s) + MAXVARFMT;
		b = qrealloc(b, incr + i, &bsize);
		c = *f;
		if (c != '\0')
			f++;
		c1 = *f;
		*f = '\0';
		arg = evaluate(nextarg(&n), v);

		j = i;
		if (c == 'c' || !c) {
			i += sprintf(b+i, s, is_numeric(arg) ?
					(char)getvar_i(arg) : *getvar_s(arg));
		} else if (c == 's') {
			s1 = getvar_s(arg);
			b = qrealloc(b, incr+i+strlen(s1), &bsize);
			i += sprintf(b+i, s, s1);
		} else {
			i += fmt_num(b+i, incr, s, getvar_i(arg), FALSE);
		}
		*f = c1;

		/* if there was an error while sprintf, return value is negative */
		if (i < j)
			i = j;
	}

	free(fmt);
	nvfree(v);
	b = xrealloc(b, i + 1);
	b[i] = '\0';
	return b;
}

/* Common substitution routine.
 * Replace (nm)'th substring of (src) that matches (rn) with (repl),
 * store result into (dest), return number of substitutions.
 * If nm = 0, replace all matches.
 * If src or dst is NULL, use $0.
 * If subexp != 0, enable subexpression matching (\1-\9).
 */
int awk_sub(node *rn, const char *repl, int nm, var *src, var *dest, int subexp)
{
	char *resbuf;
	const char *sp;
	int match_no, residx, replen, resbufsize;
	int regexec_flags;
	regmatch_t pmatch[10];
	regex_t sreg, *regex;

	resbuf = NULL;
	residx = 0;
	match_no = 0;
	regexec_flags = 0;
	regex = as_regex(rn, &sreg);
	sp = getvar_s(src ? src : intvar[F0]);
	replen = strlen(repl);
	while (regexec(regex, sp, 10, pmatch, regexec_flags) == 0) {
		int so = pmatch[0].rm_so;
		int eo = pmatch[0].rm_eo;

		//bb_error_msg("match %u: [%u,%u] '%s'%p", match_no+1, so, eo, sp,sp);
		resbuf = qrealloc(resbuf, residx + eo + replen, &resbufsize);
		memcpy(resbuf + residx, sp, eo);
		residx += eo;
		if (++match_no >= nm) {
			const char *s;
			int nbs;

			/* replace */
			residx -= (eo - so);
			nbs = 0;
			for (s = repl; *s; s++) {
				char c = resbuf[residx++] = *s;
				if (c == '\\') {
					nbs++;
					continue;
				}
				if (c == '&' || (subexp && c >= '0' && c <= '9')) {
					int j;
					residx -= ((nbs + 3) >> 1);
					j = 0;
					if (c != '&') {
						j = c - '0';
						nbs++;
					}
					if (nbs % 2) {
						resbuf[residx++] = c;
					} else {
						int n = pmatch[j].rm_eo - pmatch[j].rm_so;
						resbuf = qrealloc(resbuf, residx + replen + n, &resbufsize);
						memcpy(resbuf + residx, sp + pmatch[j].rm_so, n);
						residx += n;
					}
				}
				nbs = 0;
			}
		}

		regexec_flags = REG_NOTBOL;
		sp += eo;
		if (match_no == nm)
			break;
		if (eo == so) {
			/* Empty match (e.g. "b*" will match anywhere).
			 * Advance by one char. */
//BUG (bug 1333):
//gsub(/\<b*/,"") on "abc" will reach this point, advance to "bc"
//... and will erroneously match "b" even though it is NOT at the word start.
//we need REG_NOTBOW but it does not exist...
//TODO: if EXTRA_COMPAT=y, use GNU matching and re_search,
//it should be able to do it correctly.
			/* Subtle: this is safe only because
			 * qrealloc allocated at least one extra byte */
			resbuf[residx] = *sp;
			if (*sp == '\0')
				goto ret;
			sp++;
			residx++;
		}
	}

	resbuf = qrealloc(resbuf, residx + strlen(sp), &resbufsize);
	strcpy(resbuf + residx, sp);
 ret:
	//bb_error_msg("end sp:'%s'%p", sp,sp);
	setvar_p(dest ? dest : intvar[F0], resbuf);
	if (regex == &sreg)
		regfree(regex);
	return match_no;
}

NOINLINE int do_mktime(const char *ds)
{
	struct tm then;
	int count;

	/*memset(&then, 0, sizeof(then)); - not needed */
	then.tm_isdst = -1; /* default is unknown */

	/* manpage of mktime says these fields are ints,
	 * so we can sscanf stuff directly into them */
	count = sscanf(ds, "%u %u %u %u %u %u %d",
		&then.tm_year, &then.tm_mon, &then.tm_mday,
		&then.tm_hour, &then.tm_min, &then.tm_sec,
		&then.tm_isdst);

	if (count < 6
	 || (unsigned)then.tm_mon < 1
	 || (unsigned)then.tm_year < 1900
	) {
		return -1;
	}

	then.tm_mon -= 1;
	then.tm_year -= 1900;

	return mktime(&then);
}

NOINLINE var *exec_builtin(node *op, var *res)
{
#define tspl (G.exec_builtin__tspl)

	var *tv;
	node *an[4];
	var *av[4];
	const char *as[4];
	regmatch_t pmatch[2];
	regex_t sreg, *re;
	node *spl;
	uint32_t isr, info;
	int nargs;
	time_t tt;
	int i, l, ll, n;

	tv = nvalloc(4);
	isr = info = op->info;
	op = op->l.n;

	av[2] = av[3] = NULL;
	for (i = 0; i < 4 && op; i++) {
		an[i] = nextarg(&op);
		if (isr & 0x09000000)
			av[i] = evaluate(an[i], &tv[i]);
		if (isr & 0x08000000)
			as[i] = getvar_s(av[i]);
		isr >>= 1;
	}

	nargs = i;
	if ((uint32_t)nargs < (info >> 30))
		syntax_error(EMSG_TOO_FEW_ARGS);

	info &= OPNMASK;
	switch (info) {

	case B_a2:
		if (ENABLE_FEATURE_AWK_LIBM)
			setvar_i(res, atan2(getvar_i(av[0]), getvar_i(av[1])));
		else
			syntax_error(EMSG_NO_MATH);
		break;

	case B_sp: {
		char *s, *s1;

		if (nargs > 2) {
			spl = (an[2]->info & OPCLSMASK) == OC_REGEXP ?
				an[2] : mk_splitter(getvar_s(evaluate(an[2], &tv[2])), &tspl);
		} else {
			spl = &fsplitter.n;
		}

		n = awk_split(as[0], spl, &s);
		s1 = s;
		clear_array(iamarray(av[1]));
		for (i = 1; i <= n; i++)
			setari_u(av[1], i, nextword(&s));
		free(s1);
		setvar_i(res, n);
		break;
	}

	case B_ss: {
		char *s;

		l = strlen(as[0]);
		i = getvar_i(av[1]) - 1;
		if (i > l)
			i = l;
		if (i < 0)
			i = 0;
		n = (nargs > 2) ? getvar_i(av[2]) : l-i;
		if (n < 0)
			n = 0;
		s = xstrndup(as[0]+i, n);
		setvar_p(res, s);
		break;
	}

	/* Bitwise ops must assume that operands are unsigned. GNU Awk 3.1.5:
	 * awk '{ print or(-1,1) }' gives "4.29497e+09", not "-2.xxxe+09" */
	case B_an:
		setvar_i(res, getvar_i_int(av[0]) & getvar_i_int(av[1]));
		break;

	case B_co:
		setvar_i(res, ~getvar_i_int(av[0]));
		break;

	case B_ls:
		setvar_i(res, getvar_i_int(av[0]) << getvar_i_int(av[1]));
		break;

	case B_or:
		setvar_i(res, getvar_i_int(av[0]) | getvar_i_int(av[1]));
		break;

	case B_rs:
		setvar_i(res, getvar_i_int(av[0]) >> getvar_i_int(av[1]));
		break;

	case B_xo:
		setvar_i(res, getvar_i_int(av[0]) ^ getvar_i_int(av[1]));
		break;

	case B_lo:
	case B_up: {
		char *s, *s1;
		s1 = s = xstrdup(as[0]);
		while (*s1) {
			//*s1 = (info == B_up) ? toupper(*s1) : tolower(*s1);
			if ((unsigned char)((*s1 | 0x20) - 'a') <= ('z' - 'a'))
				*s1 = (info == B_up) ? (*s1 & 0xdf) : (*s1 | 0x20);
			s1++;
		}
		setvar_p(res, s);
		break;
	}

	case B_ix:
		n = 0;
		ll = strlen(as[1]);
		l = strlen(as[0]) - ll;
		if (ll > 0 && l >= 0) {
			if (!icase) {
				char *s = strstr(as[0], as[1]);
				if (s)
					n = (s - as[0]) + 1;
			} else {
				/* this piece of code is terribly slow and
				 * really should be rewritten
				 */
				for (i = 0; i <= l; i++) {
					if (strncasecmp(as[0]+i, as[1], ll) == 0) {
						n = i+1;
						break;
					}
				}
			}
		}
		setvar_i(res, n);
		break;

	case B_ti:
		if (nargs > 1)
			tt = getvar_i(av[1]);
		else
			time(&tt);
		//s = (nargs > 0) ? as[0] : "%a %b %d %H:%M:%S %Z %Y";
		i = strftime(g_buf, MAXVARFMT,
			((nargs > 0) ? as[0] : "%a %b %d %H:%M:%S %Z %Y"),
			localtime(&tt));
		g_buf[i] = '\0';
		setvar_s(res, g_buf);
		break;

	case B_mt:
		setvar_i(res, do_mktime(as[0]));
		break;

	case B_ma:
		re = as_regex(an[1], &sreg);
		n = regexec(re, as[0], 1, pmatch, 0);
		if (n == 0) {
			pmatch[0].rm_so++;
			pmatch[0].rm_eo++;
		} else {
			pmatch[0].rm_so = 0;
			pmatch[0].rm_eo = -1;
		}
		setvar_i(newvar("RSTART"), pmatch[0].rm_so);
		setvar_i(newvar("RLENGTH"), pmatch[0].rm_eo - pmatch[0].rm_so);
		setvar_i(res, pmatch[0].rm_so);
		if (re == &sreg)
			regfree(re);
		break;

	case B_ge:
		awk_sub(an[0], as[1], getvar_i(av[2]), av[3], res, TRUE);
		break;

	case B_gs:
		setvar_i(res, awk_sub(an[0], as[1], 0, av[2], av[2], FALSE));
		break;

	case B_su:
		setvar_i(res, awk_sub(an[0], as[1], 1, av[2], av[2], FALSE));
		break;
	}

	nvfree(tv);
	return res;
#undef tspl
}

/*
 * Evaluate node - the heart of the program. Supplied with subtree
 * and place where to store result. returns ptr to result.
 */
#define XC(n) ((n) >> 8)

var *evaluate(node *op, var *res)
{
/* This procedure is recursive so we should count every byte */
#define fnargs (G.evaluate__fnargs)
/* seed is initialized to 1 */
#define seed   (G.evaluate__seed)
#define sreg   (G.evaluate__sreg)

	var *v1;

	//printf("op = %p\n", op);
	//printf("res = %p\n", res);

	if (!op) {
		//printf("op is NULL\n");
		var *re = setvar_s(res, NULL);
		//printf("re = %p\n", re);
		return setvar_s(res, NULL);
	}

	// for (int i = 0; i < 19; i++) {
	// 	printf("intvar[%d] as a string: %s\n", i, getvar_s(intvar[i]));
	// }


	debug_printf_eval("entered %s()\n", __func__);
	// printf("entered %s()\n", __func__);

	v1 = nvalloc(2);

	while (op) {
		struct {
			var *v;
			const char *s;
		} L = L; /* for compiler */
		struct {
			var *v;
			const char *s;
		} R = R;
		double L_d = L_d;
		uint32_t opinfo;
		int opn;
		node *op1;

		opinfo = op->info;
		opn = (opinfo & OPNMASK);
		g_lineno = op->lineno;
		//printf("l.n: %p\n", op->l.n);
		op1 = op->l.n;
		//printf("op1: %p\n", op1);
		debug_printf_eval("opinfo:%08x opn:%08x\n", opinfo, opn);
		//printf("opinfo:%08x opn:%08x\n", opinfo, opn);


		/* "delete" is special:
		 * "delete array[var--]" must evaluate index expr only once,
		 * must not evaluate it in "execute inevitable things" part.
		 */
		if (XC(opinfo & OPCLSMASK) == XC(OC_DELETE)) {
			//printf("delete\n");
			uint32_t info = op1->info & OPCLSMASK;
			var *v;

			debug_printf_eval("DELETE\n");
			//printf("DELETE\n");
			if (info == OC_VAR) {
				v = op1->l.v;
			} else if (info == OC_FNARG) {
				v = &fnargs[op1->l.aidx];
			} else {
				syntax_error(EMSG_NOT_ARRAY);
			}
			if (op1->r.n) { /* array ref? */
				const char *s;
				//printf("array ref\n");
				s = getvar_s(evaluate(op1->r.n, v1));
				hash_remove(iamarray(v), s);
			} else {
				clear_array(iamarray(v));
			}
			goto next;
		}

		/* execute inevitable things */
		if (opinfo & OF_RES1) {
			L.v = evaluate(op1, v1);
			//printf("L.v = %p\n", L.v);
		}
		if (opinfo & OF_RES2) {
			R.v = evaluate(op->r.n, v1+1);
			//printf("R.v = %p\n", R.v);
		}
		if (opinfo & OF_STR1) {
			L.s = getvar_s(L.v);
			debug_printf_eval("L.s:'%s'\n", L.s);
			//printf("L.s:'%s'\n", L.s);
		}
		if (opinfo & OF_STR2) {
			R.s = getvar_s(R.v);
			debug_printf_eval("R.s:'%s'\n", R.s);
			//printf("R.s:'%s'\n", R.s);
		}
		if (opinfo & OF_NUM1) {
			L_d = getvar_i(L.v);
			debug_printf_eval("L_d:%f\n", L_d);
			//printf("L_d:%f\n", L_d);
		}

		//printf("intvar[NF] address: %p\n", intvar[NF]);


		debug_printf_eval("switch(0x%x)\n", XC(opinfo & OPCLSMASK));
		//printf("switch(0x%x)\n", XC(opinfo & OPCLSMASK));
		switch (XC(opinfo & OPCLSMASK)) {

		/* -- iterative node type -- */

		/* test pattern */
		case XC( OC_TEST ):
			//printf("OC_TEST\n");
			if ((op1->info & OPCLSMASK) == OC_COMMA) {
				/* it's range pattern */
				if ((opinfo & OF_CHECKED) || ptest(op1->l.n)) {
					op->info |= OF_CHECKED;
					if (ptest(op1->r.n))
						op->info &= ~OF_CHECKED;
					op = op->a.n;
				} else {
					op = op->r.n;
				}
			} else {
				op = ptest(op1) ? op->a.n : op->r.n;
			}
			break;

		/* just evaluate an expression, also used as unconditional jump */
		case XC( OC_EXEC ):
			//printf("OC_EXEC\n");
			break;

		/* branch, used in if-else and various loops */
		case XC( OC_BR ):
			//printf("OC_BR\n");
			op = istrue(L.v) ? op->a.n : op->r.n;
			break;

		/* initialize for-in loop */
		case XC( OC_WALKINIT ):{
			//printf("OC_WALKINIT\n");
			xhash* tmp = iamarray(R.v); hashwalk_init(L.v, tmp);
			break;}

		/* get next array item */
		case XC( OC_WALKNEXT ):
			//printf("OC_WALKNEXT\n");
			op = hashwalk_next(L.v) ? op->a.n : op->r.n;
			break;

		case XC( OC_PRINT ):
		case XC( OC_PRINTF ): {
			//printf("OC_PRINT|//printf\n");
			FILE *F = stdout;

			if (op->r.n) {
				//printf("op->r.n is not NULL\n");
				rstream *rsm = newfile(R.s);
				if (!rsm->F) {
					if (opn == '|') {
						rsm->F = popen(R.s, "w");
						if (rsm->F == NULL)
							bb_simple_perror_msg_and_die("popen");
						rsm->is_pipe = 1;
					} else {
						rsm->F = xfopen(R.s, opn=='w' ? "w" : "a");
					}
				}
				F = rsm->F;
			}

			if ((opinfo & OPCLSMASK) == OC_PRINT) {
				//printf("opinfo & OPCLSMASK == OC_PRINT\n");
				if (!op1) {
					//printf("op1 is NULL\n");
					//printf("%s", getvar_s(intvar[F0]));
					fputs(getvar_s(intvar[F0]), F);
				} else {
					while (op1) {
						//printf("op1 is not NULL\n");
						//printf("op1->info = %d\n", op1->info);
						//printf("&op1 = %p\n", op1);
						var *v = evaluate(nextarg(&op1), v1);
						//printf("hi %s\n", getvar_s(v));
						//printf("op1 info = %d\n", op1->info);
						if (v->type & VF_NUMBER) {
							//printf("intvar[OFMT] = %s\n", getvar_s(intvar[OFMT]));
							fmt_num(g_buf, MAXVARFMT, getvar_s(intvar[OFMT]),
									getvar_i(v), TRUE);
							fputs(g_buf, F);
						} else {
							fputs(getvar_s(v), F);
						}

						if (op1)
							fputs(getvar_s(intvar[OFS]), F);
					}
				}
				fputs(getvar_s(intvar[ORS]), F);

			} else {	/* OC_PRINTF */
				char *s = awk_printf(op1);
				fputs(s, F);
				free(s);
			}
			fflush(F);
			break;
		}

		/* case XC( OC_DELETE ): - moved to happen before arg evaluation */

		case XC( OC_NEWSOURCE ):
			//printf("OC_NEWSOURCE\n");
			g_progname = op->l.new_progname;
			break;

		case XC( OC_RETURN ):
			//printf("OC_RETURN\n");
			copyvar(res, L.v);
			break;

		case XC( OC_NEXTFILE ):
			//printf("OC_NEXTFILE\n");
			nextfile = TRUE;
		case XC( OC_NEXT ):
			//printf("OC_NEXT\n");
			nextrec = TRUE;
		case XC( OC_DONE ):
			//printf("OC_DONE\n");
			clrvar(res);
			break;

		case XC( OC_EXIT ):
			//printf("OC_EXIT\n");
			awk_exit(L_d);

		/* -- recursive node type -- */

		case XC( OC_VAR ):
			//printf("OC_VAR\n");
			debug_printf_eval("VAR\n");
			//printf("op->l.v = %p\n", op->l.v);
			L.v = op->l.v;
			//printf("L.v = %p\n", L.v);
			//printf("intvar[NF] address: %p\n", intvar[NF]);
			//printf("L.v == intvar[NF] = %d\n", L.v == intvar[NF]);
			if (L.v == intvar[NF])
				split_f0();
			goto v_cont;

		case XC( OC_FNARG ):
			//printf("OC_FNARG\n");
			debug_printf_eval("FNARG[%d]\n", op->l.aidx);
			//printf("FNARG[%d]\n", op->l.aidx);
			L.v = &fnargs[op->l.aidx];
 v_cont:
			res = op->r.n ? findvar(iamarray(L.v), R.s) : L.v;
			break;

		case XC( OC_IN ):
			//printf("OC_IN\n");
			setvar_i(res, hash_search(iamarray(R.v), L.s) ? 1 : 0);
			break;

		case XC( OC_REGEXP ):
			//printf("OC_REGEXP\n");
			op1 = op;
			L.s = getvar_s(intvar[F0]);
			goto re_cont;

		case XC( OC_MATCH ):
			//printf("OC_MATCH\n");
			op1 = op->r.n;
 re_cont:
			{
				regex_t *re = as_regex(op1, &sreg);
				int i = regexec(re, L.s, 0, NULL, 0);
				if (re == &sreg)
					regfree(re);
				setvar_i(res, (i == 0) ^ (opn == '!'));
			}
			break;

		case XC( OC_MOVE ):
			debug_printf_eval("MOVE\n");
			//printf("OC_MOVE\n");
			/* if source is a temporary string, jusk relink it to dest */
//Disabled: if R.v is numeric but happens to have cached R.v->string,
//then L.v ends up being a string, which is wrong
//			if (R.v == v1+1 && R.v->string) {
//				res = setvar_p(L.v, R.v->string);
//				R.v->string = NULL;
//			} else {
				res = copyvar(L.v, R.v);
//			}
			break;

		case XC( OC_TERNARY ):
			//printf("OC_TERNARY\n");
			if ((op->r.n->info & OPCLSMASK) != OC_COLON)
				syntax_error(EMSG_POSSIBLE_ERROR);
			res = evaluate(istrue(L.v) ? op->r.n->l.n : op->r.n->r.n, res);
			break;

		case XC( OC_FUNC ): {
			//printf("OC_FUNC\n");
			var *vbeg, *v;
			const char *sv_progname;

			/* The body might be empty, still has to eval the args */
			if (!op->r.n->info && !op->r.f->body.first)
				syntax_error(EMSG_UNDEF_FUNC);

			vbeg = v = nvalloc(op->r.f->nargs + 1);
			while (op1) {
				var *arg = evaluate(nextarg(&op1), v1);
				copyvar(v, arg);
				v->type |= VF_CHILD;
				v->x.parent = arg;
				if (++v - vbeg >= op->r.f->nargs)
					break;
			}

			v = fnargs;
			fnargs = vbeg;
			sv_progname = g_progname;

			res = evaluate(op->r.f->body.first, res);

			g_progname = sv_progname;
			nvfree(fnargs);
			fnargs = v;

			break;
		}

		case XC( OC_GETLINE ):
		case XC( OC_PGETLINE ): {
			//printf("OC_GETLINE\n");
			rstream *rsm;
			int i;

			if (op1) {
				rsm = newfile(L.s);
				if (!rsm->F) {
					if ((opinfo & OPCLSMASK) == OC_PGETLINE) {
						rsm->F = popen(L.s, "r");
						rsm->is_pipe = TRUE;
					} else {
						rsm->F = fopen_for_read(L.s);  /* not xfopen! */
					}
				}
			} else {
				if (!iF)
					iF = next_input_file();
				rsm = iF;
			}

			if (!rsm || !rsm->F) {
				setvar_i(intvar[ERRNO], errno);
				setvar_i(res, -1);
				break;
			}

			if (!op->r.n)
				R.v = intvar[F0];

			i = awk_getline(rsm, R.v);
			if (i > 0 && !op1) {
				incvar(intvar[FNR]);
				incvar(intvar[NR]);
			}
			setvar_i(res, i);
			break;
		}

		/* simple builtins */
		case XC( OC_FBLTIN ): {
			//printf("OC_FBLTIN\n");
			double R_d = R_d; /* for compiler */

			switch (opn) {
			case F_in:
				R_d = (long long)L_d;
				break;

			case F_rn:
				R_d = (double)rand() / (double)RAND_MAX;
				break;

			case F_co:
				if (ENABLE_FEATURE_AWK_LIBM) {
					R_d = cos(L_d);
					break;
				}

			case F_ex:
				if (ENABLE_FEATURE_AWK_LIBM) {
					R_d = exp(L_d);
					break;
				}

			case F_lg:
				if (ENABLE_FEATURE_AWK_LIBM) {
					R_d = log(L_d);
					break;
				}

			case F_si:
				if (ENABLE_FEATURE_AWK_LIBM) {
					R_d = sin(L_d);
					break;
				}

			case F_sq:
				if (ENABLE_FEATURE_AWK_LIBM) {
					R_d = sqrt(L_d);
					break;
				}

				syntax_error(EMSG_NO_MATH);
				break;

			case F_sr:
				R_d = (double)seed;
				seed = op1 ? (unsigned)L_d : (unsigned)time(NULL);
				srand(seed);
				break;

			case F_ti:
				R_d = time(NULL);
				break;

			case F_le:
				debug_printf_eval("length: L.s:'%s'\n", L.s);
				//printf("length: L.s:'%s'\n", L.s);
				if (!op1) {
					L.s = getvar_s(intvar[F0]);
					//printf("length: L.s='%s'\n", L.s);
					debug_printf_eval("length: L.s='%s'\n", L.s);
				}
				else if (L.v->type & VF_ARRAY) {
					R_d = L.v->x.array->nel;
					//printf("length: array_len:%d\n", L.v->x.array->nel);
					debug_printf_eval("length: array_len:%d\n", L.v->x.array->nel);
					break;
				}
				R_d = strlen(L.s);
				break;

			case F_sy:
				//printf("F_sy\n");
				fflush_all();
				R_d = (ENABLE_FEATURE_ALLOW_EXEC && L.s && *L.s)
						? (system(L.s) >> 8) : 0;
				break;

			case F_ff:
				//printf("F_ff\n");
				if (!op1) {
					fflush(stdout);
				} else if (L.s && *L.s) {
					rstream *rsm = newfile(L.s);
					fflush(rsm->F);
				} else {
					fflush_all();
				}
				break;

			case F_cl: {
				//printf("F_cl\n");
				rstream *rsm;
				int err = 0;
				rsm = (rstream *)hash_search(fdhash, L.s);
				debug_printf_eval("OC_FBLTIN F_cl rsm:%p\n", rsm);
				if (rsm) {
					debug_printf_eval("OC_FBLTIN F_cl "
						"rsm->is_pipe:%d, ->F:%p\n",
						rsm->is_pipe, rsm->F);
					/* Can be NULL if open failed. Example:
					 * getline line <"doesnt_exist";
					 * close("doesnt_exist"); <--- here rsm->F is NULL
					 */
					if (rsm->F)
						err = rsm->is_pipe ? pclose(rsm->F) : fclose(rsm->F);
					free(rsm->buffer);
					hash_remove(fdhash, L.s);
				}
				if (err)
					setvar_i(intvar[ERRNO], errno);
				R_d = (double)err;
				break;
			}
			} /* switch */
			//printf("R_d = %f\n", R_d);
			setvar_i(res, R_d);
			//printf("res.string = %s\n", res->string);
			break;
		}

		case XC( OC_BUILTIN ):
			//printf("OC_BUILTIN\n");
			res = exec_builtin(op, res);
			break;

		case XC( OC_SPRINTF ):
			//printf("OC_SPRINTF\n");
			setvar_p(res, awk_printf(op1));
			break;

		case XC( OC_UNARY ): {
			//printf("OC_UNARY\n");
			double Ld, R_d;

			Ld = R_d = getvar_i(R.v);
			switch (opn) {
			case 'P':
				Ld = ++R_d;
				goto r_op_change;
			case 'p':
				R_d++;
				goto r_op_change;
			case 'M':
				Ld = --R_d;
				goto r_op_change;
			case 'm':
				R_d--;
 r_op_change:
				setvar_i(R.v, R_d);
				break;
			case '!':
				Ld = !istrue(R.v);
				break;
			case '-':
				Ld = -R_d;
				break;
			}
			setvar_i(res, Ld);
			break;
		}

		case XC( OC_FIELD ): {
			//printf("OC_FIELD\n");
			int i = (int)getvar_i(R.v);
			if (i < 0)
				syntax_error(EMSG_NEGATIVE_FIELD);
			if (i == 0) {
				res = intvar[F0];
			} else {
				split_f0();
				if (i > nfields)
					fsrealloc(i);
				res = &Fields[i - 1];
			}
			break;
		}

		/* concatenation (" ") and index joining (",") */
		case XC( OC_CONCAT ):
		case XC( OC_COMMA ): {
			//printf("OC_CONCAT\n");
			const char *sep = "";
			if ((opinfo & OPCLSMASK) == OC_COMMA) {
				//printf("OC_COMMA\n");
				sep = getvar_s(intvar[SUBSEP]);
			}
			setvar_p(res, xasprintf("%s%s%s", L.s, sep, R.s));
			break;
		}

		case XC( OC_LAND ):
			//printf("OC_LAND\n");
			setvar_i(res, istrue(L.v) ? ptest(op->r.n) : 0);
			break;

		case XC( OC_LOR ):
			//printf("OC_LOR\n");
			setvar_i(res, istrue(L.v) ? 1 : ptest(op->r.n));
			break;

		case XC( OC_BINARY ):
		case XC( OC_REPLACE ): {
			//printf("OC_BINARY\n");
			double R_d = getvar_i(R.v);
			debug_printf_eval("BINARY/REPLACE: R_d:%f opn:%c\n", R_d, opn);
			//printf("BINARY/REPLACE: R_d:%f opn:%c\n", R_d, opn);
			switch (opn) {
			case '+':
				L_d += R_d;
				break;
			case '-':
				L_d -= R_d;
				break;
			case '*':
				L_d *= R_d;
				break;
			case '/':
				if (R_d == 0)
					syntax_error(EMSG_DIV_BY_ZERO);
				L_d /= R_d;
				break;
			case '&':
				if (ENABLE_FEATURE_AWK_LIBM)
					L_d = pow(L_d, R_d);
				else
					syntax_error(EMSG_NO_MATH);
				break;
			case '%':
				if (R_d == 0)
					syntax_error(EMSG_DIV_BY_ZERO);
				L_d -= (long long)(L_d / R_d) * R_d;
				break;
			}
			debug_printf_eval("BINARY/REPLACE result:%f\n", L_d);
			//printf("BINARY/REPLACE result:%f\n", L_d);
			res = setvar_i(((opinfo & OPCLSMASK) == OC_BINARY) ? res : L.v, L_d);
			break;
		}

		case XC( OC_COMPARE ): {
			//printf("OC_COMPARE\n");
			int i = i; /* for compiler */
			double Ld;

			if (is_numeric(L.v) && is_numeric(R.v)) {
				Ld = getvar_i(L.v) - getvar_i(R.v);
			} else {
				//printf("NOT NUMARIC\n");
				const char *l = getvar_s(L.v);
				const char *r = getvar_s(R.v);
				Ld = icase ? strcasecmp(l, r) : strcmp(l, r);
			}
			switch (opn & 0xfe) {
			case 0:
				i = (Ld > 0);
				break;
			case 2:
				i = (Ld >= 0);
				break;
			case 4:
				i = (Ld == 0);
				break;
			}
			setvar_i(res, (i == 0) ^ (opn & 1));
			break;
		}

		default:
			syntax_error(EMSG_POSSIBLE_ERROR);
		} /* switch */
 next:
		if ((opinfo & OPCLSMASK) <= SHIFT_TIL_THIS)
			op = op->a.n;
		if ((opinfo & OPCLSMASK) >= RECUR_FROM_THIS)
			break;
		if (nextrec)
			break;
	} /* while (op) */

	nvfree(v1);
	debug_printf_eval("returning from %s(): %p\n", __func__, res);
	//printf("returning from %s(): %p\n", __func__, res);
	//printf("Res->number:%lf\n", res->number);
	return res;
#undef fnargs
#undef seed
#undef sreg
}


/* -------- main & co. -------- */

int awk_exit(int r)
{
	var tv;
	unsigned i;
	hash_item *hi;

	zero_out_var(&tv);

	if (!exiting) {
		exiting = TRUE;
		nextrec = FALSE;
		evaluate(endseq.first, &tv);
	}

	/* waiting for children */
	for (i = 0; i < fdhash->csize; i++) {
		hi = fdhash->items[i];
		while (hi) {
			if (hi->data.rs.F && hi->data.rs.is_pipe)
				pclose(hi->data.rs.F);
			hi = hi->next;
		}
	}

	exit(r);
}

/* if expr looks like "var=value", perform assignment and return 1,
 * otherwise return 0 */
int is_assignment(const char *expr)
{
	char *exprc, *val;

	if (!isalnum_(*expr) || (val = strchr(expr, '=')) == NULL) {
		return FALSE;
	}

	exprc = xstrdup(expr);
	val = exprc + (val - expr);
	*val++ = '\0';

	unescape_string_in_place(val);
	setvar_u(newvar(exprc), val);
	free(exprc);
	return TRUE;
}

/* switch to next input file */
rstream *next_input_file(void)
{
#define rsm          (G.next_input_file__rsm)
#define files_happen (G.next_input_file__files_happen)

	FILE *F;
	const char *fname, *ind;

	if (rsm.F)
		fclose(rsm.F);
	rsm.F = NULL;
	rsm.pos = rsm.adv = 0;

	for (;;) {
		if (getvar_i(intvar[ARGIND])+1 >= getvar_i(intvar[ARGC])) {
			if (files_happen)
				return NULL;
			fname = "-";
			F = stdin;
			break;
		}
		ind = getvar_s(incvar(intvar[ARGIND]));
		fname = getvar_s(findvar(iamarray(intvar[ARGV]), ind));
		// printf("fname:%s\n", fname);
		if (fname && *fname && !is_assignment(fname)) {
			F = xfopen_stdin(fname);
			break;
		}
	}

	files_happen = TRUE;
	setvar_s(intvar[FILENAME], fname);
	rsm.F = F;
	return &rsm;
#undef rsm
#undef files_happen
}

int awk_main(int argc UNUSED_PARAM, char **argv)
{
	unsigned opt;
	char *opt_F;
	llist_t *list_v = NULL;
	llist_t *list_f = NULL;
#if ENABLE_FEATURE_AWK_GNU_EXTENSIONS
	llist_t *list_e = NULL;
#endif
	int i, j;
	var *v;
	var tv;
	char **envp;
	char *vnames = (char *)vNames; /* cheat */
	char *vvalues = (char *)vValues;

	INIT_G();

	/* Undo busybox.c, or else strtod may eat ','! This breaks parsing:
	 * $1,$2 == '$1,' '$2', NOT '$1' ',' '$2' */
	if (ENABLE_LOCALE_SUPPORT)
		setlocale(LC_NUMERIC, "C");

	zero_out_var(&tv);

	/* allocate global buffer */
	g_buf = xmalloc(MAXVARFMT + 1);

	vhash = hash_init();
	ahash = hash_init();
	fdhash = hash_init();
	fnhash = hash_init();

	/* initialize variables */
	for (i = 0; *vnames; i++) {
		// printf("vnames in vnames loop: %s\n", vnames);
		// printf("i in vnames loop: %d\n", i);
		intvar[i] = v = newvar(nextword(&vnames));
		if (*vvalues != '\377')
			setvar_s(v, nextword(&vvalues));
		else
			setvar_i(v, 0);


		if (*vnames == '*') {
			// printf("i in vnames == *: %d\n", i);
			// printf("intvar[%d]->type before |=: %u\n", i, v->type);
			v->type |= VF_SPECIAL;
			// printf("intvar[%d]->type after |=: %u\n", i, v->type);
			vnames++;
		}
	}
	// printf("after vnames loop\n");
	// printf("FS type: %d\n", intvar[FS]->type);
	// printf("RS type: %d\n", intvar[RS]->type);
	handle_special(intvar[FS]);
	handle_special(intvar[RS]);

	newfile("/dev/stdin")->F = stdin;
	newfile("/dev/stdout")->F = stdout;
	newfile("/dev/stderr")->F = stderr;

	/* Huh, people report that sometimes environ is NULL. Oh well. */
	if (environ) for (envp = environ; *envp; envp++) {
		/* environ is writable, thus we don't strdup it needlessly */

		char *s = *envp;
		char *s1 = strchr(s, '=');

		if (s1) {
			*s1 = '\0';
			/* Both findvar and setvar_u take const char*
			 * as 2nd arg -> environment is not trashed */
			setvar_u(findvar(iamarray(intvar[ENVIRON]), s), s1 + 1);
			*s1 = '=';
		}
	}
	opt = getopt32(argv, OPTSTR_AWK, &opt_F, &list_v, &list_f, IF_FEATURE_AWK_GNU_EXTENSIONS(&list_e,) NULL);
	// printf("argv before adding optind: %s\n", *argv);
	argv += optind;
	// printf("argv after adding optind: %s\n", *argv);
	//argc -= optind;
	if (opt & OPT_W)
		bb_simple_error_msg("warning: option -W is ignored");
	if (opt & OPT_F) {
		unescape_string_in_place(opt_F);
		setvar_s(intvar[FS], opt_F);
	}
	while (list_v) {
		if (!is_assignment(llist_pop(&list_v)))
			bb_show_usage();
	}
	while (list_f) {
		// printf("enter list_f loop\n");
		char *s = NULL;
		FILE *from_file;

		g_progname = llist_pop(&list_f);
		from_file = xfopen_stdin(g_progname);
		/* one byte is reserved for some trick in next_token */
		for (i = j = 1; j > 0; i += j) {
			s = xrealloc(s, i + 4096);
			j = fread(s + i, 1, 4094, from_file);
		}
		s[i] = '\0';
		fclose(from_file);
		parse_program(s + 1);
		free(s);
	}
	g_progname = "cmd. line";
#if ENABLE_FEATURE_AWK_GNU_EXTENSIONS
	while (list_e) {
		// printf("enter list_e loop\n");
		parse_program(llist_pop(&list_e));
	}
#endif
	if (!(opt & (OPT_f | OPT_e))) {
		// printf("entering opt & opt_f | opt_e\n");
		if (!*argv)
			bb_show_usage();
		// printf("argv before parse_program: %s\n", *argv);
		parse_program(*argv++);
		// printf("argv after parse_program: %s\n", *argv);
	}

	/* fill in ARGV array */
	setari_u(intvar[ARGV], 0, "awk");
	// printf("intvar[ARGV]: %s\n", intvar[ARGV]->string);
	i = 0;
	while (*argv) {
		setari_u(intvar[ARGV], ++i, *argv++);
		// printf("i = %d\n", i);
	}
	setvar_i(intvar[ARGC], i + 1);
	// printf("intvar[ARGC]: %u\n", intvar[ARGC]->type);

	evaluate(beginseq.first, &tv);
	// printf("tv.aidx after 1st eval: %d\n", tv.x.aidx);
	if (!mainseq.first && !endseq.first)
		awk_exit(EXIT_SUCCESS);

	/* input file could already be opened in BEGIN block */
	if (!iF)
		iF = next_input_file();

	/* passing through input files */
	while (iF) {
		// printf("entering iF while loop\n");
		nextfile = FALSE;
		setvar_i(intvar[FNR], 0);

		// print_rstream(iF);
		// print_var(intvar[F0]);
		while ((i = awk_getline(iF, intvar[F0])) > 0) {
			nextrec = FALSE;
			incvar(intvar[NR]);
			incvar(intvar[FNR]);
			evaluate(mainseq.first, &tv);

			if (nextfile)
				break;
		}

		if (i < 0)
			syntax_error(strerror(errno));

		iF = next_input_file();
	}

	awk_exit(EXIT_SUCCESS);
	/*return 0;*/
}
