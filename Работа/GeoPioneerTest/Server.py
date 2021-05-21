import socket
import time

from pioneer import Pioneer

host = "10.10.16.173"
port = 8000

class Server():
    def __init__(self, server_ip, server_port):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Попытка создать сервер
        try:
            self.sock.bind((server_ip, server_port))
            self.sock.listen(True)
        except:
            print("Не удалось создать сервер")

        self.conn, self.addr = self.sock.accept()
        print("Connected by", self.addr)

        # Отправляем клтенту подтверждение подключения
        output_message = "successful"
        self.conn.sendall(bytes(output_message, encoding="utf-8"))

    # Принять сообщение
    def accept_message(self):
        message = self.conn.recv(1024)
        return message.decode("utf-8")

    # Отправить сообщениеё
    def send_message(self, output_message):
        self.conn.sendall(bytes(output_message, encoding="utf-8"))


if __name__ == '__main__':
    pioneer = Pioneer(logger=True)

    server = Server(host, port)

    while(True):
        message = server.accept_message()

        if(message == "arm"):
            pioneer.arm()
        elif (message == "disarm"):
            pioneer.disarm()

