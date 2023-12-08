/* 
* modbus_tcp_server.cpp
*
* Created: 12/8/2023 8:13:36 AM
* Author: asolchenberger
*/


#include "modbus_tcp_server.h"

// default constructor
ModbusTcpServer::ModbusTcpServer()
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
		IpAddress ip = IpAddress(172, 16, 10, 177);
		EthernetMgr.LocalIp(ip);
		ConnectorUsb.Send("Assigned manual IP address: ");
		// Optionally, set additional network addresses if needed
		IpAddress gateway = IpAddress(172, 16, 10, 1);
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
	// Connect to clients, and send/receive packets

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
				break;
			}
		}
		// Rejects client if the client list is full 
		if(_newClient == 1){
			_newClient = 0;
			_tempClient.Send("This server has reached its max number of clients. Closing connection.");
			_tempClient.Close();
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
			// Check if client has incoming data available	
			if(_clients[i].BytesAvailable()){
				if(HandleRequest(i) == 0){ //handle the request and return bytes sent back to client
					// If the message was unable to send, close the client connection
					_clients[i].Close();
					_clients[i] = EthernetTcpClient();
				}	
			}			
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
}

int ModbusTcpServer::HandleRequest(int client_idx)
{
		// Read packet from the client
	while (_clients[client_idx].BytesAvailable()) {
		// Send the data received from the client over a serial port
		_rx_len = _clients[client_idx].Read(_rx_buffer, BUFFER_LENGTH);
		
		//send to libmodbus here
		//_modbus_receive_msg(_ctx, _rx_buffer, MSG_INDICATION);
		
		// Clear the message buffer for the next iteration of the loop
		for(int i=0; i<BUFFER_LENGTH; i++){
			_rx_buffer[i]=NULL;
		}
	}
	//Sends unique response to the client
	_tx_len = sprintf((char*)_tx_buffer, "Hello client %s",_clients[client_idx].RemoteIp().StringValue());
	return _clients[client_idx].Send(_tx_buffer, _tx_len); //bytes sent
}


int ModbusTcpServer::InitContext()
{
	//_ctx = (modbus_t *)malloc(sizeof(modbus_t));
	//_modbus_init_common(_ctx);
	//modbus_set_slave(_ctx, MODBUS_TCP_SLAVE);
	return 0;
}