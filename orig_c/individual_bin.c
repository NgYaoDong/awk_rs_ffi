/* Minimal wrapper to build an individual busybox applet.
 *
 * Copyright 2005 Rob Landley <rob@landley.net
 *
 * Licensed under GPLv2, see file LICENSE in this source tree.
 */

// const char *applet_name;
const char *applet_name = "awk";

// #include "usage.h"
#include "inc_stripped.h"
#include <stdio.h>
#include <stdlib.h>

// copied from usage.h
#define awk_trivial_usage \
       "[OPTIONS] [AWK_PROGRAM] [FILE]..." \

#define awk_full_usage "\n\n" \
       "	-v VAR=VAL	Set variable" \
     "\n	-F SEP		Use SEP as field separator" \
     "\n	-f FILE		Read program from FILE" \
	IF_FEATURE_AWK_GNU_EXTENSIONS( \
     "\n	-e AWK_PROGRAM" \
	) \
// end of copied from usage.h

#define APPLET_main awk_main
#define APPLET_full_usage awk_full_usage

// This file is the main file of the individual busybox applet.
// The `individual.c` is for the library.
int main(int argc, char **argv)
{
	applet_name = argv[0];
	return APPLET_main(argc, argv);
}

void bb_show_usage(void)
{
	// fputs(APPLET_full_usage "\n", stdout);
	fputs("Usage: awk " awk_trivial_usage APPLET_full_usage "\n", stdout);
	exit(EXIT_FAILURE);
}
