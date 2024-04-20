/* vi: set sw=4 ts=4: */
/*
 * ascii-to-numbers implementations for busybox
 *
 * Copyright (C) 2003  Manuel Novoa III  <mjn3@codepoet.org>
 *
 * Licensed under GPLv2, see file LICENSE in this source tree.
 */
// #include "libbb.h"
#include "inc_stripped.h"

#ifdef WEEEEEEE
#define type long long
#define xstrtou(rest) xstrtoull##rest
#define xstrto(rest) xstrtoll##rest
#define xatou(rest) xatoull##rest
#define xato(rest) xatoll##rest
#define XSTR_UTYPE_MAX ULLONG_MAX
#define XSTR_TYPE_MAX LLONG_MAX
#define XSTR_TYPE_MIN LLONG_MIN
#define XSTR_STRTOU strtoull
#include "xatonum_template.c"
#endif

#if ULONG_MAX != ULLONG_MAX
#define type long
#define xstrtou(rest) xstrtoul##rest
#define xstrto(rest) xstrtol##rest
#define xatou(rest) xatoul##rest
#define xato(rest) xatol##rest
#define XSTR_UTYPE_MAX ULONG_MAX
#define XSTR_TYPE_MAX LONG_MAX
#define XSTR_TYPE_MIN LONG_MIN
#define XSTR_STRTOU strtoul
#include "xatonum_template.c"
#endif

#if UINT_MAX != ULONG_MAX
static ALWAYS_INLINE
unsigned bb_strtoui(const char *str, char **end, int b)
{
	unsigned long v = strtoul(str, end, b);
	if (v > UINT_MAX) {
		errno = ERANGE;
		return UINT_MAX;
	}
	return v;
}
#define type int
#define xstrtou(rest) xstrtou##rest
#define xstrto(rest) xstrtoi##rest
#define xatou(rest) xatou##rest
#define xato(rest) xatoi##rest
#define XSTR_UTYPE_MAX UINT_MAX
#define XSTR_TYPE_MAX INT_MAX
#define XSTR_TYPE_MIN INT_MIN
/* libc has no strtoui, so we need to create/use our own */
#define XSTR_STRTOU bb_strtoui
// #include "xatonum_template.c"
unsigned type FAST_FUNC xstrtou(_range_sfx)(const char *numstr, int base,
		unsigned type lower,
		unsigned type upper,
		const struct suffix_mult *suffixes)
{
	unsigned type r;
	int old_errno;
	char *e;

	/* Disallow '-' and any leading whitespace. */
	if (*numstr == '-' || *numstr == '+' || isspace(*numstr))
		goto inval;

	/* Since this is a lib function, we're not allowed to reset errno to 0.
	 * Doing so could break an app that is deferring checking of errno.
	 * So, save the old value so that we can restore it if successful. */
	old_errno = errno;
	errno = 0;
	r = XSTR_STRTOU(numstr, &e, base);
	/* Do the initial validity check.  Note: The standards do not
	 * guarantee that errno is set if no digits were found.  So we
	 * must test for this explicitly. */
	if (errno || numstr == e)
		goto inval; /* error / no digits / illegal trailing chars */

	errno = old_errno;  /* Ok.  So restore errno. */

	/* Do optional suffix parsing.  Allow 'empty' suffix tables.
	 * Note that we also allow nul suffixes with associated multipliers,
	 * to allow for scaling of the numstr by some default multiplier. */
	if (suffixes) {
		while (suffixes->mult) {
			if (strcmp(suffixes->suffix, e) == 0) {
				if (XSTR_UTYPE_MAX / suffixes->mult < r)
					goto range; /* overflow! */
				r *= suffixes->mult;
				goto chk_range;
			}
			++suffixes;
		}
	}

	/* Note: trailing space is an error.
	 * It would be easy enough to allow though if desired. */
	if (*e)
		goto inval;
 chk_range:
	/* Finally, check for range limits. */
	if (r >= lower && r <= upper)
		return r;
 range:
	bb_error_msg_and_die("number %s is not in %llu..%llu range",
		numstr, (unsigned long long)lower,
		(unsigned long long)upper);
 inval:
	bb_error_msg_and_die("invalid number '%s'", numstr);
}
unsigned type FAST_FUNC xatou(_range)(const char *numstr,
		unsigned type lower,
		unsigned type upper)
{
	return xstrtou(_range_sfx)(numstr, 10, lower, upper, NULL);
}
// end of xatonum_template.c
#endif

/* A few special cases */

int FAST_FUNC xatoi_positive(const char *numstr)
{
	return xatou_range(numstr, 0, INT_MAX);
}

uint16_t FAST_FUNC xatou16(const char *numstr)
{
	return xatou_range(numstr, 0, 0xffff);
}

const struct suffix_mult bkm_suffixes[] ALIGN_SUFFIX = {
	{ "b", 512 },
	{ "k", 1024 },
	{ "m", 1024*1024 },
	{ "", 0 }
};

const struct suffix_mult cwbkMG_suffixes[] ALIGN_SUFFIX = {
	{ "c", 1 },
	{ "w", 2 },
	{ "b", 512 },
	{ "kB", 1000 },
	{ "kD", 1000 },
	{ "k", 1024 },
	{ "KB", 1000 }, /* compat with coreutils dd */
	{ "KD", 1000 }, /* compat with coreutils dd */
	{ "K", 1024 },  /* compat with coreutils dd */
	{ "MB", 1000000 },
	{ "MD", 1000000 },
	{ "M", 1024*1024 },
	{ "GB", 1000000000 },
	{ "GD", 1000000000 },
	{ "G", 1024*1024*1024 },
	/* "D" suffix for decimal is not in coreutils manpage, looks like it's deprecated */
	/* coreutils also understands TPEZY suffixes for tera- and so on, with B suffix for decimal */
	{ "", 0 }
};

const struct suffix_mult kmg_i_suffixes[] ALIGN_SUFFIX = {
	{ "KiB", 1024 },
	{ "kiB", 1024 },
	{ "K", 1024 },
	{ "k", 1024 },
	{ "MiB", 1048576 },
	{ "miB", 1048576 },
	{ "M", 1048576 },
	{ "m", 1048576 },
	{ "GiB", 1073741824 },
	{ "giB", 1073741824 },
	{ "G", 1073741824 },
	{ "g", 1073741824 },
	{ "KB", 1000 },
	{ "MB", 1000000 },
	{ "GB", 1000000000 },
	{ "", 0 }
};
