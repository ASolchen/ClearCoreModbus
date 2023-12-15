from pymodbus.client import ModbusTcpClient
import time, threading


def connect_mb(thread_num):
    with ModbusTcpClient('192.168.10.177') as clear_core:
        for y in range(4):
            for x in range(6):
                res = clear_core.write_coil(x, bool((x+y)%2), slave=1)
                res = clear_core.read_coils(1,10, slave=1)
                #print(res.bits)
                time.sleep(0.05)
                res = clear_core.write_register(0,y*6+x,slave=1)
                res = clear_core.read_holding_registers(0,5,slave=1)
                #res = clear_core.read_input_registers(x,1,slave=1)
                #print(x, res.registers)
                if(thread_num == 4):
                    print(res.registers)
                time.sleep(0.02)

t_list = []

#Created the Threads
for t in range(5):
    t_list.append(threading.Thread(target=connect_mb, args=(t,)))
 
#Started the threads
for t in t_list:
    t.start()
    time.sleep(0.005)
 
#Joined the threads
for t in t_list:
    t.join()
print("All done")
 