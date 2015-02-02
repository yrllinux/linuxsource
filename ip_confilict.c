#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/if_arp.h>

s32 find_index(char *ifname)
{
	s32 ifindex;
   	struct ifreq ifr;
	s32 sockfd;
    
    sockfd = socket (AF_INET, SOCK_DGRAM, 0);
   	if (sockfd < 0) {
       	perror ("socket");
        return ERROR;
   	}

	memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

	if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl error:");
       	goto exit;
    }
	ifindex = ifr.ifr_ifindex;

	close(sockfd);
	return ifindex;

exit:
	close(sockfd);
	return ERROR;
}

s32 interface_check(char *ifname)
{
   	struct ifreq ifr;
	s32 sockfd;
	
    sockfd = socket (AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror ("socket");
		return ERROR;
	}

	memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);

    if (ioctl(sockfd, SIOCGIFFLAGS, (char*)&ifr) < 0) {
	perror("ioctl error:");
        perror("Failed to get interface %s flags.\n", ifname);
        goto exit;
    }

    if (0 == (ifr.ifr_flags & IFF_UP)) {
      	goto exit;
    }

    if (0 != (ifr.ifr_flags & IFF_NOARP)) {
        goto exit;
   	}

    if (0 != (ifr.ifr_flags & IFF_LOOPBACK)) {
        goto exit;
    }

	close(sockfd);
	return OK;

exit:
	close(sockfd);
	return 1;

}

void send_gratuitous_arp(char *if_name, u32 ipaddr, u8 *macaddr)
{
    s32 ifindex;
    struct ifreq ifr;	
    int err;
    struct sniff_arp arp = {0};
    struct sockaddr_ll he;
    s32 sock_fd;
	u32 source_ip = ipaddr;	

    memset(&arp ,0 ,sizeof(arp));
    memset(&ifr, 0, sizeof(ifr));

	ifindex = find_index(if_name);
	if(ifindex < 0) {
		return ;
	}

	sock_fd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_ARP));
	if (sock_fd <=0) {
        		perror("Socket failed error!");
        		return;
    	}
    	he.sll_family = AF_PACKET;
    	he.sll_ifindex = ifindex;
    	he.sll_protocol = htons(ETH_P_ARP);
    	he.sll_pkttype = PACKET_BROADCAST;
   		he.sll_hatype  = ARPHRD_ETHER;
    	he.sll_halen = ETHER_ADDR_LEN;
    	memset(he.sll_addr, 0xff, ETHER_ADDR_LEN);

   		/* fill arp packet's head */
    	arp.arp_hrd = htons(ARPHRD_ETHER);
    	arp.arp_pro = htons(ETH_P_IP);
    	arp.arp_hln = ETHER_ADDR_LEN;
    	arp.arp_pln = INET_ADDR_LEN;
    	arp.arp_op  = htons(ARP_REQUEST);

   		/* 
     	填充ARP报文内容, 请求报文格式为:
      	SHA:  我们的MAC地址
      	SIP:  我们的IP地址
       	THA:  00:00:00:00:00:00
       	DIP:  我们的IP地址
    	*/
    	memcpy(arp.arp_sha, macaddr, ETHER_ADDR_LEN);
    	memcpy(arp.arp_sip, &source_ip, INET_ADDR_LEN);
    	memset(arp.arp_tha, 0, ETHER_ADDR_LEN);
    	memcpy(arp.arp_tip, &source_ip, INET_ADDR_LEN);

    	err = sendto(sock_fd, &arp, sizeof(arp), 0, (struct sockaddr*)&he, sizeof(he));
    	if (err != sizeof(arp)) {
        		perror("Failed to send arp packet!");
        		goto exit;
    	}
    

exit:
	close(sock_fd);
	return;
		
}


