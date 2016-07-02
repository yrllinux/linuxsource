#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

int test(int *a)
{
	int b = 3;
	int c = 0;

	c = *a;

	*a = c + b;

	return c;

}

char *
read_file_malloc_buf(char *fname)
{
	register int i, fd, cc;
	register char *cp;
	struct stat buf;

	fd = open(fname, O_RDONLY);
	if (fd < 0)
		printf("can't open %s", fname);

	if (fstat(fd, &buf) < 0)
		printf("can't stat %s", fname);

	cp = malloc((u_int)buf.st_size + 1);
	if (cp == NULL)
  		printf("malloc(%d) for %s", (u_int)buf.st_size + 1, fname);
	cc = read(fd, cp, (u_int)buf.st_size);
	if (cc < 0)
		printf("read %s", fname);
	if (cc != buf.st_size)
		printf("short read %s (%d != %d)", fname, cc, (int)buf.st_size);

	close(fd);
	/* replace "# comment" with spaces */
	for (i = 0; i < cc; i++) {
		if (cp[i] == '#')
			while (i < cc && cp[i] != '\n')
				cp[i++] = ' ';
	}
	cp[cc] = '\0';
	return (cp);
}


/*
 * Copy arg vector into a new buffer, concatenating arguments with spaces.
 */
char *
copy_argv_malloc_buf(char **argv)
{
	char **p;
	u_int len = 0;
	char *buf;
	char *src, *dst;

	p = argv;
	if (*p == 0)
		return 0;

	while (*p)
		len += strlen(*p++) + 1;

	buf = (char *)malloc(len);
	if (buf == NULL)
		printf("copy_argv: malloc");

	p = argv;
	dst = buf;
	while ((src = *p++) != NULL) {
		while ((*dst++ = *src++) != '\0')
			;
		dst[-1] = ' ';
	}
	dst[-1] = '\0';

	return buf;
}

/*
 * Format the timestamp
 */
char *ts_format(int sec, int usec)
{
        static char buf[sizeof("00:00:00.000000")];
        (void)snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%06u",
               sec / 3600, (sec % 3600) / 60, sec % 60, usec);

        return buf;
}

