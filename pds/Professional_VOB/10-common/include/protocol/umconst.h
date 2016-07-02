#ifndef _UMCOSNT_H_
#define _UMCONST_H_
//错误代码
#define UM_LOGIN_NOUSER		19001
#define UM_LOGIN_ERRPSW		19002
#define UM_ACTOR_NOTMATCH	19003
#define UM_USER_DUPLICATE	19004
#define UM_USER_NOTEXIST	19005
#define UM_NOT_ENOUGHMEM	19006
#define UM_ACTOR_ONLY		19007
#define UM_OPERATE_NOTPERMIT    19008
#define UM_MULTILOGIN		0xffff //终端定义的一个返回，不允许重复登录的错误码
//返回成功
#define UM_OK			0xAA
//定义最大的用户数目
#define MAX_USERNUM		128
#define MAX_CHARLENGTH		32
//定义用户身份
#define UM_ADMIN		0x01
#define UM_OPERATOR		0x02
//每条UM_MCS_GETALLUSER_NOTIF返回的用户信息数
#define MAX_USERINFO_PER_MSG    15
//最大文件名长度
#define MAX_UMFILENAME_LEN        32

#endif
