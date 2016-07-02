/*****************************************************************************
模块名      : GPS
文件名      : gpsLib.h
相关文件    : 
文件实现功能:  .
作者        : 张方明
版本        : V1.0  Copyright(C) 2005-2007 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2005/11/15  01a         张方明      创建	
******************************************************************************/
#ifndef __INC_GPS_LIB_H
#define __INC_GPS_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

/* 头文件 */
#include "kdvtype.h"

/* GPS协议解析模块版本宏定义 */
#define GPS_LIB_VER            "GPS_LIB 40.01.01.01.051125"
#define GPS_VER_MAX_LEN                    255  /* 版本信息的最大长度 */


/* GPS 返回值定义 */
#define GPS_OPT_SUCCESS                      0

#define GPS_UNDEF_ERR                       -1
#define GPS_ERR_BASE                         1
#define GPS_ERR_TIMEOUT        (GPS_ERR_BASE+0) /* 数据接收超时,默认3秒,可能是GPS设备没有接或波特率不匹配 */
#define GPS_ERR_LENERR         (GPS_ERR_BASE+1) /* 数据长度错误，一般指没收到完整的一条消息 */
#define GPS_ERR_PARAMERR       (GPS_ERR_BASE+2) /* 参数错误 */
#define GPS_ERR_DATAERR        (GPS_ERR_BASE+3) /* 数据错误，一般指某字段数据内容错 */
#define GPS_ERR_CRCERR         (GPS_ERR_BASE+4) /* Checksum校验错误 */
#define GPS_ERR_NOT_OPENED     (GPS_ERR_BASE+5) /* GPS设备没有打开 */
#define GPS_ERR_OPEN_FAILED    (GPS_ERR_BASE+6) /* GPS设备打开失败，即打开串口失败 */
#define GPS_ERR_READ_FAILED    (GPS_ERR_BASE+7) /* GPS设备读串口失败 */
#define GPS_ERR_WRITE_FAILED   (GPS_ERR_BASE+8) /* GPS设备写串口失败 */
#define GPS_ERR_CLOSE_FAILED   (GPS_ERR_BASE+9) /* GPS设备关串口失败 */


/* 设备控制部分宏定义 */
#define GPS_MAX_DEVS                         1  /* 最多支持的GPS设备数 */


/* 协议相关宏定义 */
#define GPS_GSV_MAX_MSGS                     3  /* 最多上报3条GSV消息 */
#define GPS_SEGMENT_MAX_LEN                 12  /* 消息中字段最大长度 */
#define GPS_SAT_USED_MAX_NUM                12  /* GPS最多可以跟踪的卫星数目 */
#define GPS_SAT_GROUPS_PER_GSV               4  /* 每个GSV消息中最多包含卫星信息组的数目 */


