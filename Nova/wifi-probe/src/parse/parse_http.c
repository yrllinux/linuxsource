//#ifdef HAVE_CONFIG_H
//#include "config.h"
//#endif

//#include <tcpdump-stdinc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "interface.h"
//#include "addrtoname.h"
//#include "extract.h"

//#include "rpc_auth.h"
//#include "rpc_msg.h"

//#include "nameser.h"
#include "parse_public.h"
#include "parse_http.h"
//#include "probe-statistic.h"

const char * http_methods[] = { "GET", "POST", "HEAD", "PUT", "DELETE", };
const char * http_versions[] = { "HTTP/0.9", "HTTP/1.0", "HTTP/1.1", };

#define STRING_TAOBAO		"taobao"
#define STRING_QQ			"qq"
#define STRING_WEIBO_SINA	"weibo"

enum prob_APP {
	PROB_QQ =0,
	PROB_TAOBAO,
	PROB_WEIBO_SINA,
};

const char * http_valid_host[] = { STRING_TAOBAO, STRING_QQ, STRING_WEIBO_SINA, };

const char *tb_cookie_key[] = { "lgc=", "tracknick=", "_nk_=", "_w_tb_nick=" };
const char *wb_cookie_key[] = { "name%3D", "usre%3D" };

#define INVALID_VALUE		(-1)
#define HTTP_COOKIE_STRING	"Cookie"
#define HTTP_USEAGENT_STRING	"user-Agent"
#define HTTP_HOST_STRING	"Host"
#define HTTP_CONTENT_LEN_STRING 	"Content-Length"
#define HTTP_X_LOG_UID			"X-Log-Uid"
#define HTTP_X_UID				"X-Uid"

char tb_account[32];
char wb_account[32];

#define array_size(x) \
	(sizeof(x) / sizeof(x[0]))

/* Simple strstr() like function that takes len arguments for both haystack and needle. */
static char *strfind(char *haystack, int hslen, const char *needle, int ndlen)
{
	int match = 0;
	int i, j;

	for (i = 0; i < hslen; i++)
	{
		if (haystack[i] == needle[0])
		{
			match = ((ndlen == 1) || ((i + ndlen) <= hslen));

			for (j = 1; (j < ndlen) && ((i + j) < hslen); j++)
			{
				if (haystack[i+j] != needle[j])
				{
					match = 0;
					break;
				}
			}

			if (match)
				return &haystack[i];
		}
	}

	return NULL;
}


static char * http_header_lookup(struct http_request *req,const char *hdrname)
{
	int i;

	foreach_header(i, req->headers)
	{
		if (!strcasecmp(req->headers[i], hdrname))
			return req->headers[i+1];
	}

	return NULL;
}

int http_urldecode(char *buf, int blen, const char *src, int slen)
{
	int i;
	int len = 0;

#define hex(x) \
	(((x) <= '9') ? ((x) - '0') : \
		(((x) <= 'F') ? ((x) - 'A' + 10) : \
			((x) - 'a' + 10)))

	for (i = 0; (i < slen) && (len < blen); i++)
	{
		if (src[i] == '%')
		{			
			if (((i+2) < slen) && isxdigit(src[i+1]) && isxdigit(src[i+2]))
			{
				buf[len++] = (char)(16 * hex(src[i+1]) + hex(src[i+2]));
				i += 2;
			}
			else
			{
				/* Encoding error: it's hard to think of a
				** scenario in which returning an incorrect
				** 'decoding' of the malformed string is
				** preferable to signaling an error condition. */
				#if 0 /* WORSE_IS_BETTER */
				    buf[len++] = '%';
				#else
				    return -2;
				#endif
			}
		}
		else
		{
			buf[len++] = src[i];
		}
	}

	return (i == slen) ? len : -1;
}


