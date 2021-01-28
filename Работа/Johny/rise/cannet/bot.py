import struct
import can
from rise.cannet import protocan
import threading
import time


class Robot(threading.Thread):
    """ Класс, ставящийся на робота с can шиной """

    def __init__(self, bus=None):
        threading.Thread.__init__(self, daemon=True)
        self._bus = bus  # шина can
        self.online = False  # флаг, определяющий шлется онлайн метка или нет
        self.__exit = False  # метка выхода из потоков
        self._deviceList = []  # Список подключенных устройств
        self._onlineDeviceList = []  # список активных устройств, которые ответили на запрос

    def __repr__(self):
        """ Выдает информацию о себе """
        return """ class Robot: {bus: """ + self._bus.__repr__() + """, deviceList: """ + self._deviceList.__repr__() + """, onlineDeviceList: """ + self._onlineDeviceList.__repr__() + """}"""

    def send(self, msg):
        """ отправка сообщения по can """
        self._bus.send(msg)

    def recv(self):
        """ прием сообщения по can """
        return self._bus.recv()

    def addDevice(self, dev):
        """ добавляет подключенное устройство """
        self._deviceList.append(dev)

    def _deviceRequest(self):
        """ запрос всем устройствам, для проверки их наличия на шине """
        self._onlineDeviceList.clear()
        msg = protocan.getRequestMessage()
        self.send(msg)

    def __onlineThread(self):
        """ поток отправляющий онлайн метки """
        while not self.__exit:
            if self.online:  # если включена посылка онлайн меток
                msg = protocan.getOnlineMessage()
                self.send(msg)  # отправляем онлайн метку
            time.sleep(1)

    def _parseMsg(self, msg):
        if protocan.isRequestAnswer(msg):  # проверяем, является ли сообщение ответом на запрос
            answer = struct.unpack(protocan.proto["requestAnswer"]["format"], msg.data)
            device = (answer[2], answer[0], answer[3], answer[4])
            self._onlineDeviceList.append(device)
            return
        for device in self._deviceList:
            if msg.arbitration_id == device.address + 0xFF:
                device.parseMsg(msg)

    def run(self):
        threading.Thread(target=self.__onlineThread, daemon=True).start()  # включаем посылку онлайн меток
        self._deviceRequest()
        while not self.__exit:
            msg = self.recv()
            self._parseMsg(msg)

    def exit(self):
        self.__exit = True


if __name__ == """__main__""":
    bus = can.interface.Bus(channel="can0", bustype='socketcan_native')
    robot = Robot(bus)
    robot.start()
    while True:
        time.sleep(1)
