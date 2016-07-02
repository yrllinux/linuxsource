#ifndef COSDMAPDEF_H
#define COSDMAPDEF_H

#include "kdvtype.h"
#include "osp.h"
//typedef struct tagBackBGDColor
//{
//	u8 RColor;    //R背景
//	u8 GColor;    //G背景
//	u8 BColor;    //B背景	
//}TBGColor;
//
//
///* 滚动横幅(从左往右滚)参数 */
//typedef struct
//{
//	u32 dwXPos;
//    u32 dwYPos;//显示区域的Y坐标
//    u32 dwWidth;//显示区域的宽
//    u32 dwHeight;//显示区域的高
//    u32 dwBMPWight;//BMP图像宽
//    u32 dwBMPHeight;//BMP图像高
//    u32 dwBannerCharHeight;//字体高度－－主要用于上下滚动时，确定停顿的位置
//    TBGColor tBackBGDColor;//背景色 
//    u32 dwBGDClarity;//具体颜色透明度
//    u32 dwPicClarity;//整幅图片透明度
//    u32 dwRunMode;//滚动模式：上下or左右or静止（宏定义如上）
//    u32 dwRunSpeed;//滚动速度 四个等级（宏定义如上）
//    u32 dwRunTimes;//滚动次数 0为无限制滚动
//	u32 dwHaltTime;//停顿时间(秒)，上下滚动时，每滚完一行字的停顿间隔：0始终停留（与静止等同） 
//}TAddBannerParam;
//
//typedef TAddBannerParam TDynimicPicInfo;
//
//#define SPEED_FAST 2
//#define SPEED_HIGH 3
//#define SPEED_NORM 6
//#define SPEED_LOW  8
//
//#define UPDWON     1
//#define LEFTRIGHT  2
//#define STATIC     3
//
//const u32 dwUpDownStep    = 16;
//const u32 dwLeftRightStep = 16;
//
//
//const u8  byOsdProcess_Priority  = 5;
//const u32 dwOsdProcess_Stacksize = 0;
//const u16  wOsdProcess_Flag      = 0;
//
//const u8  byAllTransparent = 0x00; 
//const u8  byNoneTransparent = 0xFF;
//const u32 dwOsdDelayTimes = 100;
//
//const u32 dwOsdBackgroudSize = 720 * 288 * 2;



#endif