void reply_gratuitous_arp(char *if_name, u32 ipaddr, u8 *smacaddr, u8 *dmacaddr)
{
    s32 ifindex;
    struct ifreq ifr;	
    int err;
    struct sniff_arp arp = {0};
    struct sockaddr_ll he;
    s32 sock_fd;
	u32 source_ip = ipaddr;	

    memset(&arp ,0 ,sizeof(arp));
    memset(&ifr, 0, sizeof(ifr));

	ifindex = find_index(if_name);
	if(ifindex < 0) {
		return ;
	}

	sock_fd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_ARP));
	if (sock_fd <=0) {
        		perror("Socket failed error!");
        		return;
    }

    he.sll_family = AF_PACKET;
    he.sll_ifindex = ifindex;
    he.sll_protocol = htons(ETH_P_ARP);
    he.sll_pkttype = PACKET_OTHERHOST;
   	he.sll_hatype  = ARPHRD_ETHER;
    he.sll_halen = ETHER_ADDR_LEN;
	/*the header of ethernet is not ff*/	
 	memcpy(he.sll_addr, dmacaddr, ETHER_ADDR_LEN);

   	/* fill arp packet's head */
    arp.arp_hrd = htons(ARPHRD_ETHER);
    arp.arp_pro = htons(ETH_P_IP);
    arp.arp_hln = ETHER_ADDR_LEN;
    arp.arp_pln = INET_ADDR_LEN;
    arp.arp_op  = htons(ARP_REPLY);

    memcpy(arp.arp_sha, smacaddr, ETHER_ADDR_LEN);
    memcpy(arp.arp_sip, &source_ip, INET_ADDR_LEN);
    memcpy(arp.arp_tha, dmacaddr, ETHER_ADDR_LEN);
    memcpy(arp.arp_tip, &source_ip, INET_ADDR_LEN);

    err = sendto(sock_fd, &arp, sizeof(arp), 0, (struct sockaddr*)&he, sizeof(he));
    if (err != sizeof(arp)) {
        perror("Failed to send arp packet!");
        goto exit;
    }


exit:
	close(sock_fd);
	return;
		
}


/*
*arp_is_conflict---detect ip is conflict 
*@if_name:	interface name
*@arp:		arp paceket
*@ipaddr:		ip address
*@mac:		ethernet address
return:
1 : ip confilct 
2:  mac conflict 
0 : don't conflict with other device
*/

