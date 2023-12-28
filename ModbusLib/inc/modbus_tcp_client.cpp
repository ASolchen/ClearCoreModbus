/*
  This file is part of the ArduinoModbus library.
  Copyright (c) 2018 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "modbus_tcp_client.h"

ModbusTcpClient::ModbusTcpClient(unsigned long defaultTimeout)
  : ModbusClient(defaultTimeout)
{
}

ModbusTcpClient::~ModbusTcpClient()
{
}

int ModbusTcpClient::Begin(IpAddress ip, uint16_t port)
{
  EthernetTcpClient c;
  _client = &c;
  _ip = ip;
  _port = port;
  modbus_t* mb = modbus_new_tcp(_client, _ip, _port);
  return ModbusClient::begin(mb, MODBUS_TCP_SLAVE);
}

int ModbusTcpClient::Connected()
{
  if(!EthernetMgr.PhyLinkActive()){
	  return 0;
  }
  bool status = 0;
  if(_client->Connected()){
	  status = 1;
  } else {
	  //modbus_t* mb = modbus_new_tcp(_client, _ip, _port);
	  //ModbusClient::begin(mb, MODBUS_TCP_SLAVE);
  }
  
  return status;
}

void ModbusTcpClient::Close()
{
  end();
}