/* vi: set sw=4 ts=4: */
/*
 * Utility routines.
 *
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */
// #include "libbb.h"
#include "inc_stripped.h"

/////// copied from safe_write
ssize_t FAST_FUNC safe_write(int fd, const void *buf, size_t count)
{
	ssize_t n;

	for (;;) {
		n = write(fd, buf, count);
		if (n >= 0 || errno != EINTR)
			break;
		/* Some callers set errno=0, are upset when they see EINTR.
		 * Returning EINTR is wrong since we retry write(),
		 * the "error" was transient.
		 */
		errno = 0;
		/* repeat the write() */
	}

	return n;
}
///////

/*
 * Write all of the supplied buffer out to a file.
 * This does multiple writes as necessary.
 * Returns the amount written, or -1 if error was seen
 * on the very first write.
 */
ssize_t FAST_FUNC full_write(int fd, const void *buf, size_t len)
{
	ssize_t cc;
	ssize_t total;

	total = 0;

	while (len) {
		cc = safe_write(fd, buf, len);

		if (cc < 0) {
			if (total) {
				/* we already wrote some! */
				/* user can do another write to know the error code */
				return total;
			}
			return cc;  /* write() returns -1 on failure. */
		}

		total += cc;
		buf = ((const char *)buf) + cc;
		len -= cc;
	}

	return total;
}
