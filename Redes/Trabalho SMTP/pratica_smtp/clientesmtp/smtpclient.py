from socket import socket, AF_INET, SOCK_STREAM
from ssl import create_default_context
from base64 import b64encode

TEXTO = "Mensagem mais light\r\n"
ENDMSG = "\r\n.\r\n"
MAILSERVER = ("smtp.gmail.com", 587)


def response(socket, mensagem):
    print(mensagem + socket.recv(1024).decode()) 


def send_auth_command(socket, smtp_command, response_msg):
    command = smtp_command + '\r\n'.encode()
    socket.send(command)
    response(socket, response_msg)

def send_command(socket, smtp_command, response_msg):
    command = smtp_command + '\r\n'
    socket.send(command.encode())
    response(socket, response_msg)

def send_email(socket, rem, dest, msg):

# Send MAIL FROM command and print server response.
    send_command(socket, f"MAIL FROM: <{rem}>", "Resposta de MAIL: ") 

# Send RCPT TO command and print server response.
    send_command(socket, f"RCPT TO: <{dest}>", "Resposta de RCPT: ") 

# Send DATA command and print server response.
    send_command(socket, "DATA", "Resposta de DATA: ") 

# Send message data.
    socket.send(msg.encode())
    socket.send(ENDMSG.encode())

    response(s_clientSocket, "Resposta de fim da mensagem: ")
    pass
#================================================================
# Antes de usar o protocolo smtp da google e preciso
# fazer uma autentificação na conexao via TLS, a biblioteca
# SSL nos da um objeto que faz um socket com esse suporte
# Com isso autencicamos a conexao com um login do email do
# usuario e sua senha de aplicativo (configurada na conta Google
# do usuario).
#================================================================

def autenticar(socket, server):
    s_socket = create_default_context().wrap_socket(socket, server_hostname=server)
    send_command(s_socket, "auth login", "Pedido de login: ") 

    login = input("Informe seu Email: ")
    login_encoded = b64encode(login.encode())
    send_auth_command(s_socket, login_encoded, "Email: ") 

    passw = input("Informe sua senha (senha de APP): ")
    passw = b64encode(passw.encode())
    send_auth_command(s_socket, passw, "Senha: ") 
    return s_socket, login

if __name__ == "__main__":

# Create socket called clientSocket and establish a TCP connection with mailserver
    clientSocket = socket(AF_INET, SOCK_STREAM)
    clientSocket.connect(MAILSERVER)
    response(clientSocket, "Conecao TCP estabelecida: ")

# Send HELO command and print server response.
    send_command(clientSocket, "HELO gmail.com", "Resultado de Helo: ") 

#Indica ao servidor que queremos iniciar uma conexao por TLS
    send_command(clientSocket, "starttls", "Resultado do TLS: ") 

#Processo de autentificação da conexao
    s_clientSocket, user = autenticar(clientSocket, MAILSERVER[0])

#Envio de Email
    send_email(s_clientSocket, user, "jerrydesousa172@gmail.com", TEXTO)

# Send QUIT command and get server response.
    send_command(s_clientSocket, "QUIT", "Fim de conexao....: ") 

    s_clientSocket.close()
    clientSocket.close()
