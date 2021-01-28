#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import threading
import can
import time
from rise.cannet.bot import Robot
from rise.board.robothandle import JohnyHandle
from rise.rtx.urtxsocket import TcpServer
import json

#добавляем файл конфигурации для can шины
configuration = {}
with open("rise/board/robotconf.json", "r") as file:
    configuration = json.load(file)

#создаем can шину
bus = can.interface.Bus(channel=configuration["can_interface"], bustype='socketcan_native')
#bus = seeedstudio.SeeedBus(channel=configuration["candevice"])
time.sleep(1)

robot = Robot(bus)
robot.online = True #шлется онлайн метка

server = TcpServer()
server.open(("", configuration["port"]))

jh = None


def sendError(num, dlc=0):
    """ сообщить об ошибке на пульт """
    server.sendPackage(1, (num, dlc))


def recvError(data):
    """ Обработчик пришедшей ошибки с пульта """
    pass


def recvCalibrate(data):
    """ обработчик события о пришествии комманды калибровки """
    try:
        jh.calibrateHead()
    except:
        pass


def recvPosition(data):
    """ обработчик события о пришествии позиции робота """
    try:
        jh.setHeadPosition(*data)
    except KeyError:
        sendError(0x03)  # отправляем код ошибки
    except:
        pass


def recvVideoState(data):
    try:
        jh.setVideoState(configuration["videodevice"], server.clientAddr, bool(data[0]))
    except:
        pass


def recvMove(data):
    try:
        jh.move(data[0])
    except:
        pass


def recvRotate(data):
    try:
        jh.rotate(data[0])
    except:
        pass


def recvOnline(data):
    global onlineCount
    onlineCount = 0


def onReceive(data):
    """ Хендлер - заглушка """
    pass


global onlineCount
onlineCount = 0


def th():
    global onlineCount
    while True:
        onlineCount += 1
        if onlineCount > 3:
            robot.online = False    # не шлем метки
        else:
            robot.online = True     # шлем метки
        time.sleep(1)


server.subscribe(0, recvOnline)
server.subscribe(1, recvError)
server.subscribe(2, recvPosition)
server.subscribe(3, recvCalibrate)
server.subscribe(4, recvVideoState)
server.subscribe(5, recvMove)
server.subscribe(6, recvRotate)
server.subscribe("onReceive", onReceive)
server.start()
robot.start()
threading.Thread(daemon=True, target=th).start()

while True:
    server.connect(None)  # подключаемся в цикле, т.к. один раз запускаем скрипт на все время работы робота
    del jh
    jh = None
    jh = JohnyHandle(robot)
    jh.start()
