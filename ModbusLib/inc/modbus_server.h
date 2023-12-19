/*
* modbus_server.h
* Copyright © 2010-2012 Stéphane Raimbault <stephane.raimbault@gmail.com>
* Copyright © 2018 Arduino SA. All rights reserved.
* Copyright © 2023 Adam Solchenberger <asolchenberger@gmail.com>
* SPDX-License-Identifier: LGPL-2.1+
*/


#ifndef __MODBUS_SERVER_H__
#define __MODBUS_SERVER_H__


class ModbusServer
{
//variables
public:
protected:
private:

//functions
public:
	ModbusServer();
	~ModbusServer();
protected:
private:
	ModbusServer( const ModbusServer &c );
	ModbusServer& operator=( const ModbusServer &c );

}; //ModbusServer

#endif //__MODBUS_SERVER_H__
