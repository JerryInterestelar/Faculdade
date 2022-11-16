#Cliente SMTP com sockets
#Feito por Jerry de Sousa
#           Marcos Domingos

from socket import socket, AF_INET, SOCK_STREAM # Familia de socket do tipo IPV4 com transmissao TCP
from ssl import create_default_context          # Socket com TLS
from base64 import b64encode                    # Encode necessario para autenticar a conexao TLS

TEXTO = "Mensagem mais light\r\n"
ENDMSG = "\r\n.\r\n"
MAILSERVER = ("smtp.gmail.com", 587)            # Servidor de SMTP utilizado
TARGET_EMAIL = ""

# Resposta do Servidor
def response(socket, mensagem):         
    print(mensagem + socket.recv(1024).decode()) 

# Variacao do "send_command" que envia comandos de autentificação
def send_auth_command(socket, smtp_command, response_msg):
    command = smtp_command + '\r\n'.encode()
    socket.send(command)
    response(socket, response_msg)

# Manda comandos SMTP ao servidor e mostra a resposta
def send_command(socket, smtp_command, response_msg):
    command = smtp_command + '\r\n'
    socket.send(command.encode())
    response(socket, response_msg)

# Protocolo padrao de envio de uma mensagem via SMTP
def send_email(socket, rem, dest, msg):


    # Envia um comando MAIL FROM e mostra a resposta
    send_command(socket, f"MAIL FROM: <{rem}>", "Resposta de MAIL: ") 

    # Envia um comando RCPT TO e mostra a resposta
    send_command(socket, f"RCPT TO: <{dest}>", "Resposta de RCPT: ") 

    # Envia um comando DATA e mostra a resposta
    send_command(socket, "DATA", "Resposta de DATA: ") 

    # Onde e escrita a mensagem
    socket.send(msg.encode())
    socket.send(ENDMSG.encode())

    response(s_clientSocket, "Resposta de fim da mensagem: ")
    pass


#================================================================
# Antes de usar o protocolo smtp da google e preciso
# fazer uma autentificação na conexao via TLS, a biblioteca
# SSL nos da um objeto que faz um socket com esse suporte
# Com isso autenticamos a conexao com um login do email do
# usuario e sua senha de aplicativo (configurada na conta Google
# do usuario).
#================================================================

def autenticar(socket, server):
    # Criamos um socket com TLS
    s_socket = create_default_context().wrap_socket(socket, server_hostname=server)
    send_command(s_socket, "auth login", "Pedido de login: ") 

    # Enviamos os dados de login 
    login = input("Informe seu Email: ")
    login_encoded = b64encode(login.encode())
    send_auth_command(s_socket, login_encoded, "Email: ") 

    # Enviamos os dados da senha do usuario
    # E necessario uma senha de aplicativo pre configurada pelo o usuario
    # Esse procedimento e feito pelo o proprio usuario em sua conta google
    passw = input("Informe sua senha (senha de APP): ")
    passw = b64encode(passw.encode())
    send_auth_command(s_socket, passw, "Senha: ") 
    return s_socket, login

if __name__ == "__main__":

    # Criamos um socket chamado clinetSocket e estabelecemos uma conexao TCP com servidor MAILSERVER
    clientSocket = socket(AF_INET, SOCK_STREAM)
    clientSocket.connect(MAILSERVER)
    response(clientSocket, "Conecao TCP estabelecida: ")

    # Envia um comando HELO e mostra a resposta
    send_command(clientSocket, "HELO gmail.com", "Resultado de Helo: ") 

    #Indica ao servidor que queremos iniciar uma conexao por TLS
    send_command(clientSocket, "starttls", "Resultado do TLS: ") 
    #Processo de autentificação da conexao
    s_clientSocket, user = autenticar(clientSocket, MAILSERVER[0])

    #Envio de Email
    send_email(s_clientSocket, user, TARGET_EMAIL, TEXTO)

    # Envia um comando QUIT para fechar a conexao SMTP e mostra a resposta
    send_command(s_clientSocket, "QUIT", "Fim de conexao....: ") 

    s_clientSocket.close()
    clientSocket.close()
