#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<netdb.h>
#include<stdarg.h>
#include<string.h>
#include<signal.h>

#include "ap_log.h"
#include "osal_api.h"
#include "pktbuff.h"
#include "network_com.h"

static NETWORK_T g_NetWork;

/* 定义一个地址，用于捕获客户端地址 */
struct sockaddr_in g_client_addr[MAX_LINK_COUNT];
socklen_t client_addr_length = sizeof(g_client_addr[0]);

/* 定义一个地址，用于向服务器发送数据 */
struct sockaddr_in g_server_addr;
socklen_t server_addr_length = sizeof(g_server_addr);

static void NETWORK_add_client_addr(struct sockaddr_in *client_addr)
{
	int i = 0;
	for(; i < MAX_LINK_COUNT; i++)
	{
		if(memcmp(&g_client_addr[i], client_addr, sizeof(g_client_addr[0])) == 0)
			return;
	}

	i = 0;
	for(; i < MAX_LINK_COUNT; i++)
	{
		if(g_client_addr[i].sin_addr.s_addr == 0)
		{
			memcpy(&g_client_addr[i], client_addr, sizeof(g_client_addr[0]));
			break;
		}
	}
}

static void NETWORK_remove_client_addr(struct sockaddr_in *client_addr)
{
	int i = 0;
	for(; i < MAX_LINK_COUNT; i++)
	{
		if(memcmp(&g_client_addr[i], client_addr, sizeof(g_client_addr[0])) == 0)
		{
			memset(&g_client_addr[i], 0, sizeof(g_client_addr[0]));
			break;
		}
	}
}

static void NETWORK_add_fd(int fd)
{
	int i = 0;
	for(; i < MAX_LINK_COUNT; i++)
	{
		if(g_NetWork.server_fd[i] == fd)
			return;
	}

	i = 0;
	for(; i < MAX_LINK_COUNT; i++)
	{
		if(g_NetWork.server_fd[i] == -1)
		{
			g_NetWork.server_fd[i] = fd;
			break;
		}
	}
}

static void NETWORK_remove_fd(int fd)
{
	int i = 0;
	for(; i < MAX_LINK_COUNT; i++)
	{
		if(g_NetWork.server_fd[i] == fd)
		{
			g_NetWork.server_fd[i] = -1;
			break;
		}
	}
}

static INT32 NETWORK_handle_info(PPROBE_NODE_INFO probeHdr)
{
	char buf[BUFFER_SIZE] = {0};
	memset(buf, 0, BUFFER_SIZE);
	
	/* 封装buf发送 */
	memcpy(buf, "1234567890", sizeof("1234567890"));
	int len = sizeof("1234567890");

	if(UDP == g_NetWork.type)
	{
		if(CLIENT == g_NetWork.mode || CLIENT_SERVER == g_NetWork.mode)
		{
			int sendlen = sendto(g_NetWork.client_fd, buf, len, 0, (struct sockaddr*)&g_server_addr, sizeof(g_server_addr));
			if (sendlen == -1)
			{
				perror("sendto fail");
				close(g_NetWork.client_fd);
				g_NetWork.client_fd = -1;
				return -1;
			}
		}

		if(SERVER == g_NetWork.mode || CLIENT_SERVER == g_NetWork.mode)
		{
			int i = 0;
			for(; i < MAX_LINK_COUNT; i++)
			{
				if(g_client_addr[i].sin_addr.s_addr != 0)
				{
					int sendlen = sendto(g_NetWork.server_fd[0], buf, len, 0, (struct sockaddr*)&g_client_addr[i], sizeof(g_client_addr[0]));
					if (sendlen == -1)
					{
						perror("send fail");
						return -2;
					}
				}
			}
		}
	}
	else if(TCP == g_NetWork.type)
	{
		if(CLIENT == g_NetWork.mode || CLIENT_SERVER == g_NetWork.mode)
		{
			if(g_NetWork.client_fd >= 0)
			{
				int sendlen = send(g_NetWork.client_fd, buf, len, MSG_NOSIGNAL);
				if (sendlen == -1)
				{
					perror("sendto fail");
					close(g_NetWork.client_fd);
					g_NetWork.client_fd = -1;
					return -1;
				}
			}
		}

		if(SERVER == g_NetWork.mode || CLIENT_SERVER == g_NetWork.mode)
		{
			int i = 0;
			for(; i < MAX_LINK_COUNT; i++)
			{
				if(g_NetWork.server_fd[i] != -1)
				{
					int sendlen = send(g_NetWork.server_fd[i], buf, len, MSG_NOSIGNAL);
					if (sendlen == -1)
					{
						perror("send fail");
						close(g_NetWork.server_fd[i]);
						g_NetWork.server_fd[i] = -1;
						return -2;
					}
				}
			}
		}
	}

	return 0;
}

