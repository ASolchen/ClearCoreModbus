from pymodbus.client import ModbusTcpClient
import time

with ModbusTcpClient('192.168.10.177') as clear_core:
    for x in range(64):

        res = clear_core.write_coil(x, bool(x%2), slave=1)
        res = clear_core.read_coils(1,10, slave=1)
        print(res.bits)
        time.sleep(0.05)
        res = clear_core.write_register(x,x,slave=1)
        res = clear_core.read_holding_registers(x,1,slave=1)
        print(res.registers)
        time.sleep(0.05)