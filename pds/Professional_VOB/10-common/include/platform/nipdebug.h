#ifndef __NIPDEBUG_NIPDEBUG_H__
#define __NIPDEBUG_NIPDEBUG_H__
#include <signal.h>

#define NO_SYMBOL_BT_SIZE 4	/* elf无符号，且没有指定符号文件，且没有指定大小时默认打印堆栈大小 */
#ifdef __cplusplus
extern "C"{
#endif

/* 
 * 说明：btsize最多显示回溯个数，为0则找到main函数为止
 * 	 symbol_file是符号文件（可执行文件本身包含符号表时会忽略此参数）
 *	 crushdump_file是程序异常的输出文件，为空会输出到控制台
 *	 signum信号id
 * 示例：init_nipdebug(0,  NULL, NULL);
 *	 init_nipdebug(10, NULL, "/usr/cursh.out");
 *	 init_nipdebug(0,  "/bin/ebt.sym", "/usr/cursh.out");
 *	 init_nipdebug2(SIGPIPE,  "/bin/ebt.sym", "/usr/cursh.out");
 */
int init_nipdebug(int btsize, const char *symbol_file, const char *crushdump_file);
int init_nipdebug2(int signum, int btsize, const char *symbol_file, const char *crushdump_file);

#ifdef __cplusplus
}
#endif
#endif

