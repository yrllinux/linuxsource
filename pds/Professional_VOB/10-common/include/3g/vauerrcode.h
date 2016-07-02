#ifndef _VAU_ERR_CODE_H_
#define _VAU_ERR_CODE_H_

//#include "kdmerrorid.h"
#define ERR_SUCCESS 0
#define ERR_CU_BGN  19000
enum EVauErr
{
    ERR_VAU_SUCCESS = ERR_SUCCESS,    // 成功
    ERR_VAU_ERROR = ERR_CU_BGN,       // 失败
    ERR_VAU_TIMEOUT,                  // 超时
    ERR_VAU_SESSION_INVALID,          // 会话已无效

	ERR_VAU_CREAT_APP_FAIL,           // 创建APP失败
	ERR_VAU_INTERNAL,				  // VAU内部错误
	ERR_VAU_PARAM,					  // 错误的参数

	ERR_VAU_PATH_LEN,                 // 错误的路径长度
	ERR_VAU_FILE_NAME_LEN,            // 错误的文件名 长度
	ERR_VAU_NOEXIST_FILE,	          // 文件不存在
    ERR_VAU_NOEXIST_USERNAME,         // 用户名不存在
    ERR_VAU_WRONG_USERPWD,            // 密码错误
    ERR_VAU_USER_UNUSED,              // 用户已被停用
    ERR_VAU_EXPIREED_USERNAME,        // 用户名使用期限已到
    ERR_VAU_EXPIREED_USERPWD,         // 密码使用期限已到
    ERR_VAU_OVERLOGIN,                // 同一用户帐号已登陆

    ERR_VAU_URL_INVALID,
    ERR_VAU_PU_INVALID,               // 非法PU
    ERR_VAU_PU_CHN_INVALID,           // 非法PU通道
    ERR_VAU_TRACK_INVALID,

    ERR_VAU_EXPLORE_EXCEED_RESTRICT,  // 超过最大浏览路数
	ERR_VAU_REQUEST_EXCEED_RESTRICT,  // 超过最大点播路数
	ERR_VAU_DISCONNECTED_FROM_CMS,
};

#endif // _VAU_ERR_CODE_H_
