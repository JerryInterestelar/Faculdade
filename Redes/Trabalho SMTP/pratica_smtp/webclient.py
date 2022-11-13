from socket import socket, AF_INET, SOCK_STREAM

host = "127.0.0.1"
port = 33333

conn_sock = socket(AF_INET, SOCK_STREAM)

conn_sock.connect((host, port))
conn_sock.sendall("GET /index.html".encode())
data = conn_sock.recv(1024)

print(f"Received {data}")

