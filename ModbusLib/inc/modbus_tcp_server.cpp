/* 
* modbus_tcp_server.cpp
* Copyright © 2010-2012 Stéphane Raimbault <stephane.raimbault@gmail.com>
* Copyright © 2018 Arduino SA. All rights reserved.
* Copyright © 2023 Adam Solchenberger <asolchenberger@gmail.com>
*/

#include "modbus_tcp_server.h"


// default constructor
ModbusTcpServer::ModbusTcpServer() :
_tempClient(NULL)
{
} //ModbusTcpServer

// default destructor
ModbusTcpServer::~ModbusTcpServer()
{
} //~ModbusTcpServer

int ModbusTcpServer::Begin()
{
	// Set usingDhcp to false to use user defined network settings
	bool usingDhcp = false;
    while (!ConnectorUsb && Milliseconds() - _startTime < MODBUS_TCP_TIMEOUT) {
        continue;
    }
	// Make sure the physical link is active before continuing
	while (!EthernetMgr.PhyLinkActive()) {
		Delay_ms(100);
	}
	//To configure with an IP address assigned via DHCP
	EthernetMgr.Setup();
	if (usingDhcp) {
		// Use DHCP to configure the local IP address
		bool dhcpSuccess = EthernetMgr.DhcpBegin();
		if (!dhcpSuccess) {
			while (true) {
				// TCP will not work without a configured IP address
				continue;
			}
		}
	} else {
		// Configure with a manually assigned IP address
		// Set ClearCore's IP address
		IpAddress ip = IpAddress(192, 168, 10, 177);
		EthernetMgr.LocalIp(ip);
		ConnectorUsb.Send("Assigned manual IP address: ");
		// Optionally, set additional network addresses if needed
		IpAddress gateway = IpAddress(192, 168, 10, 1);
		IpAddress netmask = IpAddress(255, 255, 255, 0);
		EthernetMgr.GatewayIp(gateway);
		EthernetMgr.NetmaskIp(netmask);
	}
	// Initialize the ClearCore as a server 
	InitContext();
	// Clients connect on specified port (502 by default)
	_server = new EthernetTcpServer(MODBUS_PORT);
	// Initialize an array of clients
	// This array holds the information of any client that
	// is currently connected to the server, and is used by the
	// server to interact with these clients
	//EthernetTcpClient clients[NUMBER_OF_CLIENTS];
	// Initialize an temporary client for client management
	// Start listening for TCP connections
	_server->Begin();
	// Resets a timer used to display a list of connected clients
	_startTime = Milliseconds();
	// Connect to clients, and send/receive packet
	return 0;
} 

int ModbusTcpServer::AcceptClient()
{
	// Checks if server.Accept() has returned a new client
	if(_tempClient.Connected()){
		_newClient = 1; 
		for(int i=0; i<NUMBER_OF_CLIENTS; i++){
			//Checks for an available location to store a new client
			if(!_clients[i].Connected()){
				_clients[i].Close();
				_clients[i] = _tempClient;
				_newClient = 0;
				char msg[80];
				memset(msg,0x00,80);
				sprintf(msg, "New Client %d", i);
				SerialPort.SendLine(msg, sizeof(msg));
				break;
			}
		}
		// Rejects client if the client list is full 
		if(_newClient == 1){
			_newClient = 0;
			_tempClient.Close();
			SerialPort.SendLine("Client rejected, too many connections");
		}
	}
	return 0;
}

int ModbusTcpServer::Poll()
{
	// Obtain a reference to a connected client
	// This function returns a specific client once per connection attempt
	// To maintain a connection with a client using Accept() 
	// some basic client management must be performed
	_tempClient = _server->Accept();
	AcceptClient();

	// Loops through list of clients to receive/send messages
	for(int i=0; i<NUMBER_OF_CLIENTS; i++){
		if (_clients[i].Connected()) {
			HandleRequest(i);			
		} else{
			// Removes any disconnected clients
			if(_clients[i].RemoteIp() != IpAddress(0,0,0,0)){
				_clients[i].Close();
				_clients[i] = EthernetTcpClient();
			}
		}
	}
	// Make sure the physical link is active before continuing
	while (!EthernetMgr.PhyLinkActive()) {
		Delay_ms(1000);
	}
	// Print out a list of current clients
	if(Milliseconds()- _startTime > 2000){
		//do this every x mSec
		_startTime = Milliseconds();
	}
	// Perform any necessary periodic ethernet updates
	// Must be called regularly when actively using ethernet
	EthernetMgr.Refresh();
	return 0;
}

int ModbusTcpServer::HandleRequest(int client_idx)
{
	modbus_tcp_t *ctx_tcp = (modbus_tcp_t*)_ctx->backend_data;
	ctx_tcp->client = &_clients[client_idx];
	if (ctx_tcp->client != NULL) {
		uint8_t request[MODBUS_TCP_MAX_ADU_LENGTH];
		int requestLength = modbus_receive(_ctx, request);
		if (requestLength > 0) {
			modbus_reply(_ctx, request, requestLength, &_mbMapping);
			return 1; //success
		}
	}
	return 0;	
}

int ModbusTcpServer::InitContext()
{
	_ctx = new modbus_t;
	//_mbMapping = modbus_mapping_new(128,128,64,64);
	configureCoils(0,128);
	configureDiscreteInputs(0,128);
	configureInputRegisters(0,64);
	configureHoldingRegisters(0,64);
	_ctx->backend = new modbus_backend_t{
		_MODBUS_BACKEND_TYPE_TCP,
		_MODBUS_TCP_HEADER_LENGTH,
		_MODBUS_TCP_CHECKSUM_LENGTH,
		MODBUS_TCP_MAX_ADU_LENGTH,
		modbus_set_slave,
		build_request_basis_tcp,
		build_response_basis_tcp,
		prepare_response_tid_tcp,
		send_msg_pre_tcp,
		send_tcp,
		receive_tcp,
		recv_tcp,
		check_integrity_tcp,
		pre_check_confirmation_tcp,
		connect_tcp,
		close_tcp,
		flush_tcp,
		select_tcp,
		modbus_free
	};
	_ctx->backend_data = &_backend_data; //attach tcp data for t_id etc.
	return 0;
}



