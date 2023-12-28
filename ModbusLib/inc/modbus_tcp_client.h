/*
* modbus_tcp_client.h
* Copyright © 2010-2012 Stéphane Raimbault <stephane.raimbault@gmail.com>
* Copyright © 2018 Arduino SA. All rights reserved.
* Copyright © 2023 Adam Solchenberger <asolchenberger@gmail.com>
* SPDX-License-Identifier: LGPL-2.1+
*/

#ifndef _MODBUS_TCP_CLIENT_H_INCLUDED
#define _MODBUS_TCP_CLIENT_H_INCLUDED

#include "modbus_client.h"
#include "libmodbus/modbus_tcp.h"
#include <errno.h>
extern "C" {
	#include "libmodbus/modbus.h"
}

class ModbusTcpClient : public ModbusClient {
public:
  /**
   * ModbusTcpClient constructor
   *
   * @param client Client to use for TCP connection
   */
  ModbusTcpClient(unsigned long defaultTimeout=30000);
  virtual ~ModbusTcpClient();

  /**
   * Start the Modbus TCP client with the specified parameters
   *
   * @param ip IP Address of the Modbus server
   * @param port TCP port number of Modbus server, defaults to 502
   *
   * @return 1 on success, 0 on failure
   */
  int Begin(IpAddress ip, uint16_t port=MODBUS_PORT);

  /**
   * Query connection status.
   *
   * @return 1 if connected, 0 if not connected
   */
  int Connected();

  /**
   * Disconnect the client.
   */
  void Close();

private:
  EthernetTcpClient* _client;
  IpAddress _ip;
  uint32_t _port;
};

#endif