/* 
* modbus_tcp_server.h
* Copyright © 2010-2012 Stéphane Raimbault <stephane.raimbault@gmail.com>
* Copyright © 2018 Arduino SA. All rights reserved.
* Copyright © 2023 Adam Solchenberger <asolchenberger@gmail.com>
* SPDX-License-Identifier: LGPL-2.1+
*/


#ifndef __MODBUS_TCP_SERVER_H__
#define __MODBUS_TCP_SERVER_H__

#include "ClearCore.h"
#define SerialPort ConnectorUsb //use for debug
#include "EthernetTcpServer.h"
#include "libmodbus/modbus_tcp.h"
#include "modbus_server.h"

#define NUMBER_OF_CLIENTS 6 // Set total number of clients the server will accept

class ModbusTcpServer : public ModbusServer
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
