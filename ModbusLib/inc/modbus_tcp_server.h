/* 
* modbus_tcp_server.h
* Copyright � 2010-2012 St�phane Raimbault <stephane.raimbault@gmail.com>
* Copyright � 2018 Arduino SA. All rights reserved.
* Copyright � 2023 Adam Solchenberger <asolchenberger@gmail.com>
* SPDX-License-Identifier: LGPL-2.1+
*/


#ifndef __MODBUS_TCP_SERVER_H__
#define __MODBUS_TCP_SERVER_H__

#include "ClearCore.h"
#include "EthernetTcpServer.h"
#include "libmodbus/modbus.h"
#include "libmodbus/modbus-private.h"

#define NUMBER_OF_CLIENTS 6 // Set total number of clients the server will accept
#define MODBUS_PORT 502
#define MODBUS_TCP_SLAVE 0xFF
#define BUFFER_LENGTH 512
#define MODBUS_TCP_TIMEOUT 5000
#define MODBUS_TCP_MAX_ADU_LENGTH  260
#define _MODBUS_TCP_HEADER_LENGTH      7
#define _MODBUS_TCP_PRESET_REQ_LENGTH 12
#define _MODBUS_TCP_PRESET_RSP_LENGTH  8
#define _MODBUS_TCP_CHECKSUM_LENGTH    0








// modbus backend functions
int build_request_basis_tcp(modbus_t *ctx, int function, int addr, int nb, uint8_t *req);
int build_response_basis_tcp(sft_t *sft, uint8_t *rsp);
int prepare_response_tid_tcp(const uint8_t *req, int *req_length);
int send_msg_pre_tcp(uint8_t *req, int req_length);
ssize_t send_tcp(modbus_t *ctx, const uint8_t *req, int req_length);
int receive_tcp(modbus_t *ctx, uint8_t *req);
ssize_t recv_tcp(modbus_t *ctx, uint8_t *rsp, int rsp_length);
int check_integrity_tcp(modbus_t *ctx, uint8_t *msg, const int msg_length);
int pre_check_confirmation_tcp(modbus_t *ctx, const uint8_t *req, const uint8_t *rsp, int rsp_length);
int connect_tcp(modbus_t *ctx);
void close_tcp(modbus_t *ctx);
int flush_tcp(modbus_t *ctx);
int select_tcp(modbus_t *ctx, fd_set *rset, struct timeval *tv, int length_to_read);

typedef struct _modbus_tcp {
    /* Extract from MODBUS Messaging on TCP/IP Implementation Guide V1.0b
       (page 23/46):
       The transaction identifier is used to associate the future response
       with the request. This identifier is unique on each TCP connection. */
    EthernetTcpServer* server;
    EthernetTcpClient* client;
	IpAddress			ip = IpAddress(0,0,0,0);
	uint16_t			port = MODBUS_PORT;
    uint16_t			t_id;
	//may need to keep track of clients
} modbus_tcp_t;



class ModbusTcpServer
{
//variables
public:
	modbus_mapping_t* _mb_mapping; //modbus context
protected:
private:
	uint8_t _rx_buffer[BUFFER_LENGTH];
	uint16_t _rx_len = 0;
	uint8_t _tx_buffer[BUFFER_LENGTH];
	uint16_t _tx_len = 0;
	EthernetTcpServer* _server;
	EthernetTcpClient _tempClient;
	EthernetTcpClient _clients[NUMBER_OF_CLIENTS];
	bool _newClient = 0;
    uint32_t _startTime;
	modbus_backend_t _backend;
	modbus_tcp_t _backend_data;
	modbus_t* _ctx; //modbus context
	
		

//functions
public:
	ModbusTcpServer();
	~ModbusTcpServer();
	int Begin();
	int Poll();
	
protected:
private:
	ModbusTcpServer( const ModbusTcpServer &c );
	ModbusTcpServer& operator=( const ModbusTcpServer &c );
	int AcceptClient();
	int HandleRequest(int);
	int InitContext();

}; //ModbusTcpServer

#endif //__MODBUS_TCP_SERVER_H__
