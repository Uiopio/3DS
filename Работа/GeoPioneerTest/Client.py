import socket
import time
import cv2
import numpy as np
import math


host = "10.10.16.179"
port = 8000

class Client():
    def __init__(self, server_ip, server_port):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Попытка подключиться к серверу
        try:
            self.sock.connect((server_ip, server_port))

            # Отправляем стартовое сообщение
            self.send_message("start")

            # Проверяем ответ от сервера
            answer_server = self.accept_message()
            if answer_server == "successful":
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

    def accept_message(self):
        message = self.sock.recv(1024)
        return message.decode("utf-8")

    def send_message(self, output_message):
        self.sock.sendall(bytes(output_message, encoding="utf-8"))

class Message():
    arm = "arm"
    disarm = "disarm"

    def led(self, r,g,b):
        message = "L:" + " R" + str(r) + " G" + str(g) +" B" + str(b)
        return message

    def motion(self, x,y,z,Y):
        message = "M:" + " X" + str(x) + " Y" + str(y) + " Z" + str(z) + " A" + str(Y)
        return message


if __name__ == '__main__':
    mes = Message()
    client = Client(host, port)

    client.send_message(mes.led(125, 20, 0))

    time.sleep(5)

    client.send_message(mes.led(0, 0, 0))

    window = np.zeros((300, 400, 3), np.uint8)

    command_x = float(0)
    command_y = float(0)
    command_z = float(1)
    command_yaw = math.radians(float(0))
    increment_xy = float(0.2)
    increment_z = float(0.1)
    increment_deg = math.radians(float(90))

    new_command = False

    while(True):
        cv2.imshow('window', window)
        key = cv2.waitKey(1)
        if key == 27:  # esc
            print('esc pressed')
            cv2.destroyAllWindows()
            client.send_message("land")
            client.disconnect()
            break

        elif key == ord('w'):
            print('w')
            command_y += increment_xy
            new_command = True

        elif key == ord('s'):
            print('s')
            command_y -= increment_xy
            new_command = True

        elif key == ord('a'):
            print('a')
            command_x -= increment_xy
            new_command = True

        elif key == ord('d'):
            print('d')
            command_x += increment_xy
            new_command = True

        elif key == ord('q'):
            print('q')
            client.send_message("arm")
            new_command = True

        elif key == ord('e'):
            print('e')
            client.send_message("disarm")
            new_command = True

        elif key == ord('r'):
            print('r')
            client.send_message("takeoff")
            new_command = True

        elif key == ord('h'):
            print('h')
            command_z += increment_z
            new_command = True

        elif key == ord('l'):
            print('l')
            command_z -= increment_z
            new_command = True

        if new_command:
            client.send_message(mes.motion(command_x, command_y, command_z, command_yaw))
            new_command = False