/* client recv */
void *NETWORK_rx_Client_pthread(void* args)
{
	if(CLIENT != g_NetWork.mode && CLIENT_SERVER != g_NetWork.mode)
		return NULL;

	if(UDP == g_NetWork.type)
	{
		while(1)
		{
			char buf[BUFFER_SIZE] = {0};
			memset(buf, 0, BUFFER_SIZE);
			if(recvfrom(g_NetWork.client_fd, buf, BUFFER_SIZE, 0, (struct sockaddr*)&g_server_addr, &server_addr_length) == -1)
			{
				continue;
			}

			/* 处理buf放入队列 */
			printf("recv: ");
			printf(buf);
			printf("\n");
		}
	}
	else if(TCP == g_NetWork.type)
	{
		while(1)
		{
			char buf[BUFFER_SIZE] = {0};
			memset(buf, 0, BUFFER_SIZE);
			int count = recv(g_NetWork.client_fd, buf, BUFFER_SIZE, 0);
			if(count <= 0)
				continue;

			/* 处理buf放入队列 */
			printf("recv: ");
			printf(buf);
			printf("\n");
		}
	}

	if(g_NetWork.client_fd >= 0)
		close(g_NetWork.client_fd);
}

/* server recv */
void *NETWORK_rx_Server_pthread(void* args)
{
	if(SERVER != g_NetWork.mode && CLIENT_SERVER != g_NetWork.mode)
		return NULL;

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(g_NetWork.listen_port);

	if(UDP == g_NetWork.type)
	{
		/* 创建UDP套接口 */
		int fd = socket(AF_INET, SOCK_DGRAM, 0);
		if(fd < 0)
		{
			perror("Create Socket Failed:");
			exit(1);
		}

		g_NetWork.server_fd[0] = fd;

		/* 绑定套接口 */
		while(-1 == (bind(fd, (struct sockaddr*)&server_addr, sizeof(server_addr))))
		{
			perror("Server Bind Failed:");
			OSAL_TaskDelay(1000);
			continue;
		}

		while(1)
		{
			char buf[BUFFER_SIZE] = {0};
			memset(buf, 0, BUFFER_SIZE);
			struct sockaddr_in tmp_client_addr;
			int count = recvfrom(fd, buf, BUFFER_SIZE, 0, (struct sockaddr*)&tmp_client_addr, &client_addr_length);
			if(count == 0)
			{
				/*fd不变化，g_client_addr变化*/
				NETWORK_remove_client_addr(&tmp_client_addr);
				continue;
			}

			if(count > 0)
			{
				NETWORK_add_client_addr(&tmp_client_addr);

				printf("recv: ");
				printf(buf);
				printf("\n");
				/* 处理buf放入队列 */
			}
		}
	}
	else if(TCP == g_NetWork.type)
	{
		/* 创建TCP套接口 */
 		int listenfd = socket(AF_INET, SOCK_STREAM, 0);
		if(listenfd < 0)
		{
			perror("Create Socket Failed:");
			exit(1);
		}

		/* 绑定套接口 */
		while( bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1 )
		{
			printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
			OSAL_TaskDelay(1000);
			continue;
		}

		/* 监听 */
		while( listen(listenfd, MAX_LINK_COUNT) == -1 )
		{
			printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
			OSAL_TaskDelay(1000);
			continue;
		}

		fd_set readfds;
		FD_ZERO(&readfds);
		/*将服务器socket加入到集合中*/
		FD_SET(listenfd, &readfds);

		while(1)
		{
			/*无限期阻塞，并测试文件描述符变动 */
			int result = select(FD_SETSIZE, &readfds, (fd_set *)0, (fd_set *)0, (struct timeval *) 0);
			if(result < 1)
			{
				perror("select");
				continue;
			}

			int fd = 0;
			for(; fd < FD_SETSIZE; fd++)
			{
				/*找到相关文件描述符*/
				if(FD_ISSET(fd,&readfds))
				{
					if(fd == listenfd)
					{
						int tmpfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
						/*将客户端socket加入到集合中*/
						FD_SET(tmpfd, &readfds);
						NETWORK_add_fd(tmpfd);
						printf("adding client on fd %d\n", tmpfd);
					}
					else
					{
						/*客户端socket中有数据请求时*/
						char buf[BUFFER_SIZE] = {0};
						memset(buf, 0, BUFFER_SIZE);
						int count = recv(fd, buf, BUFFER_SIZE, 0);
						if(count == 0)
						{
							close(fd);
							FD_CLR(fd, &readfds);
							NETWORK_remove_fd(fd);
							continue;
						}

						if(count > 0)
						{
							printf("recv: ");
							printf(buf);
							printf("\n");
							/* 处理buf放入队列 */
						}
					}
				}	
			}
		}

		close(listenfd);
	}

	if(g_NetWork.server_fd >= 0)
		close(g_NetWork.server_fd);

	return NULL;
}