/*------------------------------  GGA消息结构定义 ----------------------------
  $GPGGA,hhmmss.sss,ddmm.mmmm,t,ooonn.nnnn,e,q,ss,y.y,a.a,z,g.g,z,b.b,
   iiii*CC<CR><LF>
 例:$GPGGA,091522.286,3111.3348,N,12125.8920,E,1,03,23.0,0.0,M,8.1,M,0.0,0000*70
 ----------------------------------------------------------------------------
    字段    | 功能介绍                                 | 值的范围,全部是字符
    hh      | Hours (time stamp of position) 时         0 .. 23
    mm      | Minutes (time stamp of position) 分       0 .. 59
    ss.sss  | Seconds (time stamp of position) 秒.毫秒  0 .. 59.999
    dd      | Latitude degrees 纬度,度                  0 .. 90
    mm.mmmm | Latitude minutes 纬度,分                  00.0000 .. 59.9999
    t       | Latitude direction 纬度方向                N =	North 北纬
            |                                           S =	South 南纬
    ooo     | Longitude degrees 经度,度                 0 .. 180
    nn.nnnn | Longitude minutes 经度,分	                00.0000 .. 59.9999
    e       | Longitude direction 经度方向		        E =	East 东经
            |                                           W =	West 西经
    q       | GPS status indicator          0 = Fix not available or invalid
            | GPS状态指示                    1 = GPS SPS Mode, fix valid
            |                               2 =	Diferential GPS, SPS Mode, fix valid
            |                               3 =	GPS PPS Mode, fix valid
    ss      | Number of satellites used
            | 使用的卫星个数                             0 .. 12
    y.y     | HDOP(Horizontal Dilution of Precision)    0.0 .. 99.9
    a.a     | GPS antenna height in meters              注意:小数点前面长度不定,可能多个字节
            | 天线的高度，指海拔，米                      需要4颗可用卫星，否则为0.0
    z       | Units 单位		                            M	=	meters
    g.g     | Geoid separation 大地水准面
    z       | Units	                                    M	=	meters
    b.b     | Age of differential data      0.0 field when DGPS is not used
    iiii    | Differential reference station ID 微分参考身份号  0000 .. 1023
    UTC为格林尼制时间,记为T,它是经度为0的地区的绝对时间。
    本地时间算法:东经x度时间=(T+12*x/180)%24; 西经x度时间=(24+T-12*x/180)%24
---------------------------------------------------------------------------*/
typedef struct{
    u8 abyUTC[GPS_SEGMENT_MAX_LEN];             /* hhmmss.sss 格林尼制时间 */
    u8 abyLatitude[GPS_SEGMENT_MAX_LEN];        /* ddmm.mmmm  纬度 */
    u8 abyLatitudeDir[GPS_SEGMENT_MAX_LEN];     /* t          纬度方向 */
    u8 abyLongitude[GPS_SEGMENT_MAX_LEN];       /* ooonn.nnnn 经度 */
    u8 abyLongitudeDir[GPS_SEGMENT_MAX_LEN];    /* e          经度方向 */
    u8 abyGpsStatus[GPS_SEGMENT_MAX_LEN];       /* q          GPS状态指示 */
    u8 abyUsedSats[GPS_SEGMENT_MAX_LEN];        /* ss         使用的卫星个数 */
    u8 abyHDOP[GPS_SEGMENT_MAX_LEN];            /* y.y              */
    u8 abyAltitude[GPS_SEGMENT_MAX_LEN];        /* a.a        海拔*/
    u8 abyAltitudeUnit[GPS_SEGMENT_MAX_LEN];    /* z          海拔值单位 */
    u8 abyGeoid[GPS_SEGMENT_MAX_LEN];           /* g.g        大地水准面 */
    u8 abyGeoidUint[GPS_SEGMENT_MAX_LEN];       /* z          大地水准面单位*/
    u8 abyAgeofDiffData[GPS_SEGMENT_MAX_LEN];   /* b.b                      */
    u8 abyDifRefStationID[GPS_SEGMENT_MAX_LEN]; /* iiii       微分参考身份号 */
    BOOL32  bValid;                             /* TRUE/FALSE GGA消息有效标志  */
}TInfoGGA;


/*------------------------------  GSA消息结构定义 ----------------------------
  $GPGSA,a,b,cc,dd,ee,ff,gg,hh,ii,jj,kk,mm,nn,oo,p.p,q.q,r.r*CC<CR><LF>
 例:$GPGSA,A,2,20,11,19,,,,,,,,,,30.5,23.0,20.0*0C
 ----------------------------------------------------------------------------
    字段    | 功能介绍                                 | 值的范围,全部是字符
    a       | Satellite Acquisition Mode
            | 卫星获取的模式	   M = Manual (forced to operate in 2D or 3D mode)
            |                  A = Automatic (automatically switch between 2D and 3D)
    b       | Positioning Model	                        1 =	Fix not available
            | 定位模式                                   2 =	2D
            |                                           3 =	3D
    cc, dd, ee, ff, gg, hh, ii, jj, kk, mm, nn, oo |    使用卫星的编号，最多12个
            | Satellite PRNs used in solution (null for unused fields)
    p.p     | PDOP(Position dilution of precision)      00.0 .. 99.9
            | 位置之精度强弱度
    q.q     | HDOP(Horizontal dilution of precision)
            | 水平（即二维）坐标之精度强弱度               00.0 .. 99.9
    r.r     | VDOP(Vertical dilution of precision)
            | 垂直（即高度）坐标之精度强弱度               00.0 .. 99.9
---------------------------------------------------------------------------*/
typedef struct{
    u8 abySatMode1[GPS_SEGMENT_MAX_LEN];        /* a   卫星获取的模式 */
    u8 abySatMode2[GPS_SEGMENT_MAX_LEN];        /* b   定位模式 */
    u8 abySatIDUsed[GPS_SAT_USED_MAX_NUM][GPS_SEGMENT_MAX_LEN];/* cc..oo 使用卫星的编号 */
    u8 abyPDOP[GPS_SEGMENT_MAX_LEN];            /* p.p 位置之精度强弱度 */
    u8 abyHDOP[GPS_SEGMENT_MAX_LEN];            /* q.q 水平(即二维)坐标之精度强弱度 */
    u8 abyVDOP[GPS_SEGMENT_MAX_LEN];            /* r.r 垂直(即高度)坐标之精度强弱度 */
    BOOL32  bValid;                             /* TRUE/FALSE GSA消息有效标志  */
}TInfoGSA;