s32 arp_ip_is_conflict(char *if_name, u32 ipaddr, u8 *mac, const struct sniff_arp *arp)
{
	u16 arp_op;
    u32  sip;
    u32  tip;
    u8 smac[ETHER_ADDR_LEN + 1] = {0};
    struct in_addr saddr;
	u32 ip = ipaddr;
		
    arp_op = ntohs(arp->arp_op);
    memcpy(&sip, arp->arp_sip, INET_ADDR_LEN);
    memcpy(&tip, arp->arp_tip, INET_ADDR_LEN);

    if((sip == ipaddr) && (tip == ipaddr) && (0 != memcmp(arp->arp_sha, mac, ETHER_ADDR_LEN))) 
	{
       	memcpy(smac, arp->arp_sha, ETHER_ADDR_LEN);
        		
		memset(&saddr, 0, sizeof(saddr));
		saddr.s_addr = sip;
		memcpy(&saddr.s_addr,&ip, INET_ADDR_LEN);

		printf("interface[%s]:Conflicted IP[%s] The other side MAC[%02x:%02x:%02x:%02x:%02x:%02x]\r\n",\
			  if_name, inet_ntoa(saddr), (unsigned char)smac[0], (unsigned char)smac[1], (unsigned char)smac[2],\
			  (unsigned char)smac[3], (unsigned char)smac[4], (unsigned char)smac[5]);    
    
		if (ARP_REQUEST == arp_op) 
		{ 				
			reply_gratuitous_arp(if_name, ipaddr, mac, (u8 *)arp->arp_sha);
        		
		} 
		else if (ARP_REPLY == arp_op) 
		{
			/*keep sending arp packet until don't receive conflicting arp packet*/
			/*legacy bug: shouldn't send too much gratuitous arp packet*/						
			send_gratuitous_arp(if_name, ipaddr, mac);
		}
			
		return IP_CONFLICT_RET;
    }
    else if((sip == 0) && (tip == ipaddr) && (0 != memcmp(arp->arp_sha, mac, ETHER_ADDR_LEN))) 
	{
        memcpy(smac, arp->arp_sha, ETHER_ADDR_LEN);
        
		memset(&saddr, 0, sizeof(saddr));
		saddr.s_addr = tip;
		memcpy(&saddr.s_addr,&ip, INET_ADDR_LEN);	
		
		printf("interface[%s]:Conflicted IP[%s] The other side MAC[%02x:%02x:%02x:%02x:%02x:%02x]\r\n",\
                if_name, inet_ntoa(saddr), (unsigned char)smac[0], (unsigned char)smac[1], (unsigned char)smac[2],\
                (unsigned char)smac[3], (unsigned char)smac[4], (unsigned char)smac[5]);

        if (ARP_REQUEST == arp_op) 
		{ 
            reply_gratuitous_arp(if_name, ipaddr, mac, (u8 *)arp->arp_sha);
       	}  
		else if (ARP_REPLY == arp_op) 
		{
       		/*keep sending arp packet until don't receive conflicting arp packet*/
			/*legacy bug: shouldn't send too much gratuitous arp packet*/
			send_gratuitous_arp(if_name, ipaddr, mac);
		}
        	return IP_CONFLICT_RET;
    }
    else if(0 == memcmp(arp->arp_sha, mac, ETHER_ADDR_LEN)) 
	{
        memcpy(smac, arp->arp_sha, ETHER_ADDR_LEN);
        saddr.s_addr = sip;
		
        printf("interface[%s]:Conflicted MAC[%02x:%02x:%02x:%02x:%02x:%02x]\r\n",\
            if_name, (unsigned char)smac[0], (unsigned char)smac[1], (unsigned char)smac[2],\
            (unsigned char)smac[3], (unsigned char)smac[4], (unsigned char)smac[5]);

		/*keep sending arp packet until don't receive conflicting arp packet*/
        send_gratuitous_arp(if_name, ipaddr, mac);

        return MAC_CONFLICT_RET;
    }

	return IP_NOT_CONFILICT_RET;	
}


s32 ip_conflict_check(void *buf, u32 size)
{
	char dev_name[NETDEV_NAME_LEN];
	u32 status;
	struct ir_ipaddr_show_info ipaddr_show_info;
	u32 ipaddr;
	u8 mac[6];
	s32 i;

	pthread_mutex_lock(&g_netdev_member.netdev_mutex);
	probe_active_netdev(&g_netdev_member);
	pthread_mutex_unlock(&g_netdev_member.netdev_mutex);
		
	if (NETDEV_NAME_LEN < strlen(buf) + 1) 
	{
		perror("the strings is invalid!\n");
		return ERROR;
	}
	strcpy(dev_name, buf);
	ifeth_get_line_status(dev_name, &status);
	if(status ==0) 
	{
		perror("%s:link down!\n", dev_name);
		return OK;
	}
	
	memset(&ipaddr_show_info, 0, sizeof(ipaddr_show_info));
	ipaddr_show_filter(dev_name, &ipaddr_show_info);
	if(str_to_u8_witchout_sepchar(ipaddr_show_info.mac, ':', (char *)mac, 6))
	{
		perror("delete sepeated char error\n");	
		goto error_out;
	}
	
	for(i = 0; i < ipaddr_show_info.ipaddr_param_all.ipnum; i++) 
	{
		/*discard ipv6 address*/
		ipaddr =0;
		if (IPV6_CHECK(ipaddr_show_info.ipaddr_param_all.ipaddr_param[i].ipflag)) 
		{
			continue;
		}
		if ((inet_pton(AF_INET, ipaddr_show_info.ipaddr_param_all.ipaddr_param[i].ipaddr, &ipaddr)) == 0) 
		{
			perror("ipaddr=[%s]\n", ipaddr_show_info.ipaddr_param_all.ipaddr_param[i].ipaddr);
			perror("converts  the  character string src into a network address error");
			goto error_out;
		}
		perror("%s:%d:ipaddr=0x%x\n",__func__, __LINE__,ipaddr);
		send_gratuitous_arp(dev_name, ipaddr, mac);
	}

error_out:
	return 0;
}

