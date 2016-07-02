#ifndef __PARSE_API_H__
#define __PARSE_API_H__

#include <pcap.h>

u_int prism_if_handle(const struct pcap_pkthdr *h, const u_char *p, void *pvOutput);

#endif /* __PARSE_API_H__ */

