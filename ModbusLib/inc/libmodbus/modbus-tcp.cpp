/*
 * modbus_tcp.cpp
* Copyright © 2010-2012 Stéphane Raimbault <stephane.raimbault@gmail.com>
* Copyright © 2018 Arduino SA. All rights reserved.
* Copyright © 2023 Adam Solchenberger <asolchenberger@gmail.com>
* SPDX-License-Identifier: LGPL-2.1+
*/

// modbus backend functions

#include "modbus_tcp.h"

int build_request_basis_tcp(modbus_t *ctx, int function, int addr, int nb, uint8_t *req)
{
    modbus_tcp_t *ctx_tcp = (modbus_tcp_t*)ctx->backend_data;

    /* Increase transaction ID */
    if (ctx_tcp->t_id < UINT16_MAX)
        ctx_tcp->t_id++;
    else
        ctx_tcp->t_id = 0;
    req[0] = ctx_tcp->t_id >> 8;
    req[1] = ctx_tcp->t_id & 0x00ff;

    /* Protocol Modbus */
    req[2] = 0;
    req[3] = 0;

    /* Length will be defined later by set_req_length_tcp at offsets 4
       and 5 */

    req[6] = ctx->slave;
    req[7] = function;
    req[8] = addr >> 8;
    req[9] = addr & 0x00ff;
    req[10] = nb >> 8;
    req[11] = nb & 0x00ff;

    return _MODBUS_TCP_PRESET_REQ_LENGTH;
}

int build_response_basis_tcp(sft_t *sft, uint8_t *rsp)
{
    /* Extract from MODBUS Messaging on TCP/IP Implementation
       Guide V1.0b (page 23/46):
       The transaction identifier is used to associate the future
       response with the request. */
    rsp[0] = sft->t_id >> 8;
    rsp[1] = sft->t_id & 0x00ff;

    /* Protocol Modbus */
    rsp[2] = 0;
    rsp[3] = 0;

    /* Length will be set later by send_msg (4 and 5) */

    /* The slave ID is copied from the indication */
    rsp[6] = sft->slave;
    rsp[7] = sft->function;

    return _MODBUS_TCP_PRESET_RSP_LENGTH;
}

int prepare_response_tid_tcp(const uint8_t *req, int *req_length)
{
    (void)req_length;
	return (req[0] << 8) + req[1];
}

int send_msg_pre_tcp(uint8_t *req, int req_length)
{
    /* Subtract the header length to the message length */
    int mbap_length = req_length - 6;

    req[4] = mbap_length >> 8;
    req[5] = mbap_length & 0x00FF;

    return req_length;
}

ssize_t send_tcp(modbus_t *ctx, const uint8_t *req, int req_length)
{
	modbus_tcp_t *ctx_tcp = (modbus_tcp_t*)ctx->backend_data;
	return ctx_tcp->client->Send(req, req_length);
}

int receive_tcp(modbus_t *ctx, uint8_t *req)
{
    return _modbus_receive_msg(ctx, req, MSG_INDICATION);
}

ssize_t recv_tcp(modbus_t *ctx, uint8_t *rsp, int rsp_length)
{
	modbus_tcp_t *ctx_tcp = (modbus_tcp_t*)ctx->backend_data;
	return ctx_tcp->client->Read(rsp, rsp_length);
}

int check_integrity_tcp(modbus_t *ctx, uint8_t *msg, const int msg_length)
{
    (void)ctx;
    (void)msg;
    return msg_length;
}

int pre_check_confirmation_tcp(modbus_t *ctx, const uint8_t *req, const uint8_t *rsp, int rsp_length)
{
    (void)rsp_length;
    /* Check transaction ID */
    if (req[0] != rsp[0] || req[1] != rsp[1]) {
	    if (ctx->debug) {
		    //fprintf(stderr, "Invalid transaction ID received 0x%X (not 0x%X)\n",
		    //(rsp[0] << 8) + rsp[1], (req[0] << 8) + req[1]);
	    }
	    //errno = EMBBADDATA;
	    return -1;
    }

    /* Check protocol ID */
    if (rsp[2] != 0x0 && rsp[3] != 0x0) {
	    if (ctx->debug) {
		    //fprintf(stderr, "Invalid protocol ID received 0x%X (not 0x0)\n",
		    //(rsp[2] << 8) + rsp[3]);
	    }
	    //errno = EMBBADDATA;
	    return -1;
    }

    return 0;
}

int connect_tcp(modbus_t *ctx)
{
    modbus_tcp_t *ctx_tcp = (modbus_tcp_t*)ctx->backend_data;
    if (!ctx_tcp->client->Connect(ctx_tcp->ip, ctx_tcp->port)) {
	    return -1;
	}
	return 0;
}

void close_tcp(modbus_t *ctx)
{
        modbus_tcp_t *ctx_tcp = (modbus_tcp_t*)ctx->backend_data;
        if(ctx_tcp && ctx_tcp->client) {
	        ctx_tcp->client->Close();
        }
}

int flush_tcp(modbus_t *ctx)
{
    modbus_tcp_t *ctx_tcp = (modbus_tcp_t*)ctx->backend_data;
    ctx_tcp->client->FlushInput();
    return 0;
}

int select_tcp(modbus_t *ctx, fd_set *rset, struct timeval *tv, int length_to_read)
{
	//this seems to be a 'wait' function to see if data is coming in, probably meant for RTU?
    int s_rc;
	(void)rset;
    modbus_tcp_t *ctx_tcp = (modbus_tcp_t*)ctx->backend_data;
    unsigned long wait_time_millis = (tv == NULL) ? 0 : (tv->tv_sec * 1000) + (tv->tv_usec / 1000);
    unsigned long start = Milliseconds();
    do {
	    s_rc = ctx_tcp->client->BytesAvailable();
	    if (s_rc >= length_to_read) {
		    break;
	    }
    } while ((Milliseconds() - start) < wait_time_millis && ctx_tcp->client->Connected());
	if (s_rc == 0) {
		//errno = ETIMEDOUT;
		return -1;
	}
	return s_rc;
}