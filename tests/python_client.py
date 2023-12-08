# echo-client.py

import socket, time

HOST = "172.16.10.177"  # The server's hostname or IP address
PORT = 502  # The port used by the server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    for x in range(100):
        s.sendall(b"Hello, world")
        data = s.recv(1024)
        print(f"Received {data!r}")
        time.sleep(0.01)
