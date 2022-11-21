from socket import socket, AF_INET, SOCK_STREAM
import sys 

host = "localhost"
port = 33333

server = socket(AF_INET, SOCK_STREAM)
server.bind((host, port))
server.listen()

while True:
    print("Servidor Rodando")
    connection, addr = server.accept()
    print(f"Usuario {addr} conectou-se")
    
    try:
        message = connection.recv(1024).decode()
        file_name = message.split()[1]
        print(file_name)
        file = open(file_name[1:], "r")


        #parece que eu acertei
        output = file.readlines()

        #headers html
        connection.send('HTTP/1.0 200 OK\n'.encode())
        connection.send('Content-Type: text/html\n'.encode())
        #arquivo html em si
        for i in range(len(output)):
            connection.send(output[i].encode())
        #fim do arquivo
        connection.send("\r\n".encode())

        file.close()
        connection.close()
    except IOError:
        connection.send('HTTP/1.0 404 OK\n'.encode())
        connection.send('Content-Type: text/html\n'.encode())
        connection.send("""
                        <h1>404</h1>
                        <p>Arquivo nao encontrado</p>
                        """.encode())
        connection.close()
server.close()
sys.exit()