void *NETWORK_tx_pthread(void* args)
{
TX_RESTART:

	/* 创建client socket */
	if(CLIENT == g_NetWork.mode || CLIENT_SERVER == g_NetWork.mode)
	{
		/* 服务端地址 */
		struct sockaddr_in server_addr;
		bzero(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = inet_addr(g_NetWork.server_ip);
		server_addr.sin_port = htons(g_NetWork.server_port);

		if(UDP == g_NetWork.type)
		{
			/* 保存服务端地址 */
			memcpy(&g_server_addr, &server_addr, sizeof(server_addr));

			g_NetWork.client_fd = socket(AF_INET, SOCK_DGRAM, 0);
			if(g_NetWork.client_fd < 0)
			{
				perror("Create Socket Failed:");
				return NULL;
			}
			printf("g_NetWork.client_fd:%d\n", g_NetWork.client_fd);
		}
		else if(TCP == g_NetWork.type)
		{
			g_NetWork.client_fd = socket(AF_INET, SOCK_STREAM, 0);
			if(g_NetWork.client_fd < 0)
			{
				perror("Create Socket Failed:");
				return NULL;
			}
			printf("g_NetWork.client_fd:%d\n", g_NetWork.client_fd);

			while (0 != connect(g_NetWork.client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
	        {
	            perror("connected failed");
				OSAL_TaskDelay(1000);
	            continue;
	        }
		}
	}

	PPROBE_NODE_INFO probeHdr;

#if 0	
	while(1)
	{
		probeHdr = dequeue_probebuff();
		if(probeHdr == NULL)
		{
			OSAL_TaskDelay(20);
			continue;
		}

		/* handle INFO */
		NETWORK_handle_info(probeHdr);
	}
#else
	while(1)
	{
		if(-1 == NETWORK_handle_info(probeHdr))
			goto TX_RESTART;

		OSAL_TaskDelay(1000);
		continue;
	}
#endif

	return NULL;
}

INT32 NETWORK_init(void)
{
	g_NetWork.client_fd = -1;
	int i = 0;
	for(; i < MAX_LINK_COUNT; i++)
	{
		g_NetWork.server_fd[i] = -1;
	}

	memset(g_client_addr, 0, sizeof(g_client_addr));

	return STATUS_SUCCESS;
}

INT32 NETWORK_uninit(void)
{
	if(g_NetWork.client_fd >= 0)
		close(g_NetWork.client_fd);

	if(g_NetWork.server_fd >= 0)
		close(g_NetWork.server_fd);

	return STATUS_SUCCESS;
}

static void usage(void)
{
	fprintf(stderr, "Usage: network [<options> ...]\n\n"
        "Following options are available:\n"
        "        -t <type>        eg: tcp-0 udp-1\n"
        "        -m <mode>        eg: CLIENT-0 SERVER-1 CLIENT_SERVER-2\n"
        "        -d <dest ip>     eg: 192.168.1.10\n"
        "        -p <dest port>   eg: 5678\n");
 
	exit(1);
}

int main(int argc,char **argv)
{
	int ch;
	INT32 retcode;

	while ((ch = getopt(argc, argv,	"t:m:d:p:")) != -1)
	{
		switch (ch)
		{
			case 't':
				g_NetWork.type = atoi(optarg);
				break;
			case 'm':
				g_NetWork.mode = atoi(optarg);
				break;
			case 'd':
				g_NetWork.server_ip = optarg;
				break;
			case 'p':
				g_NetWork.server_port = atoi(optarg);
				g_NetWork.listen_port = atoi(optarg) + 1;
				break;
			default:
				usage();
		}
	}

	NETWORK_init();

	printf("type:%s\nmode:%s\nip:%s\nport:%d\n",
		g_NetWork.type == 0 ? "TCP" : "UDP",
		g_NetWork.mode == 0 ? "Client" : g_NetWork.mode == 1 ? "Server" : "Client && Server",
		g_NetWork.server_ip, g_NetWork.server_port);

	static pthread_t server_rx_thread;
	static pthread_t client_rx_thread;
	static pthread_t network_tx_thread;

	pthread_create(&server_rx_thread, NULL, NETWORK_rx_Server_pthread, NULL);
	pthread_create(&client_rx_thread, NULL, NETWORK_rx_Client_pthread, NULL);
	pthread_create(&network_tx_thread, NULL, NETWORK_tx_pthread, NULL);

	while(1)
	{
		OSAL_TaskDelay(0xffffffff);
		continue;
	}

	pthread_join(server_rx_thread, NULL);
	pthread_join(client_rx_thread, NULL);
	pthread_join(network_tx_thread, NULL);

	NETWORK_uninit();
}