int check_host_filter_list(char *host, struct http_request *req)
{

	if((NULL == host) || (NULL == req))
		return 0;
	
	if(NULL != strstr(host, STRING_QQ))
		req->prob_app_idx = PROB_QQ;
	else if((NULL != strstr(host, STRING_WEIBO_SINA)) || (NULL != strstr(host, "sina")))
		req->prob_app_idx = PROB_WEIBO_SINA;
	else if(NULL != strstr(host, STRING_TAOBAO))
		req->prob_app_idx = PROB_TAOBAO;
	else
		return 0;

	return 1;
}

int is_http_host(char *hostname)
{
	if(NULL == hostname)
		return 0;

	if(!strncmp(hostname, HTTP_HOST_STRING, strlen(HTTP_HOST_STRING)))
		return 1;
	
	return 0;

}

int is_http_useragent(char *user_agent)
{
	if(NULL == user_agent)
		return 0;

	if(!strncmp(user_agent, HTTP_USEAGENT_STRING, strlen(HTTP_USEAGENT_STRING)))
		return 1;
	
	return 0;
}

int is_http_cookie(char * cookie)
{
	if(NULL == cookie)
		return 0;

	if(!strncmp(cookie, HTTP_COOKIE_STRING, strlen(HTTP_COOKIE_STRING)))
		return 1;
	
	return 0;
}

#define min(a,b) ((a)>(b)?(b):(a))
int is_http(const char *buf, int rlen)
{
	int i;

	if(rlen == 0)
		return 0;
	
	for (i = 0; i < sizeof(http_methods)/sizeof(http_methods[0]); i++)
		if (!strncmp(buf, http_methods[i], min(rlen, strlen(http_methods[i]))))
			return 1;

	return 0;
}


static int http_header_parse(char *buffer, u_int http_len, struct http_request * req)
{
	char *method  = buffer;
	char *path    = NULL;
	char *version = NULL;

	char *headers = NULL;
	char *hdrname = NULL;
	char *hdrdata = NULL;
	int host_have_chk = 0;
	int i;
	int hdrcount = 0;

	/* terminate initial header line */
	if ((headers = strfind(buffer, http_len, "\r\n", 2)) != NULL)
	{
		buffer[http_len-1] = 0;

		*headers++ = 0;
		*headers++ = 0;

		/* find request path */
		if ((path = strchr(buffer, ' ')) != NULL)
			*path++ = 0;

		/* find http version */
		if ((path != NULL) && ((version = strchr(path, ' ')) != NULL))
			*version++ = 0;

		/* check method */
		if (method && !strcmp(method, "GET"))
			req->method = UH_HTTP_MSG_GET;
		else if (method && !strcmp(method, "POST"))
			req->method = UH_HTTP_MSG_POST;
		else
		{
			/* invalid method */
			return -1;
		}

		/* check path */
		if (!path || !strlen(path))
		{
			/* malformed request */
			return -1;
		}
		else
		{
			req->url = path;
		}

		/* check version */
		if (version && !strcmp(version, "HTTP/1.1"))
			req->version = UH_HTTP_VER_1_1;
		else
		{
			/* unsupported version */
			return -1;
		}

		//printf("HTTP: %s %s %s\n",
		 // http_methods[req->method], req->url, http_versions[req->version]);

		/* process header fields */
		for (i = (int)(headers - buffer); i < http_len; i++)
		{
			/* found eol and have name + value, push out header tuple */
			if (hdrname && hdrdata && (buffer[i] == '\r' || buffer[i] == '\n'))
			{
				buffer[i] = 0;

				/* store */
				if ((hdrcount + 1) < array_size(req->headers))
				{
					//printf("HTTP: %s: %s\n", hdrname, hdrdata);

					if(is_http_host(hdrname)){
						if(check_host_filter_list(hdrdata, req)){
							req->host_idx = hdrcount;
						}
						else
						{
							//printf("not in filter list host name = %s\n", hdrdata);
							//STATISTIC_PROBE_INC(http_not_inlist);
							return -1;
						}
					}
					
					req->headers[hdrcount++] = hdrname;
					req->headers[hdrcount++] = hdrdata;
					
					hdrname = hdrdata = NULL;
				}

				/* too large */
				else
				{
					return -1;
				}
			}

			/* have name but no value and found a colon, start of value */
			else if (hdrname && !hdrdata &&
					 ((i+1) < http_len) && (buffer[i] == ':'))
			{
				buffer[i] = 0;
				hdrdata = &buffer[i+1];

				while ((hdrdata + 1) < (buffer + http_len) && *hdrdata == ' ')
					hdrdata++;
			}

			/* have no name and found [A-Za-z], start of name */
			else if (!hdrname && isalpha(buffer[i]))
			{
				hdrname = &buffer[i];
			}
		}

		/* valid enough */
		return 0;
	}

	return -1;
}

