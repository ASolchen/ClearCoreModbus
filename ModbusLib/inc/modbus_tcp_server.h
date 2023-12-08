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
#define MODBUS_TCP_SLAVE 0xFF
#define BUFFER_LENGTH 512
#define MODBUS_TCP_TIMEOUT 5000
//#include "libmodbus/modbus.h"
//#include "libmodbus/modbus-private.h"

class ModbusTcpServer
{
//variables
public:
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
	//modbus_t *_ctx; //modbus context
	

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
