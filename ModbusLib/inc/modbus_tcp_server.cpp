/* 
* modbus_tcp_server.cpp
*
* Created: 12/8/2023 8:13:36 AM
* Author: asolchenberger
*/


#include "modbus_tcp_server.h"

// The port number on the server over which packets will be sent/received 
#define PORT_NUM 502

// The maximum number of characters to receive from an incoming packet
#define MAX_PACKET_LENGTH 100
// Buffer for holding received packets
unsigned char packetReceived[MAX_PACKET_LENGTH];
// Buffer for holding packets to send
char packetToSend[MAX_PACKET_LENGTH]; 


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

	// Array of output LEDs to indicate client connections
	// NOTE: only connectors IO0 through IO5 can be configured as digital outputs (LEDs) 
	Connector *const outputLEDs[6] = {
		&ConnectorIO0, &ConnectorIO1, &ConnectorIO2, &ConnectorIO3, &ConnectorIO4, &ConnectorIO5
	};

	// Set up serial communication between ClearCore and PC serial terminal 
	ConnectorUsb.Mode(Connector::USB_CDC);
	ConnectorUsb.Speed(9600);
	ConnectorUsb.PortOpen();
	uint32_t timeout = 5000;
    uint32_t startTime = Milliseconds();
    while (!ConnectorUsb && Milliseconds() - startTime < timeout) {
        continue;
    }

	// Set connectors IO0-IO5 as a digital outputs 
	// When an outputLED's state is true, a LED will light on the
	// ClearCore indicating a successful connection to a client
	for(int i=0; i<6; i++){
	outputLEDs[i]->Mode(Connector::OUTPUT_DIGITAL);
	}
	
	// Make sure the physical link is active before continuing
	while (!EthernetMgr.PhyLinkActive()) {
		ConnectorUsb.SendLine("The Ethernet cable is unplugged...");
		Delay_ms(1000);
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
		
		//IpAddress gateway = IpAddress(192, 168, 1, 1);
		IpAddress netmask = IpAddress(255, 255, 255, 0);
		//EthernetMgr.GatewayIp(gateway);
		EthernetMgr.NetmaskIp(netmask);
	}
	

	// Initialize the ClearCore as a server 
	// Clients connect on specified port (502 by default)
	
	_server = new EthernetTcpServer(PORT_NUM);
	
	// Initialize an array of clients
	// This array holds the information of any client that
	// is currently connected to the server, and is used by the
	// server to interact with these clients
	//EthernetTcpClient clients[NUMBER_OF_CLIENTS];
	// Initialize an temporary client for client management
	EthernetTcpClient tempClient;
	
	bool newClient = 0;

	// Start listening for TCP connections
	_server->Begin();

	ConnectorUsb.SendLine("Server now listening for client connections...");

	// Resets a timer used to display a list of connected clients
	startTime = 0;
	
	// Connect to clients, and send/receive packets
	while(true){
		
		// Obtain a reference to a connected client
		// This function returns a specific client once per connection attempt
		// To maintain a connection with a client using Accept() 
		// some basic client management must be performed
		tempClient = _server->Accept();
		
		// Checks if server.Accept() has returned a new client
		if(tempClient.Connected()){
			newClient = 1; 
			
			for(int i=0; i<NUMBER_OF_CLIENTS; i++){
				//Checks for an available location to store a new client
				if(!_clients[i].Connected()){
					_clients[i].Close();
					_clients[i] = tempClient;
					ConnectorUsb.Send(_clients[i].RemoteIp().StringValue());
					ConnectorUsb.SendLine(" has been connected");
					newClient = 0;
					break;
				}
			}
			// Rejects client if the client list is full 
			if(newClient == 1){
				newClient = 0;
				tempClient.Send("This server has reached its max number of clients. Closing connection.");
				sprintf(packetToSend, "This server has reached its max number of clients. Closing connection to (%s).", tempClient.RemoteIp().StringValue());
				ConnectorUsb.SendLine(packetToSend);
				tempClient.Close();
			}
		}
		
		// Loops through list of clients to receive/send messages
		for(int i=0; i<NUMBER_OF_CLIENTS; i++){
			if (_clients[i].Connected()) {
				
				// Indicate connection on corresponding output LED
				outputLEDs[i]->State(true);
				
				// Check if client has incoming data available	
				if(_clients[i].BytesAvailable()){
					sprintf(packetToSend, "Read the following from the client(%s): ", _clients[i].RemoteIp().StringValue());
					ConnectorUsb.Send(packetToSend);
					// Read packet from the client
					while (_clients[i].BytesAvailable()) {
						// Send the data received from the client over a serial port
						_clients[i].Read(packetReceived, MAX_PACKET_LENGTH);
						ConnectorUsb.Send((char *)packetReceived);

						// Clear the message buffer for the next iteration of the loop
						for(int i=0; i<MAX_PACKET_LENGTH; i++){
							packetReceived[i]=NULL;
						}
					}
					ConnectorUsb.SendLine();
					
					//Sends unique response to the client
					sprintf(packetToSend, "Hello client %s",_clients[i].RemoteIp().StringValue());
					if(_clients[i].Send(packetToSend) == 0){
						// If the message was unable to send, close the client connection
						_clients[i].Close();
						sprintf(packetToSend, "Client (%s) has been removed from client list. ", _clients[i].RemoteIp().StringValue());
						ConnectorUsb.Send(packetToSend);
						_clients[i] = EthernetTcpClient();
						
						// Indicate loss of connection by turning off corresponding LED
						outputLEDs[i]->State(false);
					}

				}
				
			} else{
				// Removes any disconnected clients
				if(_clients[i].RemoteIp() != IpAddress(0,0,0,0)){
					sprintf(packetToSend, "Client (%s) has been removed from client list. ", _clients[i].RemoteIp().StringValue());
					ConnectorUsb.Send(packetToSend);
					_clients[i].Close();
					_clients[i] = EthernetTcpClient();

					// Indicate loss of connection by turning off corresponding LED
					outputLEDs[i]->State(false);
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
		if(Milliseconds()- startTime > delay){
			ConnectorUsb.SendLine("List of current clients: ");
			for(int i=0; i<NUMBER_OF_CLIENTS; i++){
				if(_clients[i].RemoteIp() != IpAddress(0,0,0,0)){
					sprintf(packetToSend, "Client %i = %s", i, _clients[i].RemoteIp().StringValue());
					ConnectorUsb.SendLine(packetToSend);
				}
			}
			startTime = Milliseconds();
		}
		
		// Perform any necessary periodic ethernet updates
		// Must be called regularly when actively using ethernet
		EthernetMgr.Refresh();
	}
} 