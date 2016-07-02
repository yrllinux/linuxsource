#if !defined (__NMS_DATA_CONVERT_H__)
#define __NMS_DATA_CONVERT_H__


/*=============================================================================
函 数 名:NmsHex2Asc
功    能:把一个16进制的数（0~9,a~f）转换为字符, 如0x0->'0',...,0x0f->'f'
参    数:u8 byHex                           [in]    待转换的BIN
         u8 *pbyAsc                         [out]   结果
注    意:无
返 回 值:是否成功
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/11/09  4.0     王昊    代码规范化
=============================================================================*/
inline BOOL32 NmsHex2Asc(u8 byHex, u8 *pbyAsc)
{
    if ( pbyAsc == NULL || byHex < 0 || byHex > 0x0f )
    {
        return FALSE;
    }

    if ( byHex < 10 )
    {
        *pbyAsc = byHex + '0';
    }
    else
    {
        *pbyAsc = byHex - 10 + 'A';
    }

    return TRUE;
}

/*=============================================================================
函 数 名:NmsAsc2Hex
功    能:把两个字符转为一个16进制的值, 把1~f压缩成一个字节，如7f->0x7f
参    数:const u8 *pbyAsc                   [in]    待转换的ASC
         u8 *pbyHex                         [out]   结果
注    意:无
返 回 值:是否成功, 如果转换失败，返回FALSE, 如果有字符不在0~9,a~f之间，返回FLASE
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/11/09  4.0     王昊    代码规范化
=============================================================================*/
inline BOOL32 NmsAsc2Hex(const u8 *pbyAsc, u8 *pbyHex)
{
    if ( pbyHex == NULL || pbyAsc == NULL )
    {
        return FALSE;
    }

    u8 byLow    = 0;
    u8 byHigh   = 0;

    if ( pbyAsc[0] >= '0' && pbyAsc[0] <= '9' )
    {
        byLow = pbyAsc[0] - '0';
    }
    else if ( pbyAsc[0] >= 'a' && pbyAsc[0] <= 'f' )
    {
        byLow = pbyAsc[0] - 'a' + 10;
    }
    else if ( pbyAsc[0] >= 'A' && pbyAsc[0] <= 'F' )
    {
        byLow = pbyAsc[0] - 'A' + 10;
    }
    else
    {
        return FALSE;
    }

    if ( pbyAsc[1] >= '0' && pbyAsc[1] <= '9' )
    {
        byHigh = pbyAsc[1] - '0';
    }
    else if ( pbyAsc[1] >= 'a' && pbyAsc[1] <= 'f' )
    {
        byHigh = pbyAsc[1] - 'a' + 10;
    }
    else if ( pbyAsc[1] >= 'A' && pbyAsc[1] <= 'F' )
    {
        byHigh = pbyAsc[1] - 'A' + 10;
    }
    else
    {
        return FALSE;
    }

    *pbyHex = (byLow << 4 | byHigh );

    return TRUE;
}

/*=============================================================================
函 数 名:NmsDataHex2Asc
功    能:把一个16进制的内存值转换为字符串, 如0x72a9->"72a9"
参    数:const u8 *pbyHex                   [in]    待转换的HEX码
         s32 nHexlen                        [in]    待转换的HEX码长度
         u8 *pbyAsc                         [out]   转换后的结果
注    意:长度是原来的两倍，即把一个字节转为2个字节
返 回 值:是否成功, 如果转换失败，返回FALSE, 如果有字符不在0~9,a~f之间，返回FLASE
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/11/09  4.0     王昊    代码规范化
=============================================================================*/
inline BOOL32 NmsDataHex2Asc(const u8 *pbyHex, s32 nHexlen, u8 *pbyAsc)
{
    u8 byLow    = 0;
    u8 byHigh   = 0;

    for ( s32 nIndex = 0; nIndex < nHexlen; nIndex ++ )
    {
        byLow = (pbyHex[nIndex] & 0xf0) >> 4;
        byHigh = (pbyHex[nIndex] & 0x0f);

        if ( ! NmsHex2Asc( byLow, &pbyAsc[2 * nIndex] ) )
        {
            return FALSE;
        }
        if ( ! NmsHex2Asc( byHigh, &pbyAsc[2 * nIndex + 1] ) )
        {
            return FALSE;
        }
    }

    return TRUE;
}


/*=============================================================================
函 数 名:NmsDataAsc2Hex
功    能:把一个字符串以16进制形式转为内存，如把"98df13"->0x98df13
参    数:const u8 *pbyAsc                   [in]    待转换的ASC码
         u8 *pbyHex                         [out]   输出：转换后的结果
         s32 nHexLen                        [in]    待转换的ASC码长度
注    意:长度是原来的一半，字符串的长度必须是2的倍数
返 回 值:是否成功, 如果字符串中有不在'0'~'9'，'a'~'f','A'~'F'之间的，则返回FALSE
-------------------------------------------------------------------------------
修改纪录:
日      期  版本    修改人  修改内容
2005/11/09  4.0     王昊    代码规范化
=============================================================================*/
inline BOOL32 NmsDataAsc2Hex(const u8 *pbyAsc, u8 *pbyHex, s32 nHexLen)
{
    if ( nHexLen % 2 != 0 )     //  必须是偶数个字符
    {
        return FALSE;
    }

    for ( s32 nIndex = 0; nIndex < nHexLen / 2; nIndex ++ )
    {
        if ( ! NmsAsc2Hex( pbyAsc + 2 * nIndex, pbyHex + nIndex ) )
        {
            return FALSE;
        }
    }

    return TRUE;
}

#endif // __NMS_DATA_CONVERT_H__
