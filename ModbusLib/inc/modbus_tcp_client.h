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

class ModbusTCPClient : public ModbusClient {
public:
  /**
   * ModbusTCPClient constructor
   *
   * @param client Client to use for TCP connection
   */
  ModbusTCPClient(EthernetTcpClient& client);
  virtual ~ModbusTCPClient();

  /**
   * Start the Modbus TCP client with the specified parameters
   *
   * @param ip IP Address of the Modbus server
   * @param port TCP port number of Modbus server, defaults to 502
   *
   * @return 1 on success, 0 on failure
   */
  int begin(IpAddress ip, uint16_t port = 502);

  /**
   * Query connection status.
   *
   * @return 1 if connected, 0 if not connected
   */
  int connected();

  /**
   * Disconnect the client.
   */
  void stop();

private:
  EthernetTcpClient* _client;
};

#endif