/*------------------------------- GSV 消息结构定义 ---------------------------
  $GPGSV,t,m,nn,ii,ee,aaa,ss,ii,ee,aaa,ss,ii,ee,aaa,ss,ii,ce,aaa,ss*CC<CR><LF>
 例:$GPGSV,3,1,09,11,59,040,42,28,55,322,00,24,48,223,00,08,41,239,00*79
    $GPGSV,3,2,09,20,35,136,43,27,27,208,29,07,20,287,00,19,20,069,00*7A
    $GPGSV,3,3,09,04,05,224,00*45
 ----------------------------------------------------------------------------
    字段    | 功能介绍                                 | 值的范围,全部是字符
    t       | Number of Messages                        1 .. 3
            | 本次上报总共有几条GSV消息，分批上报
    m       | Message Number                            1 .. 3
            | 当前该消息的序号
    nn      | Total number of satellites in view        0 .. 12
            | 可视卫星的总数

[ For each visible satellite (four groups per message) ]:
    ii      | Satellite ID                              1 .. 32
    ee      | Elevation in degrees 
            | 仰角，以度为单位                           0 .. 90 度
    aaa     | Azimuth in degrees True 方位角            0 .. 359 度
    ss      | SNR in dB  信噪比                         0 .. 99
[ End each visible satellite (four groups per message) ]		
---------------------------------------------------------------------------*/
typedef struct{
    u8       abySatID[GPS_SEGMENT_MAX_LEN];        /* ii  卫星编号 */
    u8       abyElevation[GPS_SEGMENT_MAX_LEN];    /* ee  仰角,以度为单位 */
    u8       abyAzimuth[GPS_SEGMENT_MAX_LEN];      /* aaa 方位角 */
    u8       abySNR[GPS_SEGMENT_MAX_LEN];          /* ss  SNR信噪比 */
}TSatInfo;

typedef struct{
    u8       abyMsgsNum[GPS_SEGMENT_MAX_LEN];      /* t   本次上报总共有几条GSV消息 */
    u8       abyMsgID[GPS_SEGMENT_MAX_LEN];        /* m   当前该消息的序号 */
    u8       abySatNumInView[GPS_SEGMENT_MAX_LEN]; /* n   可视卫星的总数 */
    TSatInfo atSatInfoGrp[GPS_SAT_GROUPS_PER_GSV]; /*  */
    BOOL32   bValid;                               /* TRUE/FALSE GSV消息有效标志  */
}TInfoGSV;


/*------------------------------  RMC消息结构定义 ----------------------------
  $GPRMC,hhuuss.sss,q,ddmm.mmmm,t,ooonn.nnnn,e,z.zz,g.g,aaxxyy,v,*CC<CR><LF>
 例:$GPRMC,091431.289,A,3111.3360,N,12125.8905,E,0.00,,081105,,*18
 ----------------------------------------------------------------------------
    字段    | 功能介绍                                 | 值的范围,全部是字符
    hh      | Hours (time stamp of position) 时	        0 .. 23
    uu      | Minutes (time stamp of position) 分       0 .. 59
    ss.sss  | Seconds (time stamp of position) 秒.毫秒  0 .. 59.999
    q       | Status                                    A = data valid
            | 数据状态指示                              	V = data Invalid
    dd      | Latitude degrees 纬度,度                   0 .. 90
    mm.mmmm | Latitude minutes 纬度,分                  00.0000 .. 59.9999
    t       | Latitude direction                        N = North北纬
            | 纬度方向                                   S = South南纬
    ooo     | Longitude degrees 经度,度                  0 .. 180
    nn.nnnn | Longitude minutes 经度,分                  00.0000 .. 59.9999
    e       | Longitude direction                       E = East东经
            | 经度方向                                   W = West西经
    z.zz    | Speed Over Ground in knots 地面上速度      单位：节，海里/小时
            |                                           小数点前面长度不定
    g.g     | Course Over Ground in degrees referenced  速度为0时可能为空
            | to true north 地面上行驶方向角，以正北为0度	0.0 .. 359.9
    aa      | Day (time stamp of position) 日           1 .. 31
    xx      | Month (time stamp of position) 月	        1 .. 12
    yy      | Year (time stamp of position) modulo 100年	00 .. 99
    v       | Magnetic Variation                        E = East 实际消息中该项没填为空
            |                                           W = West
    UTC为格林尼制时间,记为T,它是经度为0的地区的绝对时间。
    本地时间算法:东经x度时间=(T+12*x/180)%24; 西经x度时间=(24+T-12*x/180)%24
---------------------------------------------------------------------------*/
typedef struct{
    u8 abyUTC[GPS_SEGMENT_MAX_LEN];             /* hhuuss.sss 格林尼制时间 */
    u8 abyStatus[GPS_SEGMENT_MAX_LEN];          /* q          数据状态指示 */
    u8 abyLatitude[GPS_SEGMENT_MAX_LEN];        /* ddmm.mmmm  纬度 */
    u8 abyLatitudeDir[GPS_SEGMENT_MAX_LEN];     /* t          纬度方向 */
    u8 abyLongitude[GPS_SEGMENT_MAX_LEN];       /* ooonn.nnnn 经度 */
    u8 abyLongitudeDir[GPS_SEGMENT_MAX_LEN];    /* e          经度方向 */
    u8 abySpeedInKnots[GPS_SEGMENT_MAX_LEN];    /* z.zz       速度:海里/小时 */
    u8 abyCourse[GPS_SEGMENT_MAX_LEN];          /* g.g        行驶方向角 */
    u8 abyDate[GPS_SEGMENT_MAX_LEN];            /* aaxxyy     年月日 */
    u8 abyMagneticVar[GPS_SEGMENT_MAX_LEN];     /* v          不详 */
    u8 abyReserved[GPS_SEGMENT_MAX_LEN];        /* NULL reserved */
    BOOL32  bValid;                             /* TRUE/FALSE RMC消息有效标志  */
}TInfoRMC;


