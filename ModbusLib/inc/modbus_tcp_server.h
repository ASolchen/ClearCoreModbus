/* 
* modbus_tcp_server.h
*
* Created: 12/8/2023 8:13:36 AM
* Author: asolchenberger
*/


#ifndef __MODBUS_TCP_SERVER_H__
#define __MODBUS_TCP_SERVER_H__

#include "ClearCore.h"
#include "EthernetTcpServer.h"
#define NUMBER_OF_CLIENTS 6 // Set total number of clients the server will accept
#define MODBUS_PORT 502
#define BUFFER_LENGTH 512

class ModbusTcpServer
{
//variables
public:
protected:
private:
	unsigned char _rx_buffer[BUFFER_LENGTH];
	uint16_t _rx_len = 0;
	char _tx_buffer[BUFFER_LENGTH];
	uint16_t _tx_len = 0;
	EthernetTcpServer* _server;
	EthernetTcpClient _tempClient;
	EthernetTcpClient _clients[NUMBER_OF_CLIENTS];
	bool _newClient = 0;
	uint32_t _timeout = 5000;
    uint32_t _startTime = Milliseconds();

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

}; //ModbusTcpServer

#endif //__MODBUS_TCP_SERVER_H__
