

#include "cmdtable.h"
#include "cmdprotocol.h"
#include "msgpipe.h"


extern MdcomEnv g_env;

typedef struct
{
    cmd_t id;               //唯一的命令标识
    u8 num;                 //协议中定义的命令序号
    u8 len;                 //包中的数据段长度
    u8 mask1;               //要设置的标志位(目前只用一个，可以扩充)
} ProtlCmdDesp;

ProtlCmdDesp CmdDesps[] = 
{
    { MDCOM_SET_MOD_FREQ,       0x41,   0x24,   0x01},
    { MDCOM_SET_MOD_OUTPUT,     0x41,   0x24,   0x08},
    { MDCOM_SET_MOD_BITRATE,    0x42,   0x14,   0x01},
    { MDCOM_SET_MOD_CLKSRC,     0x42,   0x14,   0x40},
    { MDCOM_SET_DEM_FREQ,       0x81,   0x22,   0x01},
    { MDCOM_SET_DEM_BITRATE,    0x82,   0x1E,   0x01},
    { MDCOM_SET_DEM_CLKSRC,     0x82,   0x1E,   0x40}
};

/* 生成校验和 */
static int GenChecksum(u8 *data)
{
	int i, len = data[6] + 10 - 2;
    int sum = 0;
	for(i = 1; i < len; i++)
	{
		sum += data[i];
		sum %= 256;
	}
	sum = 256 - sum;
	return sum;
}

static void FillSendPack(u8 *data)
{
    int len = data[6] + 10;
    /* 协议包头 */
    data[0] = 0xFF;
    data[1] = 0xA5;
    data[2] = 0x01;
    data[3] = 0x01;
    data[5] = 0x01;
    /* 协议包尾 */
    data[len - 3] = 0x96;
    data[len - 1] = 0xFF;
    data[len - 2] = (u8) GenChecksum(data);
};

/* 根据命令号与内容组装一个协议包 */
int AssemSendPack(cmd_t cmd, u32 val, u8 *data)
{
    u8 idx = cmd & 0x0f;

	if (idx >= MDCOM_SET_MAX)
        return -1;

	/* 调用函数保证缓冲足够大 */
	memset(data, 0, CmdDesps[idx].len + 10);
    data[4] = CmdDesps[idx].num;
    data[7] = CmdDesps[idx].mask1;
    
    /* 对于Query命令，长度为零 */
    if (cmd & 0x0f00)
    {
        data[6] = 0x00;
        FillSendPack(data);
        return 10;
    }
    data[6] = CmdDesps[idx].len;
    
    ////////////////////////////////////////////////////////////
    switch(idx)
    {
    case MDCOM_SET_MOD_FREQ:
        data[15] = (u8)val;                        //freq num
	    data[16] = (u8)(val >> 8);                 //freq num
	    data[17] = (u8)(val >> 16);                //freq num
	    data[18] = (u8)(val >> 24);                //freq num
		break;
    case MDCOM_SET_MOD_OUTPUT:
        data[11] = (u8)val;
		break;
	case MDCOM_SET_MOD_BITRATE:
        data[17] = (u8)val;                        //freq num
	    data[18] = (u8)(val >> 8);                 //freq num
	    data[19] = (u8)(val >> 16);                //freq num
	    data[20] = (u8)(val >> 24);                //freq num
		break;
	case MDCOM_SET_MOD_CLKSRC:
		data[13] = (u8)(val << 4);
		break;
	case MDCOM_SET_DEM_FREQ:
		data[15] = (u8)val;                        //freq num
	    data[16] = (u8)(val >> 8);                 //freq num
	    data[17] = (u8)(val >> 16);                //freq num
	    data[18] = (u8)(val >> 24);                //freq num
		break;
	case MDCOM_SET_DEM_BITRATE:
		data[17] = (u8)val;                        //freq num
	    data[18] = (u8)(val >> 8);                 //freq num
	    data[19] = (u8)(val >> 16);                //freq num
	    data[20] = (u8)(val >> 24);                //freq num
		break;
	case MDCOM_SET_DEM_CLKSRC:
		data[13] = (u8)(val << 4);
		break;
	default:
        debug("Invalid command.\n");
		return -1;
    }
    FillSendPack(data);
    return data[6] + 10;
}

