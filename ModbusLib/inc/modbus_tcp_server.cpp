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

	// Set up serial communication between ClearCore and PC serial terminal 
	ConnectorUsb.Mode(Connector::USB_CDC);
	ConnectorUsb.Speed(9600);
	ConnectorUsb.PortOpen();
    while (!ConnectorUsb && Milliseconds() - _startTime < _timeout) {
        continue;
    }
	// Set connectors IO0-IO5 as a digital outputs 
	// When an outputLED's state is true, a LED will light on the
	// ClearCore indicating a successful connection to a client
	// Make sure the physical link is active before continuing
	while (!EthernetMgr.PhyLinkActive()) {
		ConnectorUsb.SendLine("The Ethernet cable is unplugged...");
		Delay_ms(100);
	}
	//To configure with an IP address assigned via DHCP
	EthernetMgr.Setup();
	if (usingDhcp) {
		// Use DHCP to configure the local IP address
		bool dhcpSuccess = EthernetMgr.DhcpBegin();
		if (dhcpSuccess) {
			ConnectorUsb.Send("DHCP successfully assigned an IP address: ");
			ConnectorUsb.SendLine(EthernetMgr.LocalIp().StringValue());
		}
		else {
			ConnectorUsb.SendLine("DHCP configuration was unsuccessful!");
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
		ConnectorUsb.SendLine(EthernetMgr.LocalIp().StringValue());
		// Optionally, set additional network addresses if needed
		IpAddress gateway = IpAddress(172, 16, 10, 1);
		IpAddress netmask = IpAddress(255, 255, 255, 0);
		EthernetMgr.GatewayIp(gateway);
		EthernetMgr.NetmaskIp(netmask);
	}
	// Initialize the ClearCore as a server 
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
	ConnectorUsb.SendLine("Server now listening for client connections...");
	// Resets a timer used to display a list of connected clients
	_startTime = 0;
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
				ConnectorUsb.Send(_clients[i].RemoteIp().StringValue());
				ConnectorUsb.SendLine(" has been connected");
				_newClient = 0;
				break;
			}
		}
		// Rejects client if the client list is full 
		if(_newClient == 1){
			_newClient = 0;
			_tempClient.Send("This server has reached its max number of clients. Closing connection.");
			sprintf(_tx_buffer, "This server has reached its max number of clients. Closing connection to (%s).", _tempClient.RemoteIp().StringValue());
			ConnectorUsb.SendLine(_tx_buffer);
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
				sprintf(_tx_buffer, "Read the following from the client(%s): ", _clients[i].RemoteIp().StringValue());
				ConnectorUsb.Send(_tx_buffer);
				// Read packet from the client
				while (_clients[i].BytesAvailable()) {
					// Send the data received from the client over a serial port
					_clients[i].Read(_rx_buffer, BUFFER_LENGTH);
					ConnectorUsb.Send((char *)_rx_buffer);
					// Clear the message buffer for the next iteration of the loop
					for(int i=0; i<BUFFER_LENGTH; i++){
						_rx_buffer[i]=NULL;
					}
				}
				ConnectorUsb.SendLine();
				//Sends unique response to the client
				sprintf(_tx_buffer, "Hello client %s",_clients[i].RemoteIp().StringValue());
				if(_clients[i].Send(_tx_buffer) == 0){
					// If the message was unable to send, close the client connection
					_clients[i].Close();
					sprintf(_tx_buffer, "Client (%s) has been removed from client list. ", _clients[i].RemoteIp().StringValue());
					ConnectorUsb.Send(_tx_buffer);
					_clients[i] = EthernetTcpClient();
				}
			}
		} else{
			// Removes any disconnected clients
			if(_clients[i].RemoteIp() != IpAddress(0,0,0,0)){
				sprintf(_tx_buffer, "Client (%s) has been removed from client list. ", _clients[i].RemoteIp().StringValue());
				ConnectorUsb.Send(_tx_buffer);
				_clients[i].Close();
				_clients[i] = EthernetTcpClient();
			}
		}
	}
	// Make sure the physical link is active before continuing
	while (!EthernetMgr.PhyLinkActive()) {
		ConnectorUsb.SendLine("The Ethernet cable is unplugged...");
		Delay_ms(1000);
	}
	// Print out a list of current clients
	int delay = 5000;
	if(Milliseconds()- _startTime > delay){
		ConnectorUsb.SendLine("List of current clients: ");
		for(int i=0; i<NUMBER_OF_CLIENTS; i++){
			if(_clients[i].RemoteIp() != IpAddress(0,0,0,0)){
				sprintf(_tx_buffer, "Client %i = %s", i, _clients[i].RemoteIp().StringValue());
				ConnectorUsb.SendLine(_tx_buffer);
			}
		}
		_startTime = Milliseconds();
	}
	// Perform any necessary periodic ethernet updates
	// Must be called regularly when actively using ethernet
	EthernetMgr.Refresh();
}