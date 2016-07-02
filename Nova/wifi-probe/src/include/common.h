#ifndef __COMMON_H__
#define __COMMON_H__



extern int test(int *a);
extern char *read_file_malloc_buf(char *fname);
extern char *copy_argv_malloc_buf(char **argv);
extern char *ts_format(int sec, int usec);

#endif /* __COMMON_H__ */
