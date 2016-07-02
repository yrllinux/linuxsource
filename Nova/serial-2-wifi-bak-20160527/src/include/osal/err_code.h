#ifndef _err_code_
#define _err_code_


typedef enum{
    
    OSAL_SUCCESS                = 0,
    OSAL_ERROR                  =-1,
    OSAL_INVALID_POINTER        =-2,
    OSAL_ERR_NAME_TOO_LONG      =-3,
    OSAL_ERR_INVALID_PRIORITY   =-4,
    OSAL_ERR_NO_FREE_IDS        =-5,
    OSAL_ERR_NAME_TAKEN         =-6,
    OSAL_ERR_NO_MEMORY          =-7,
    OSAL_ERR_STACKSIZE_TOO_LOW  =-8,
    OSAL_ERR_INVALID_ID         =-9,
    OSAL_SEM_FAILURE            =-10,
    OSAL_SEM_TIMEOUT            =-11,
    OSAL_NOT_SUPPORT            =-12,
	OSAL_INVALID_SIZE 			=-13,
	OSAL_QUEUE_FULL				=-14,
	OSAL_QUEUE_INVALID_SIZE		=-15,
	OSAL_QUEUE_EMPTY			=-16,
	OSAL_QUEUE_TIMEOUT			=-17,
	OSAL_ERR_NAME_NOT_FOUND		=-18,
	OSAL_ERR_BUFFER_OVERWRITE	=-19,
	OSAL_ERR_BUFFER_REFREE		=-20,
	OSAL_TIMER_STOP_NOUSED		=-21,
	OSAL_INVALID_EXPIRE_ARGS	=-22,
	OSAL_TIMER_DEL_NOSTOP		=-23,

    OSAL_END
}OSAL_ERR_CODE;

typedef enum{
    MSG_SUCCESS                = 0,
    MSG_ERROR                  =-1,
    MSG_INVALID_LEN			   =-2,
    MSG_INVALID_CRC			   =-3,
    MSG_INVALID_POINTER		   =-4,
	MSG_INVALID_MSGID	   	   =-5,
    
    MESSAGE_END
}MSG_ERR_CODE;

typedef enum{
	NETWORK_SUCCESS				= 0,
	NETWORK_CLIENT_LINK_ERROR	=-1,
	NETWORK_SERVER_LINK_ERROR	=-2,
	
	NETWORK_END
}NETWORK_ERR_CODE;

typedef enum{
	STATUS_SUCCESS				= 0,
	STATUS_ERROR				=-1,
	
	STATUS_END
}SERIAL2WIFI_ERR_CODE;

#endif
