/* 
* modbus_tcp_server.cpp
*
* Created: 12/8/2023 8:13:36 AM
* Author: asolchenberger
*/

#include "modbus_tcp_server.h"

// modbus backend functions


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
    (void)req_length; // do nothing and keep the compiler from complaining about not using
	return (req[0] << 8) + req[1];
}

int send_msg_pre_tcp(uint8_t *req, int req_length)
{
    /* Subbtract the header length to the message length */
    int mbap_length = req_length - 6;

    req[4] = mbap_length >> 8;
    req[5] = mbap_length & 0x00FF;

    return req_length;
}

ssize_t send_tcp(modbus_t *ctx, const uint8_t *req, int req_length)
{
    (void)ctx;
    (void)req;
    return req_length;
	/*
	modbus_tcp_t *ctx_tcp = (modbus_tcp_t*)ctx->backend_data;
	return ctx_tcp->client->write(req, req_length);
	*/
}

int receive_tcp(modbus_t *ctx, uint8_t *req)
{
    return _modbus_receive_msg(ctx, req, MSG_INDICATION);
}

ssize_t recv_tcp(modbus_t *ctx, uint8_t *rsp, int rsp_length)
{
    (void)ctx;
    (void)rsp;
    return rsp_length;
	/*
	modbus_tcp_t *ctx_tcp = (modbus_tcp_t*)ctx->backend_data;
	return ctx_tcp->client->read(rsp, rsp_length);
	*/
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
    (void)ctx;
	return 0;
}

void close_tcp(modbus_t *ctx)
{
    (void)ctx;
}

int flush_tcp(modbus_t *ctx)
{
    (void)ctx;
    return 0;
}

int select_tcp(modbus_t *ctx, fd_set *rset, struct timeval *tv, int msg_length)
{
    (void)ctx;
    (void)rset;
    (void)tv;
    (void)msg_length;
	return 1;
}




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
	return 0;
}

int ModbusTcpServer::HandleRequest(int client_idx)
{
		// Read packet from the client
	while (_clients[client_idx].BytesAvailable()) {
		// Send the data received from the client over a serial port
		_rx_len = _clients[client_idx].Read(_rx_buffer, BUFFER_LENGTH);
		
		//send to libmodbus here
		modbus_reply(_ctx, _rx_buffer, _rx_len, _mb_mapping);
		//_modbus_receive_msg(_ctx, _rx_buffer, MSG_INDICATION);
		
		// Clear the message buffer for the next iteration of the loop
		for(int i=0; i<BUFFER_LENGTH; i++){
			_rx_buffer[i]=(int8_t)NULL;
		}
	}
	//Sends unique response to the client
	_tx_len = sprintf((char*)_tx_buffer, "Hello client %s",_clients[client_idx].RemoteIp().StringValue());
	return _clients[client_idx].Send(_tx_buffer, _tx_len); //bytes sent
}

int ModbusTcpServer::InitContext()
{
	_ctx = new modbus_t;
	_mb_mapping = modbus_mapping_new(128,128,64,64);
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



