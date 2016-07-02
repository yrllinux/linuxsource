/*
 * auth: wangyuantao@kedacom.com
 * date: Tue Dec  5 17:41:33 CST 2006
 * descriptor: library head for vlynq community
 */
#ifndef __VLYNQ_INC_H__
#define __VLYNQ_INC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * value = 1: enable
 * value = 0: disable
 * other value, ignore it !!!
 * default is 0
 */
int vlynq_enable_dma(int value);

/*************************************************************/
int vlynq_channel_create(const char *channel_name, int size, int crc_flag);
/*
 * 说明：创建通道
 * 参数：channel_name：通道名，最多12字符（含\0）
 *	 size: 通道大小，单位为kb
 *	 crc_flag: crc效验标识，0为不使用crc，其他值为使用crc
 * 返回：0为成功
 *	 -1通道已存在
 *	 -2通道太大无法创建
 *	 -3从处理器不能创建通道
 *	 -4vlynq初始化失败不能创建通道
 *	 -5通道已达最大个数不能创建
 *	 -6参数错误
 *	 -7其他错误
 * 注意：size 是kb单位，由于内存映射时要对齐页面，size只能是4 的倍数
 */

int vlynq_channel_destroy(const char *channel_name);
/*
 * 说明：删除通道
 * 参数：通道名
 * 返回：0为成功
 *	 -1通道不存在
 *	 -2删除通道失败
 */

int vlynq_channel_open(const char *channel_name);
/*
 * 说明：打开通道
 * 参数：通道名
 * 返回：通道句柄
 * 	 非0表示打开通道成功
 *	 0表示打开通道失败
 */

int vlynq_channel_close(int channel_handle);

/*
 * 说明：关闭打开的通道
 * 参数：通道句柄
 * 返回：关闭通道成功为0，失败为-1
 */

/*************************************************************/
int vlynq_channel_package_num(int channel_handle);
/*
 * 说明：获取该数据通道上可以读取的数据包个数，通道不存在或失败返回－1并置errno
 * 参数：通道句柄
 * 返回：数据包个数（0或大于0），返回负数表示失败
 */

int vlynq_channel_clear(int channel_handle);
/*
 * 说明：清空通道上的数据包: 主从都可以清
 * 参数：通道句柄
 * 返回：成功返回0，失败返回负数
 */

int vlynq_channel_info(int channel_handle, char *name, int *size, int *crc_flag);
/*
 * 说明：获取通道信息（通道名，大小，是否crc效验）
 * 参数：
 * 	 输出： channel_handle: 通道句柄
 * 	 输入： name: 通道名
 *		size: 通道大小，单位为kb
 *		crc_flag: crc效验标识
 * 返回：成功返回0，失败返回负数
 */

int vlynq_channel_freesize(int channel_handle);
/*
 * 说明：获取通道剩余空间
 * 参数：通道句柄
 * 返回：失败返回负数, 成功返回可以写的偏移
 * 单位：
 */

int vlynq_channel_write(int channel_handle, const char* buffer, int len, int wait_flag);
int vlynq_channel_write2(int channel_handle, const char* buffer1, int len1, const char* buffer2, int len2, 
		int wait_flag);
/*
 * 说明：写入数据，返回实际写入数量或失败
 * 参数：wait_flag，表示如果没有空闲空间，等待的超时时间
 *	 buffer要发送数据的缓冲 (buffer1, buffer2 是一个数据包的两部分缓冲)
 *	 len发送数据长度
 *	 wait_flag，超时标记:   0直到数据发出成功为止
 *				1或正数：最多等待毫秒数，如果该时间内还未发送返回超时
 *				-1不超时，立即返回
 * 返回：0或正数为发送成功，表示成功发送的数据包长度
 *	 -1为超时
 *	 -2通道上无足够的空间
 *	 -3其他错误
 */

int vlynq_channel_read(int channel_handle, char *buffer, int wait_flag);
/*
 * 说明：读取数据，返回实际读取数量或失败
 * 参数：buffer: 接收数据的缓冲
 * 返回：0或正数为读取数据成功，表示成功读取的数据包的长度
 * 	 -1为通道上无数据包
 * 	 -2为crc效验错误
 * 	 -3为错误
 */
int vlynq_dump_config();
int vlynq_apitest_master(void);
int vlynq_apitest_slave(void);
/*
 * 说明：vlynq功能模块测试接口
 * 参数：无
 * 返回：0为成功，其他为失败
 * 其他：在内核模块加载后，主处理器调用vlynq_apitest_master对映射内存区域进行初始化，
 *       然后进行收发数据，然后从处理器调用vlynq_apitest_slave进行收发数据测试。
 *		 这两个函数没有先后顺序，如果主或从的某一端状态未准备好，将进入阻塞直到进入正确状态。
 */

char *vlynq_version();
#define VLYNQ__VERSION	"VLYNQ 1.1.2.20071029"
/***************************
模块版本：VLYNQ 1.1.1.20070827
增加功能：无
修改缺陷：修改函数，使得到正确的通道剩余空间
提交人：王远涛
*****************************/

/***************************
模块版本：VLYNQ 1.1.1.20070830
增加功能：无
修改缺陷：修改函数，使得到正确的通道剩余空间
提交人：王远涛
*****************************/

/***************************
模块版本：VLYNQ 1.1.2.20070911
增加功能：无
修改缺陷：和nipdebug定义了相同的全局变量
提交人：王远涛
*****************************/

/***************************
模块版本：VLYNQ 1.1.2.20071025
增加功能：无
修改缺陷：对于vlynq读写数据，特别是写做了很多优化
提交人：王远涛
*****************************/

/***************************
模块版本：VLYNQ 1.1.2.20071029
增加功能：无
修改缺陷：增加了dma使能及相关代码
提交人：王远涛
*****************************/

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif

