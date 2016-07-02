#ifndef _util_h_
#define _util_h_

typedef unsigned long u64;

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

#ifdef __CHECKER__
#define __force __attribute__((force))
#define __bitwise __attribute__((bitwise))
#else
#define __force
#define __bitwise
#endif


typedef u16 __bitwise be16;
typedef u16 __bitwise le16;
typedef u32 __bitwise be32;
typedef u32 __bitwise le32;
typedef u64 __bitwise be64;
typedef u64 __bitwise le64;

#define EXTRACT_16BITS(p) \
	((u_int16_t)ntohs(*(const u_int16_t *)(p)))
#define EXTRACT_32BITS(p) \
	((u_int32_t)ntohl(*(const u_int32_t *)(p)))
#define EXTRACT_64BITS(p) \
	((u_int64_t)(((u_int64_t)ntohl(*((const u_int32_t *)(p) + 0))) << 32 | \
		     ((u_int64_t)ntohl(*((const u_int32_t *)(p) + 1))) << 0))
#define EXTRACT_24BITS(p) \
		((u_int32_t)((u_int32_t)*((const u_int8_t *)(p) + 0) << 16 | \
				 (u_int32_t)*((const u_int8_t *)(p) + 1) << 8 | \
				 (u_int32_t)*((const u_int8_t *)(p) + 2)))
	
/*
 * Macros to extract possibly-unaligned little-endian integral values.
 * XXX - do loads on little-endian machines that support unaligned loads?
 */
#define EXTRACT_LE_8BITS(p) (*(p))
#define EXTRACT_LE_16BITS(p) \
		((u_int16_t)((u_int16_t)*((const u_int8_t *)(p) + 1) << 8 | \
				 (u_int16_t)*((const u_int8_t *)(p) + 0)))
#define EXTRACT_LE_32BITS(p) \
		((u_int32_t)((u_int32_t)*((const u_int8_t *)(p) + 3) << 24 | \
				 (u_int32_t)*((const u_int8_t *)(p) + 2) << 16 | \
				 (u_int32_t)*((const u_int8_t *)(p) + 1) << 8 | \
				 (u_int32_t)*((const u_int8_t *)(p) + 0)))
#define EXTRACT_LE_24BITS(p) \
		((u_int32_t)((u_int32_t)*((const u_int8_t *)(p) + 2) << 16 | \
				 (u_int32_t)*((const u_int8_t *)(p) + 1) << 8 | \
				 (u_int32_t)*((const u_int8_t *)(p) + 0)))
/* BEGIN: Added by kevin, 2015/12/26   PN:monitorÊý¾Ý½ÓÊÕtest */
#define EXTRACT_LE_48BITS(p) \
		((u_int32_t)(u_int64_t)*((const u_int8_t *)(p) + 5) << 40 | \
					(u_int64_t)*((const u_int8_t *)(p) + 4) << 32 | \
					(u_int64_t)*((const u_int8_t *)(p) + 3) << 24 | \
					(u_int64_t)*((const u_int8_t *)(p) + 2) << 16 | \
					(u_int64_t)*((const u_int8_t *)(p) + 1) << 8 | \
					(u_int64_t)*((const u_int8_t *)(p) + 0)))
		
/* END:   Added by kevin, 2015/12/26   PN:monitorÊý¾Ý½ÓÊÕtest */
#define EXTRACT_LE_64BITS(p) \
		((u_int64_t)((u_int64_t)*((const u_int8_t *)(p) + 7) << 56 | \
				 (u_int64_t)*((const u_int8_t *)(p) + 6) << 48 | \
				 (u_int64_t)*((const u_int8_t *)(p) + 5) << 40 | \
				 (u_int64_t)*((const u_int8_t *)(p) + 4) << 32 | \
					 (u_int64_t)*((const u_int8_t *)(p) + 3) << 24 | \
				 (u_int64_t)*((const u_int8_t *)(p) + 2) << 16 | \
				 (u_int64_t)*((const u_int8_t *)(p) + 1) << 8 | \
				 (u_int64_t)*((const u_int8_t *)(p) + 0)))

#define EXTRACT_BE_32BITS(p) \
		((((u_int32_t)*((const u_int8_t *)p + 0)) << 24) | \
		(((u_int32_t)*((const u_int8_t *)p + 1)) << 16) | \
		(((u_int32_t)*((const u_int8_t *)p + 2)) << 8) | \
		((u_int32_t)*((const u_int8_t *)p + 3)))
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/*EXTERN */
extern void *aligned_malloc(int size, int alignment);
extern void *aligned_free(void *paligned);

extern void *osal_malloc(int size);
extern void osal_free(void *ptr);

#endif



