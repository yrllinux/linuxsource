#ifndef __SNIFF_PRIVATE_H__
#define __SNIFF_PRIVATE_H__

#include <pcap.h>

/*
 * Maximum snapshot length.  This should be enough to capture the full
 * packet on most network interfaces.
 *
 * XXX - could it be larger?  If so, should it?  Some applications might
 * use the snapshot length in a savefile header to control the size of
 * the buffer they allocate, so a size of, say, 2^31-1 might not work
 * well.
 */
#define MAXIMUM_SNAPLEN	65535

/*
 * The default snapshot length is the maximum.
 */
#define DEFAULT_SNAPLEN	MAXIMUM_SNAPLEN

#ifndef PCAP_ERRBUF_SIZE
#define PCAP_ERRBUF_SIZE 256
#endif

#define IF_MAX_LEN	16
#define DEFAULT_IF_NAME "ath0"

#define PACKET_HANDLE_METHOD_PRINT		1	/* 报文以打印作为处理方法 */


/*
 * For 802.11 cards using the Prism II chips, with a link-layer
 * header including Prism monitor mode information plus an 802.11
 * header.
 */
#ifndef DLT_PRISM_HEADER
#define DLT_PRISM_HEADER	119
#endif

typedef u_int (*packet_handle_f)(const struct pcap_pkthdr *, const u_char *, void *output);

typedef struct __sniff_info
{
	int		iNdoType;
	packet_handle_f		packet_handle;
}SNIFF_INFO_T;

#endif /* __SNIFF_PRIVATE_H__ */