/* 从一个协议包中分解出指定命令的值 */
int DisassemRecvPack(const u8 *buf, int buflen, u32 ip, u16 port)
{
    int idx, curr, sum;
    u32 val;
    InCmd* item;
    cmd_t cmd;
    int errpack;								/* 指示正在解析的包有错误 */				
	int succpack_num = 0;						/* 总共解析出来的包的个数 */
    u8 data[PROTLPACK_MAXSIZE];
    int packlen;

    /* 从缓冲区检索出有效的协议包，并逐个处理 */
    curr = 0;
    while (curr < buflen)
    {        
        if ((buflen - curr) < 10)
        {
            break;
        }
        /* 协议包以0x5A,01,01开头，长度在第7个字节 */
        if ((buf[curr]      != 0x5A) ||
            (buf[curr + 1]  != 0x01) ||
            (buf[curr + 2]  != 0x01) ||
            (buf[curr + 6]   > (buflen - curr - 9)))
        {
            curr++;
            continue;
        }
        packlen = buf[curr + 6] + 9;
        /* 倒数第二个为0x96 */
        if (buf[curr + packlen - 2] != 0x96)
        {
            curr++;
            continue;
        }        
        sum = 0;
        for(idx = 0; idx < packlen; idx++)
        {
            sum += buf[curr + idx];
        }
        
        /* 检查校验和及命令字 */
        if ((sum % 256 != 0) ||
            ((buf[curr + 3] != 0x41) &&
            (buf[curr  + 3] != 0x42) &&
            (buf[curr  + 3] != 0x81) &&
            (buf[curr  + 3] != 0x82)))
        {            
            /*　校验和或命令字不对，丢弃整个包 */
            curr = curr + packlen;
            continue;
        };
		
		/* 通过验证 */
		memcpy(data, &buf[curr], packlen);
        curr = curr + packlen;
		succpack_num++;      

        /* 处理协议包 */
        for (idx = 0; idx < MDCOM_SET_MAX; idx++)
        {
            if (CmdDesps[idx].num != data[3])
            {
                continue;
            }
			errpack = 0;
            cmd = CmdDesps[idx].id;
            /* 检索出所有命令内容 */
            switch(cmd)
            {
            case MDCOM_SET_MOD_FREQ:
                val = data[18];
                val = (val << 8) | data[17];
                val = (val << 8) | data[16];
                val = (val << 8) | data[15];
                if (val == 0)
                {
                    debug("Warning: SET_MOD_FREQ is zero!\n");
                    errpack = -1;
                }
		        break;
            case MDCOM_SET_MOD_OUTPUT:
                val = data[11] & 0x1;
		        break;
	        case MDCOM_SET_MOD_BITRATE:
                val = data[20];
                val = (val << 8) | data[19];
                val = (val << 8) | data[18];
                val = (val << 8) | data[17];
                if (val == 0)
                {
                    debug("Warning: SET_MOD_BITRATE is zero!\n");
                    errpack = -1;
                }
	    	    break;
	        case MDCOM_SET_MOD_CLKSRC:		
                val = data[13] >> 4;
		        break;
	        case MDCOM_SET_DEM_FREQ:
		        val = data[18];
                val = (val << 8) | data[17];
                val = (val << 8) | data[16];
                val = (val << 8) | data[15];
                if (val == 0)
                {
                    debug("Warning: SET_DEM_FREQ is zero!\n");
                    errpack = -1;
                }
		        break;
	        case MDCOM_SET_DEM_BITRATE:
    		    val = data[20];
                val = (val << 8) | data[19];
                val = (val << 8) | data[18];
                val = (val << 8) | data[17];
                if (val == 0)
                {
                    debug("Warning: SET_DEM_BITRATE is zero!\n");
                    errpack = -1;
                }
    		    break;
	        case MDCOM_SET_DEM_CLKSRC:
    	    	val = data[13] >> 4;
		        break;
    	    default:    
                break;
            }
            /* 将结果写入命令表中 */
            if (errpack == 0)
            {
                debug("Receive: %d at %d\n", cmd, GetTick());
                item = LookforCmdTable(g_env.table, ip, port, cmd);
				if (item != NULL)
				{
					item->ret = val;
					item->status = ics_receive;
				}
            }
        }
    }    
    return succpack_num;
}


