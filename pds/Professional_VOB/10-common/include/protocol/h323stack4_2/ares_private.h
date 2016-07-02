/* $Id: ares_private.h,v 1.3 1998/09/22 01:46:11 ghudson Exp $ */

/* Copyright 1998 by the Massachusetts Institute of Technology.
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 * M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

#include <rvstdio.h>

typedef struct {
    RvSelectFd fd;
    RvSocketProtocol protocol;
    int server;
    RvDnsEngine *channel;
} select_fd_args;

struct server_state {
    RvAddress addr;
    select_fd_args udp_socket;
    select_fd_args tcp_socket;
    
    /* Mini-buffer for reading the length word & header*/
    unsigned char tcp_header[2+HFIXEDSZ];
    int tcp_header_pos;
    struct query *query;
    int tcp_length;
    
    /* Buffer for reading actual TCP data */
    unsigned char *tcp_buffer;
    int tcp_buffer_pos;
    
    /* TCP output queue */
    struct query *qhead;
    struct query *qtail;
};

struct query {
    RvInt64 timeout;
    unsigned short qid;    /* Query ID from qbuf, for faster lookup, and current timeout */
    RvTimer timer;

    /* User callback to handle replies */
    rvAresCallback user_callback;
    void *user_arg;
    void *mem_arg;
    
    /* Query status */
    int try;
    int server;
    int *skip_server;
    int using_tcp;
    int error_status;
    
    /* Arguments passed to ares_send() */
    char *qbuf;
    int qlen;
    
    /* Next query in chain */
    struct query *qnext;

    /* TCP output queue */
    char *tcp_data;
    int tcp_len;
    struct query *tcp_next;
};

/* An IP address pattern; matches an IP address X if X & mask == addr */
struct apattern {
    struct in_addr addr;
    struct in_addr mask;
};

int ares__send_query(RvDnsEngine *channel, struct query *query, RvInt64 now);
void ares__close_sockets(RvDnsEngine *channel, int i);
int ares__read_line(FILE *fp, char **buf, int *bufsize);