int http_data_parse(char *pData, int len, PACKET_STATISTICS_T *pstStaticOutPut)
{
	int offset;
	int length, cpy_len;
	int fix_len ;
	char tb_id[32];
	
	if(NULL == pData)
		return -1;

	
	//STATISTIC_PROBE_INC(http_data);
	
	offset = 0x1B;
	if(len < offset)
		return -1;
	
	fix_len = strlen("cntaobao");	
	length = pData[offset];

	if((length <= fix_len) || (length > (fix_len + sizeof(tb_id))))
		return -1;
		
	if(strncmp(&pData[offset + 1], "cntaobao", fix_len) == 0)
	{
		if(length < fix_len)
			return -1;

		offset = offset + 1 + fix_len;
		cpy_len = length - fix_len;
		cpy_len = min(cpy_len, sizeof(tb_id));
		memset(tb_id, 0, sizeof(tb_id));
		memcpy(tb_id, &pData[offset], cpy_len);
		tb_id[cpy_len]=0;
		//printf("data tb_id: %s\n", tb_id);
		//memcpy(tb_account, tb_id, sizeof(tb_account));
		pstStaticOutPut->ucGot_tb = DATA_STAT_SUCCESS;
		memcpy(pstStaticOutPut->ucTb, tb_id, sizeof(pstStaticOutPut->ucTb));
		//STATISTIC_PROBE_INC(got_account_tb);
	}
		
	return 0;
	
}

void process_qq(struct http_request* req, PACKET_STATISTICS_T *pstStaticOutPut)
{

}

int get_tb_userid_from_url(char *url, char *tb_id, int maxlen)
{
	char *ptr = NULL, *tmp;
	int len;
	if(!url)
		return 0;

	//printf("url=%p: %s\n", url, url);
	ptr = strstr(url, "uid=");
	//printf("ptr: %p\n", ptr);
	if(NULL != ptr)
	{
		ptr+=strlen("uid=cntaobao");
		
		//printf("ptr1: %p=%s\n", ptr, ptr);
		
		tmp = strchr(ptr, '&');
		if(tmp)
		{
			//printf("tmp: %p\n", tmp);
			len = tmp - ptr;
		}
		else
		{
			len = strlen(ptr);
		}

		len = min(len, maxlen);
		memcpy(tb_id, ptr, len);
		tb_id[len] =0;
		//STATISTIC_PROBE_INC(got_account_tb);

		//printf("tb_id from url: %s\n", tb_id);
		
		return 1;
	}

	return 0;
}



int get_tb_userid_from_cookie(char* pCookie, char *tb_id, int maxlen)
{
	int idx;
	char *cookie;
	char *ptr, *tmp;
	int len;
	int i;

	if(NULL == pCookie)
		return 0;
	
	cookie = pCookie;
	
	//printf("cookie = %s\n", cookie);

	for (i = 0; i < sizeof(tb_cookie_key)/sizeof(tb_cookie_key[0]); i++)
	{
		ptr = strstr(cookie, tb_cookie_key[i]);
		if(ptr)
		{
			ptr +=strlen(tb_cookie_key[i]);
			tmp = strchr(ptr, ';');
			if(tmp)
			{
				len = tmp - ptr;
				ptr[len]=0;

				tmp = strchr(ptr, '%');	/*url encode.chiness name*/
				if(tmp)
				{
					//STATISTIC_PROBE_INC(got_account_tb_chiness);
					return 0;
				}
				
				memcpy(tb_id, ptr, min(len,maxlen));
				tb_id[len] =0;
				//printf("tb_id from %s cookie id: %s\n", tb_cookie_key[i], tb_id);
				//STATISTIC_PROBE_INC(got_account_tb);
				return 1;
			}
		}

	}

	return 0;
	
}


