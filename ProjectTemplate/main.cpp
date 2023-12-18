/*
 * Title: EthernetTCPServer_manualClientManagement 
 *
 * Objective:
 *    This example demonstrates how to configure a ClearCore as a TCP server to 
 *    send and receive TCP datagrams (packets).
 *    
 * Description:
 *    This example configures a ClearCore device to act as a TCP server. 
 *    This server can receive connections from several other devices acting as TCP 
 *    clients to exchange data over ethernet TCP. 
 *    This simple example accepts connection requests from clients, checks for
 *    incoming data from connected devices, and sends a simple "Hello 
 *    client" response.
 *    A partner project, EthernetTcpClientHelloWorld, can be used to configure 
 *    another ClearCore as a client device.
 *
 * Setup:
 * 1. Set the usingDhcp boolean as appropriate. If not using DHCP, specify static 
 *    IP and network information.
 * 2. Ensure the server and client are setup to communicate on the same network.
 *    If server and client devices are directly connected (as opposed to through a switch)
 *    an ethernet crossover cable may be required. 
 * 3. It may be helpful to use another application to view serial output from 
 *    each device. PuTTY is one such application: https://www.putty.org/
 *
 * Links:
 * ** ClearCore Documentation: https://teknic-inc.github.io/ClearCore-library/
 * ** ClearCore Manual: https://www.teknic.com/files/downloads/clearcore_user_manual.pdf
 * 
 * Copyright (c) 2022 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */
#include "ClearCore.h"
#include "cc_modbus.h"

Connector *const outputLEDs[6] = {
	&ConnectorIO0, &ConnectorIO1, &ConnectorIO2, &ConnectorIO3, &ConnectorIO4, &ConnectorIO5
};
ModbusTcpServer server;


void setup()
{
	for(int i=0; i<6; i++){
		outputLEDs[i]->Mode(Connector::OUTPUT_DIGITAL);
	}
	server.Begin();
	server._mb_mapping->tab_input_registers[2] = 20;
}


void update_leds()
{
	for(int i=0; i<6; i++){
		outputLEDs[i]->State(server._mb_mapping->tab_bits[i]);
	}
	uint32_t num = (Milliseconds() * 2) % 40000;
	memcpy(&server._mb_mapping->tab_input_registers[0], &num, sizeof(uint32_t));
}

int main(void)
{
	setup();
	while(1){
		server.Poll(); //update modbus
		update_leds();
		Delay_ms(50);
	}
}