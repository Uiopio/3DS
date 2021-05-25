import socket
import time
import os
#import cv2

from pioneer import Pioneer

host = "10.10.16.179"
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
        self.send_message("successful")

    # Принять сообщение
    def accept_message(self):
        message = self.conn.recv(1024)
        return message.decode("utf-8")

    # Отправить сообщениеё
    def send_message(self, output_message):
        self.conn.sendall(bytes(output_message, encoding="utf-8"))



def message_perse(message):
    # Если сообщение начинается с буквы L (led_control) то возврашаем 3 значения интенсивности
    # Пример сообщения: L: R1 G20 B255
    if message[0] == "L":
        # Ищем индексы опорных букв
        ind_R = message.find("R")
        ind_G = message.find("G")
        ind_B = message.find("B")

        R = float(message[(ind_R + 1):(ind_G - 1)])
        G = float(message[(ind_G + 1):(ind_B - 1)])
        B = float(message[(ind_B + 1):(len(message))])

        return R, G, B

    # Если сообщение начинается с буквы M (motion) то возврашаем 4 значения координат (X, Y, Z, Angle)
    elif message[0] == "M":
        ind_X = message.find("X")
        ind_Y = message.find("Y")
        ind_Z = message.find("Z")
        ind_A = message.find("A")

        X = float(message[(ind_X + 1):(ind_Y - 1)])
        Y = float(message[(ind_Y + 1):(ind_Z - 1)])
        Z = float(message[(ind_Z + 1):(ind_A - 1)])
        A = float(message[(ind_A + 1):(len(message))])

        return X, Y, Z, A


if __name__ == '__main__':
    pioneer = Pioneer(logger=True)

    server = Server(host, port)

    while(True):

        message = str(server.accept_message())

        if(message == "arm"):
            pioneer.arm()

        elif (message == "disarm"):
            pioneer.disarm()

        elif (message == "takeoff"):
            pioneer.takeoff()

        elif (message == "land"):
            pioneer.land()

        elif (message.find("L") != -1) or (message.find("M") != -1):
            if message[0] == "L":
                print(message)
                R, G, B = message_perse(message)
                pioneer.led_control(r=R, g=G, b=B)

            elif message[0] == "M":
                print(message)
                X, Y, Z, Angle = message_perse(message)
                pioneer.go_to_local_point( x=X, y=Y, z=Z, yaw=Angle)



