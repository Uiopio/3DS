import socket
import time
from pioneer import Pioneer

host = "10.10.16.173"
port = 8000

class Client():
    def __init__(self, server_ip, server_port):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Попытка подключиться к серверу
        try:
            self.sock.connect((server_ip, server_port))

            # Отправляем стартовое сообщение
            output_message = "start"
            self.sock.sendall(bytes(output_message, encoding="utf-8"))

            # Проверяем ответ от сервера
            input_message = self.sock.recv(1024)
            if input_message.decode("utf-8") == "successful":
                print("Успешное подключение")

        except:
            print("Не удалось подлкючиться к серверу")

    def disconnect(self):
        # Оповещаем сервер
        output_message = "disconnect"
        self.sock.sendall(bytes(output_message, encoding="utf-8"))
        # Разрываем соединение
        print("Отключение от сервера")
        self.sock.close()

    def send_message(self, output_message):
        self.sock.sendall(bytes(output_message, encoding="utf-8"))


if __name__ == '__main__':
    client = Client(host, port)

    client.send_message("arm")

    time.sleep(3)
    client.send_message("disarm")