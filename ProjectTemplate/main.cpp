
#include "ClearCore.h"
#include "EthernetTcpServer.h"
#include "mb_tcp_server.h"

#define BUFFER_LEN 512


EthernetTcpServer server(502);

class ModbusTcpServer{
	public:
	ModbusTcpServer()
	{
		//EthernetTcpServer server(502); //bind the server to port
		_server = &server; //set the pointer in this class
	}
	int Begin()
	{
		while (!EthernetMgr.PhyLinkActive()) {
			ConnectorLed.State(_ledState);
			_ledState = !_ledState;
			Delay_ms(100);
		}
		_ledState = false;
		ConnectorLed.State(_ledState); // off until client connected
		
		
		EthernetMgr.LocalIp(IpAddress(192, 168, 10, 177));
		EthernetMgr.GatewayIp(IpAddress(192, 168, 10, 1));
		EthernetMgr.NetmaskIp(IpAddress(255, 255, 255, 0));
		EthernetMgr.Setup();
		_server->Begin(); //start the TCP server
		return 1;
	}
	
	int Poll()
	{
		if(!_server->Ready()){
			return 0;
		}
		if(_client.Connected()){
			if(_client.BytesAvailable()){
				//handle data
				_rx_len = _client.Read(_rx_buffer, BUFFER_LEN);
				HandleData();
				_client.Send(_tx_buffer, _tx_len);
			}
		} else {
			_client = _server->Accept(); //accept and handle on next poll
		}
	}
	
	private:
		bool _ledState = false;
		EthernetTcpServer* _server;
		EthernetTcpClient _client;
		uint8_t _rx_buffer[BUFFER_LEN];
		uint32_t _rx_len; //num of new data bytes
		uint8_t _tx_buffer[BUFFER_LEN];
		uint32_t _tx_len; //num of data bytes to send
	
	
		int HandleData()
		{
			//do modbus stuff here
			//for now just echo back
			_tx_len = _rx_len;
			memcpy(_tx_buffer, _rx_buffer, _tx_len);
			//fill tx buffer
		}
};



ModbusTcpServer mbServer;

int main(void) {
    mbServer.Begin();
    while (1)
    {
		mbServer.Poll();
		Delay_ms(10);
		EthernetMgr.Refresh();
    }
}