/* 检查ARP报文是否正确 */
static s32 validate_arp(const struct sniff_arp *arp)
{
	struct in_addr addr;
    u16 arp_hrd, arp_pro, arp_op;

    arp_hrd = ntohs(arp->arp_hrd);
    if (arp_hrd != 1) 
	{
        return -1;
    }

    arp_pro = ntohs(arp->arp_pro);
    if (arp_pro != 0x800) 
	{
        return -1;
    }

    if (arp->arp_hln != 6 || arp->arp_pln != 4) 
	{
        return -1;
    }

    arp_op = ntohs(arp->arp_op);

    if ((arp_op != ARP_REQUEST) && (arp_op != ARP_REPLY)) 
	{
        printf("Unsupport options, ignored\n");
        return -1;
	}
    memcpy(&addr.s_addr,arp->arp_sip, INET_ADDR_LEN);
    memcpy(&addr.s_addr, arp->arp_tip, INET_ADDR_LEN);

    return 0;
}

/*verify if it is a gratuitous arp packet*/
s32 is_gratuitous_arp(const struct sniff_arp *arp)
{
	u32  sip;
    u32  tip;

    memcpy(&sip, arp->arp_sip, INET_ADDR_LEN);
    memcpy(&tip, arp->arp_tip, INET_ADDR_LEN);
	
	if(sip != tip) 
	{
		return -1;
	}

	return OK;	
}

