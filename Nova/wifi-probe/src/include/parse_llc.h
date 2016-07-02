#ifndef __PARSE_LLC_H__
#define __PARSE_LLC_H__

#define	LLC_U_FMT	3
#define	LLC_GSAP	1
#define	LLC_IG	    1 /* Individual / Group */

#ifndef LLCSAP_SNAP
#define	LLCSAP_SNAP		0xaa
#endif

#define	LLC_UI			0x03
#define OUI_ENCAP_ETHER 0x000000        /* encapsulated Ethernet */
#define OUI_CISCO_90    0x0000f8        /* Cisco bridging */

#ifndef ETHERTYPE_IP
#define	ETHERTYPE_IP		0x0800	/* IP protocol */
#endif

#ifndef ETHERTYPE_IPV6
#define ETHERTYPE_IPV6		0x86dd	/* IP6 protocol */
#endif


#endif /* __PARSE_LLC_H__ */