/*------------------------------  VTG消息结构定义 ----------------------------
  $GPVTG,a.a,b,c.c,d,e.e,f,g.g,h*CC<CR><LF>
 例:$GPVTG,,T,,M,0.00,N,0.0,K*7E
 ----------------------------------------------------------------------------
    字段    | 功能介绍                                  | 值的范围,全部是字符
    a.a     | Course Over Ground in degrees True 方向角  0 .. 359.9 没有时为空
    b       | Units 单位                                 T = degrees true
    c.c     | Course Over Ground in degrees magnetic     Always null
    d       | Units                                      M = degrees magnetic
    e.e     | Speed Over Ground in knots                 地面速度
            |                                            单位：节，海里/小时
    f       | Units                                      N = Knots 单位：节，海里/小时
    g.g     | Speed Over Ground in km/hr                 地面速度 单位：公里/小时
    h       | Units                                      K = km/hr
---------------------------------------------------------------------------*/
typedef struct{
    u8 abyCourseTrue[GPS_SEGMENT_MAX_LEN];      /* a.a        方向角 */
    u8 abyCourseTrueUnits[GPS_SEGMENT_MAX_LEN]; /* b          T = degrees */
    u8 abyCourseMag[GPS_SEGMENT_MAX_LEN];       /* c.c        Always null */
    u8 abyCourseMagUnits[GPS_SEGMENT_MAX_LEN];  /* d          M = degrees magnetic */
    u8 abySpeedInKnots[GPS_SEGMENT_MAX_LEN];    /* e.e        速度:海里/小时 */
    u8 abySpeedKnotsUnit[GPS_SEGMENT_MAX_LEN];  /* f          N = Knots */
    u8 abySpeedInKmhr[GPS_SEGMENT_MAX_LEN];     /* g.g        速度:公里/小时 */
    u8 abySpeedKmhrUnit[GPS_SEGMENT_MAX_LEN];   /* h          K = km/hr */
    BOOL32  bValid;                             /* TRUE/FALSE VTG消息有效标志  */
}TInfoVTG;


/*  GPS按照NMEA0183协议输出的消息帧以及数据状态结构定义 */
typedef struct{
    s32    nRtnStatus;                   /* 回调时返回的工作状态，见GPS返回值定义
                                            只有该值为GPS_OPT_SUCCESS时下面数据才有效 */
    TInfoGGA tInfoGGA;                   /* GGA消息 */
    TInfoGSA tInfoGSA;                   /* GSA消息 */
    TInfoGSV tInfoGSV[GPS_GSV_MAX_MSGS]; /* GSV消息 */
    TInfoRMC tInfoRMC;                   /* RMC消息 */
    TInfoVTG tInfoVTG;                   /* VTG消息 */
}TGpsOutInfo;


/* 回调函数类型定义 */
typedef void (*TGpsMsgCallBack)(u8 byGpsID, TGpsOutInfo *ptGpsOutInfo);


/* 启动GPS接收器的参数结构定义 */
typedef struct{
    u8           byComPort;           /* 串口号，一般填 #define BRD_SERIAL_RS232 0 */
    u32          dwBautrate;          /* 串口波特率，填0则使用默认值9600，
                                         目前GPS设备要求为9600,最高允许为38400 */
    TGpsMsgCallBack ptMsgCallBack;    /* 回调函数指针 */
}TGpsOpenParam;



/* 函数声明 */
s32 GpsOpen(u8 byGpsID, TGpsOpenParam *ptGpsOpenParam);  /* 打开GPS协议解析模块 */
s32 GpsClose(u8 byGpsID);                                /* 关闭GPS协议解析模块 */
s32 GpsVersion(u8 *pbyBuf, u32 dwBufLen, u32 *pdwVerLen);/* 查询GPS协议解析库版本号 */

#ifdef __cplusplus
}
#endif

#endif /* __INC_GPS_LIB_H */