void receive_net_packet(void)
{
	s32 sock_fd = -1;
	s32 ret = -1, ret2 = -1;
	u8 packet[2048] = {0};
    const struct sniff_arp *arp;              
    struct sockaddr_ll from;
   	socklen_t alen;
	s32 netdev_count = 0, ip_count = 0;
	s32 callback_sync_sem; 
	struct ip_confilct_msg msg;
	
	sock_fd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_ARP));
	if(sock_fd < 0) 
	{
		perror("socket error!");
		return ;
	}
	while(1) 
	{
		memset(packet, 0, 2048);
		ret = recvfrom(sock_fd, packet, sizeof(packet), 0, (struct sockaddr *)&from, &alen);
		if(ret < 0) 
		{
			perror("Failed to recv packet.\n");
			continue;
		}
		if(ret < sizeof(*arp)) 
		{
			continue;
		}

		if ((from.sll_pkttype != PACKET_HOST &&\
             from.sll_pkttype != PACKET_BROADCAST &&\
             from.sll_pkttype != PACKET_MULTICAST &&\
             from.sll_pkttype != PACKET_OTHERHOST &&\
             from.sll_pkttype != PACKET_OUTGOING &&\
             from.sll_hatype  != ARPHRD_ETHER)) 
		{
             continue;
        }

		arp = (struct sniff_arp*)packet;
		if (-1 == validate_arp(arp)) 
		{
			continue;
        }

		if(-1 == is_gratuitous_arp(arp)) 
		{
			continue;
		}

		pthread_mutex_lock(&g_netdev_member.netdev_mutex);
		for (netdev_count = 0; netdev_count < g_netdev_member.netdev_num; netdev_count++) 
		{
			printf("interface =%d/%d,name=%s\n", netdev_count, g_netdev_member.netdev_num,\
				g_netdev_member.netdev_param[netdev_count].netdev_name);

			if(g_netdev_member.netdev_param[netdev_count].if_up ==0 ) 
			{
				continue;
			}
			
			for (ip_count = 0; ip_count < g_netdev_member.netdev_param[netdev_count].ip_num; ip_count++) 
			{
				printf("ip num =%d/%d,name=%s\n", ip_count, g_netdev_member.netdev_param[netdev_count].ip_num,\
						g_netdev_member.netdev_param[netdev_count].netdev_name);

				ret2 = arp_ip_is_conflict(g_netdev_member.netdev_param[netdev_count].netdev_name, \
					g_netdev_member.netdev_param[netdev_count].ipaddr[ip_count],g_netdev_member.netdev_param[netdev_count].mac, arp);
				
				if(IP_CONFLICT_RET == ret2) 
				{
					/*unlock here, forbid to hold lock for a long time*/	
					pthread_mutex_unlock(&g_netdev_member.netdev_mutex);
	
					printf("IP CONFLICT!\n");
					
					callback_sync_sem = semget((key_t)NETCBB_IP_CONFLICT_SEM, 1, 0666 | IPC_CREAT);

					if(callback_sync_sem >= 0) 
					{
						msg.ip = g_netdev_member.netdev_param[netdev_count].ipaddr[ip_count];
						memcpy(msg.mac, arp->arp_sha, ETHER_ADDR_LEN);
						ret = ipc_callback_send_and_recv(IPC_OPENSRC_IP_CONFLICT_PATH, NETCBB_IP_CONFLICT, &msg, sizeof(msg), NULL, 0);
					}
					/*forbid to print too much*/	
					sleep(1);
					break;
				} 
				else if(MAC_CONFLICT_RET == ret2) 
				{
					/*unlock here, forbid to hold lock for a long time*/	
					pthread_mutex_unlock(&g_netdev_member.netdev_mutex);
				
					printf("MAC CONFLICT!\n");

					callback_sync_sem = semget((key_t)NETCBB_IP_CONFLICT_SEM, 1, 0666 | IPC_CREAT);

					if(callback_sync_sem >= 0) 
					{
						msg.ip = g_netdev_member.netdev_param[netdev_count].ipaddr[ip_count];
						memcpy(msg.mac, arp->arp_sha, ETHER_ADDR_LEN);
						ret = ipc_callback_send_and_recv(IPC_OPENSRC_IP_CONFLICT_PATH, NETCBB_MAC_CONFLICT, &msg, sizeof(msg), NULL, 0);
					}
					/*forbid to print too much*/
					sleep(1);
					break;
				} 
				else if (IP_NOT_CONFILICT_RET != ret2) 
				{
					printf("Invalid return:%d\n", ret2);
				}
			}

			pthread_mutex_unlock(&g_netdev_member.netdev_mutex);
			/*if detect one conflict,don't check the other netdevice,go to cycle*/
			if(IP_CONFLICT_RET == ret2 || MAC_CONFLICT_RET == ret2) 
			{
				break;
			}
		}
		
		/*if no network device,don't block*/
		if (netdev_count == 0) 
		{
			pthread_mutex_unlock(&g_netdev_member.netdev_mutex);
		}
	}
}


s32 ip_confilict_init(void) 
{
	pthread_t thread_id;
	int dev_num, ip_num;

	pthread_mutex_lock(&g_netdev_member.netdev_mutex);
	probe_active_netdev(&g_netdev_member);

	for(dev_num=0; dev_num < g_netdev_member.netdev_num; dev_num++)
	{
		for(ip_num=0; ip_num < g_netdev_member.netdev_param[dev_num].ip_num; ip_num++) 
		{
			send_gratuitous_arp(g_netdev_member.netdev_param[dev_num].netdev_name, \
				g_netdev_member.netdev_param[dev_num].ipaddr[ip_num], g_netdev_member.netdev_param[dev_num].mac);
		}
	}

	pthread_mutex_unlock(&g_netdev_member.netdev_mutex);
	
	thread_id = thread_spawn("ip_confilict_recv", (void *)receive_net_packet, \
			NULL, DFT_TH_STACKSIZE, SCHED_OTHER, TH_PRIOR_1);

	if(thread_id < 0) 
	{
		perror("create ip_confilict_recv thread failed!\n");
		return ERROR;
	}

	return OK;
}