int get_tb_userid_from_content(char *pContent,  char *tb_id, int maxlen)
{
	int idx;
	int content_len;
	char *content,*ptr, *tmp;
	int len;
	

	if(!pContent)
		return 0;

	content = pContent;
	//printf("conten:%p= %s\n", content, content);
	ptr = strstr(content, "userId=");
	if(ptr)
	{
		ptr +=strlen("userId=cntaobao");
		tmp = strchr(ptr, '&');
		if(tmp)

		{
			len = tmp - ptr;
		}
		else
		{
			/*usrid in the end of content*/
			len = strlen(ptr);
		}
		
		len = min(len,maxlen);
		memcpy(tb_id, ptr, len);
		tb_id[len] =0;
		//printf("tb_id from content: %s\n", tb_id);
		//STATISTIC_PROBE_INC(got_account_tb);
		
		return 1;

	}

	return 0;
}

int get_weibo_userid_from_cookie(char* pCookie, char *weibo_id, int maxlen)
{
	int idx;
	char *cookie;
	char *ptr, *tmp;
	int len;
	int i,bytes;
	char tmp_string[64];
	
	if(NULL == pCookie)
		return 0;
	
	cookie = pCookie;
	
	printf("cookie = %s\n", cookie);

	for (i = 0; i < sizeof(wb_cookie_key)/sizeof(wb_cookie_key[0]); i++)
	{
		ptr = strstr(cookie, wb_cookie_key[i]);
		if(ptr)
		{
			ptr +=strlen(wb_cookie_key[i]);
			tmp = strstr(ptr, "%26");
			if(tmp)
			{
				len = tmp - ptr;
				if(len > sizeof(tmp_string))
					return 0;
				bytes = http_urldecode(tmp_string,sizeof(tmp_string),ptr,len);
				if(bytes < 0)
					return 0;
				bytes = http_urldecode(weibo_id, maxlen, tmp_string, bytes);
				if(bytes < 0)
					return 0;
				weibo_id[bytes] =0;
				//printf("weibo from %s cookie id: %s\n", wb_cookie_key[i], weibo_id);
				return 1;
			}
		}

	}

	return 0;
	
}

void process_taobao(struct http_request* req, PACKET_STATISTICS_T *pstStaticOutPut)
{
	char tb_id[32];
	int ret;
	int i, content_len;

	/*
		check url: uid=cntaobao*&
	*/

	ret = get_tb_userid_from_url(req->url, tb_id, sizeof(tb_id));
	if(ret > 0){
		pstStaticOutPut->ucGot_tb = DATA_STAT_SUCCESS;
		memcpy(pstStaticOutPut->ucTb, tb_id, sizeof(pstStaticOutPut->ucTb));
		return;
	}

	foreach_header(i, req->headers){
		if (!strcasecmp(req->headers[i], HTTP_COOKIE_STRING)){
			//printf("cookipe addrs=%p\n", req->headers[i+1]);
			ret = get_tb_userid_from_cookie(req->headers[i+1], tb_id, sizeof(tb_id));
		}
		else if((req->method == UH_HTTP_MSG_POST) && (!strcasecmp(req->headers[i], HTTP_CONTENT_LEN_STRING))){
			
			//printf("content len addrs=%p, len=%d\n", req->headers[i+1], atoi(req->headers[i+1]));
			content_len = atoi(req->headers[i+1]);
			if((content_len <= req->this_content_len) && (content_len > 0)){
				req->content[content_len]=0;
				ret = get_tb_userid_from_content(req->content, tb_id, sizeof(tb_id));
			}
		}

		if(ret > 0){
			pstStaticOutPut->ucGot_tb = DATA_STAT_SUCCESS;
			memcpy(pstStaticOutPut->ucTb, tb_id, sizeof(pstStaticOutPut->ucTb));
			return;
		}
		
	}

	return;
	
}

