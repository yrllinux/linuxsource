#ifndef _common_types_
#define _common_types_

typedef unsigned char                        BOOL;
typedef signed char                          INT8;
typedef short int                            INT16;
typedef long int                             INT32;
typedef unsigned char                        UINT8;
typedef unsigned short int                   UINT16;
typedef unsigned int             		     UINT32;
typedef unsigned int						 SEM_ID;
typedef unsigned int 						 MUX_ID;
typedef unsigned int 						 TASK_ID;
typedef unsigned int 						 QUEUE_ID;

#ifndef NULL              /* pointer to nothing */
   #define NULL ((void *) 0)
#endif

#ifndef TRUE              /* Boolean true */
   #define TRUE (1)
#endif

#ifndef FALSE              /* Boolean false */
   #define FALSE (0)
#endif

typedef void (*FuncPtr_t)(void);

#define COMMON_MAX(x,y) ((x>y)?x:y)
#define COMMON_MIN(x,y) ((x<y)?x:y)

#define UNINITIALIZED       0

#endif
