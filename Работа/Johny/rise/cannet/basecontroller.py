import time

from rise.cannet import protocan
import struct


class BaseController:
    """ Базовый контроллер """

    def __init__(self, owner, addr, name):
        self._owner = owner  # Handler
        self._addr = addr  # аддресс контроллера
        self._name = name  # имя контроллера
        self._isConnected = False  # подключен ли контроллер

        self._paramDict = {
            0x00: {"name": "Test connection", type: "B", "activeValue": None}}  # список параметров контроллера
        self._commandDict = {0xC8: {"name": "Send param", type: ""},  # список комманд контроллера
                             0xC9: {"name": 'Send important params', type: ""},
                             0xCA: {"name": 'Write in EEPROM', type: ""},
                             0xCB: {"name": 'Read from EEPROM', type: ""}}

    @property
    def isConnected(self):
        return self._isConnected    # TODO: !!! На прошивке драйвера не робит эта часть, перепрошить

    @property
    def address(self):
        return self._addr

    def setParamRequest(self, prmNum, value):
        """ запрос на установку указанного параметра на контроллере """
        if prmNum not in self._paramDict.keys():
            raise ValueError("Параметра " + prmNum.__repr__() + " не существует в данном контроллере")
        fmt = protocan.proto["headParamFormat"] + self._paramDict[prmNum][type]  # формат пакета
        prmLen = struct.calcsize(self._paramDict[prmNum][type])  # длина параметра в байтах
        package = struct.pack(fmt, prmNum, prmLen, value)

        msg = protocan.getDefaultMessage(self._addr, package)  # получаем сообщение
        self._owner.send(msg)  # отправляем его
        time.sleep(0.1)

    def sendCommand(self, cmdNum, *cmdParams):
        """ Отправка комманды на контроллер """
        if cmdNum not in self._commandDict.keys():
            raise ValueError("Комманды " + cmdNum.__repr__() + " не существует в данном контроллере")

        cmdPackage = struct.pack(protocan.proto["headCommandFormat"] + self._commandDict[cmdNum][type], cmdNum, *cmdParams)
        msg = protocan.getDefaultMessage(self._addr, cmdPackage)
        self._owner.send(msg)
        # TODO: разобраться почему пакет комманды отличается от пакета параметров

    def checkConnectionRequest(self):
        """ запрос - проверка подключения устройства """
        self._isConnected = False
        self._paramDict[0x00]["activeValue"] = None
        self.setParamRequest(0x00, 42)

    def parseMsg(self, msg):
        """ Парсинг сообщения, приходящего на данный контроллер с данным адресом """
        headSize = protocan.proto["headParamSize"]   # для сокращения размера кода
        prmNum, prmLen = struct.unpack(protocan.proto["headParamFormat"], msg.data[0:headSize])  # выделяем заголовок
        if prmNum not in self._paramDict.keys():    # смотрим емть ли данный параметр в списке параметров
            raise ValueError("Пришедший параметр " + prmNum.__repr__() + " на данном контроллере отсутствует")

        prmType = self._paramDict[prmNum][type]     # для сокращения размеров кода
        if prmLen != struct.calcsize(prmType):
            raise ValueError("Длина пришедшего параметра: " + prmNum.__repr__() + " не совпадает с действительной")

        data = struct.unpack(prmType, msg.data[headSize:headSize + prmLen])  # получаем данные и записываем их в словарь
        self._paramDict[prmNum]["activeValue"] = data

        if self._paramDict[0x00]["activeValue"] == protocan.testConnectionNumber:   # проверка подключения
            # контроллера к сети
            self._isConnected = True

    def getParamByNum(self, num):
        try:
            return self._paramDict[num]["activeValue"]
        except KeyError:
            raise KeyError("У данного параметра нет активного значения")
