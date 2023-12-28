#include "ClearCore.h"
#include "cc_modbus.h"

Connector *const outputLEDs[6] = {
	&ConnectorIO0, &ConnectorIO1, &ConnectorIO2, &ConnectorIO3, &ConnectorIO4, &ConnectorIO5
};
//ModbusTcpServer server;
ModbusTcpClient mb_client;


void setup()
{
	// Set usingDhcp to false to use user defined network settings
	bool usingDhcp = false;
	// Make sure the physical link is active before continuing
	while (!EthernetMgr.PhyLinkActive()) {
		Delay_ms(100);
	}
	EthernetMgr.Setup();
	//To configure with an IP address assigned via DHCP
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
		IpAddress ip = IpAddress(192, 168, 11, 177);
		EthernetMgr.LocalIp(ip);
		ConnectorUsb.Send("Assigned manual IP address: ");
		// Optionally, set additional network addresses if needed
		IpAddress gateway = IpAddress(192, 168, 11, 1);
		IpAddress netmask = IpAddress(255, 255, 255, 0);
		EthernetMgr.GatewayIp(gateway);
		EthernetMgr.NetmaskIp(netmask);
	}
	for(int i=0; i<6; i++){
		outputLEDs[i]->Mode(Connector::OUTPUT_DIGITAL);
	}
	//server.Begin();
	mb_client.Begin(IpAddress(192, 168, 11, 200));
	
}


void update_leds()
{
	for(int i=0; i<6; i++){
		outputLEDs[i]->State(mb_client.coilRead(i));
	}
	uint32_t num = (Milliseconds() * 2) % 40000;
	//server.inputRegisterWrite(0, num);
}

uint32_t t = Milliseconds();



void modbus_update(){
	if(!mb_client.Connected()){
	} else {
		mb_client.coilWrite(1, !mb_client.coilRead(1));
	}
	
}





int main(void)
{
	setup();
	while(1){
		//server.Poll(); //update modbus
		update_leds();
		Delay_ms(10);
		if ((Milliseconds() - t) > 500){
			t = Milliseconds();
			modbus_update();			
		}
		EthernetMgr.Refresh();		
	}
}