void process_weibo_sina(struct http_request* req, PACKET_STATISTICS_T *pstStaticOutPut)
{
	char weibo_id[32];
	int ret=0;
	int i, len;

	foreach_header(i, req->headers){
		//printf("%s: %s\n", req->headers[i], req->headers[i+1]);
		if (!strcasecmp(req->headers[i], HTTP_COOKIE_STRING)){
			ret = get_weibo_userid_from_cookie(req->headers[i+1], weibo_id, sizeof(weibo_id));
		}else if((!strcasecmp(req->headers[i], HTTP_X_LOG_UID)) ||
					(!strcasecmp(req->headers[i], HTTP_X_UID))){
			/*get Uid.*/
			len = strlen(req->headers[i+1]);
			len = min(len, sizeof(weibo_id));
			memcpy(weibo_id, req->headers[i+1], len);
			weibo_id[len] = 0;
			//printf("weibo uid=%s\n", weibo_id);
			ret = 1;
		}

		if(ret > 0){
			pstStaticOutPut->ucGot_wb = DATA_STAT_SUCCESS;
			memcpy(pstStaticOutPut->ucWb, weibo_id, sizeof(pstStaticOutPut->ucWb));
			//memcpy(wb_account, weibo_id, sizeof(wb_account));
			return;
		}
	}

}

int http_parse(char *bp, u_int length, PACKET_STATISTICS_T *pstStaticOutPut)
{
	int http_header_len;
	char * ptr = NULL;
	struct http_request req;
	int i;
	
	if(NULL == bp)
		return -1;

	//printf("http length: %d\n", length);

	#if 0
	for(i = 0; i < length; i++){
		if((i != 0) && (i % 16 == 0))
			printf("\n");
		
		printf("%02x ", (u_char)bp[i]);
	}

	printf("\n");
	#endif

	//STATISTIC_PROBE_INC(http_proto);
	
	ptr = strfind(bp, length, "\r\n\r\n", 4);
	if(NULL == ptr){
		//printf("no complete http header\n");
		//STATISTIC_PROBE_INC(http_err_header);
		return -1;
	}

	http_header_len = ptr + 4 - bp;

	memset((void *)&req, 0, sizeof(struct http_request));
	req.cookie_idx = INVALID_VALUE;
	req.host_idx = INVALID_VALUE;
	req.userAgent_idx = INVALID_VALUE;
	req.cookie_idx = INVALID_VALUE;
	req.prob_app_idx = INVALID_VALUE;
	req.content = bp + http_header_len;
	req.this_content_len = length - http_header_len;
	
	//printf("http : t:%d, h:%d, r:%d\n", length, http_header_len, req.this_content_len);
	
	if(http_header_parse(bp, http_header_len, &req) < 0){
		//printf("http header parse failure.\n");
		//STATISTIC_PROBE_INC(http_err_parse);
		return -1;
	}

	
//	foreach_header(i, req.headers){
//		printf("----%s = %s\n", req.headers[i], req.headers[i+1]);
//	}
	
	switch(req.prob_app_idx)
	{
		case PROB_QQ:
			process_qq(&req, pstStaticOutPut);
			break;
		case PROB_WEIBO_SINA:
			process_weibo_sina(&req, pstStaticOutPut);
			break;
		case PROB_TAOBAO:
			process_taobao(&req, pstStaticOutPut);
			break;
		default:
			return -1;
	}
	
	return 0;
}

