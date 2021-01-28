import serial
import threading
import eventmaster

"""Список того, что можно импортировоть из модуля через from"""
__all__ = [
    "State",
    "Glass"
]


class __GlassState:  # структура состояние очков
    # псевдо-приватные переменные
    __stopped = 0  # очки остановлены
    __reading = 1  # производится чтение углов
    __exit = 2  # был выход из программы
    __error = 3  # произошла ошибка

    def __setattr__(self, key, value):
        raise AttributeError("This class can not assign any attributes")    # нельзя присваивать никаких аттрибутов

    # их геттеры
    @property
    def Stopped(self):
        return self.__stopped

    @property
    def Reading(self):
        return self.__reading

    @property
    def Exit(self):
        return self.__exit

    @property
    def Error(self):
        return self.__error


State = __GlassState()  # глобальная переменная состояния, переменные можно только читать


class Glass(threading.Thread):
    def __init__(self, portName):
        threading.Thread.__init__(self)
        self.port = serial.Serial(portName, baudrate=115200)
        self.primatyData = [0, 0, 0]  # начальные углы
        self.__angleHead = [0, 0, 0]  # Углы положения головы
        self.stopAngle = [0, 0, 0]
        self.state = State.Stopped  # ставвим начальное положение - остановлен
        self.startFlag = False  # метка нажатия кнопки старт
        self.eventDict = {  # Словарь событий
            "START": eventmaster.Event("START"),  # событие начала работы
            "STOP": eventmaster.Event("STOP"),  # событие окончания работы
            "EXIT": eventmaster.Event("EXIT"),  # событие выхода из потока
            "READ": eventmaster.Event("READ"),  # события чтения углов
            "ERROR": eventmaster.Event("ERROR")  # событие ошибки чтения углов
        }
        self.eventMaster = eventmaster.EventMaster()  # создаем мастера событий
        self.eventMaster.append(self.eventDict.get("START"))  # привязываем обработчики
        self.eventMaster.append(self.eventDict.get("STOP"))
        self.eventMaster.append(self.eventDict.get("EXIT"))
        self.eventMaster.append(self.eventDict.get("READ"))
        self.eventMaster.append(self.eventDict.get("ERROR"))
        self.eventMaster.start()

    # возвращает значение углов
    @property
    def angleHead(self):
        return self.__angleHead

    def connect(self, toEvent, foo):  # ф-ия подключения обработчика события по имени события
        event = self.eventDict.get(toEvent)
        if not event:  # если в словаре событий нет такого события - ошибка
            raise eventmaster.EventError(toEvent + ": There is no such event")
        event.connect(foo)

    def exit(self):
        self.state = State.Exit
        self.eventDict.get("EXIT").push()  # вызов события выхода
        self.eventMaster.exit()
        self.port.close()

    def getState(self):
        return self.state

    def _readMessage(self):
        buf = b''  # временный буффер
        temp = self.port.read()  # читаем побайтово
        while temp != b'<':  # читаем пока не найдем вхождение
            temp = self.port.read()
        temp = self.port.read()
        while temp != b'>':
            if temp == b'<':
                return None
            buf += temp
            temp = self.port.read()
        return buf

    def _parseMessage(self, message):
        try:
            listbuf = list(map(bytes, message.split()))  # разделение сообщения на токены и запись их в список
            if listbuf[0] == b'ypr':
                newData = [float(i) for i in listbuf[1:]]
                if self.startFlag:  # если была нажата кнопка старт
                    self.primatyData = newData[:]  # устанавливаем начальные данные
                    self.startFlag = False
                if self.state is State.Reading:  # если уже производится чтение углов
                    self.__angleHead = (newData[0] - self.primatyData[0], newData[1] - self.primatyData[1], newData[2] - self.primatyData[2])  # записываем углы
                    # test
                    if (self.__angleHead[0] > 120) or (self.__angleHead[0] < -120):
                        self.__angleHead[0] = self.stopAngle[0]
                    if (self.__angleHead[1] > 80) or (self.__angleHead[1] < -80):
                        self.__angleHead[1] = self.stopAngle[1]
                    if (self.__angleHead[2] > 100) or (self.__angleHead[2] < -100):
                        self.__angleHead[2] = self.stopAngle[2]
                    self.stopAngle = self.__angleHead
                    # test
                    self.eventDict.get("READ").push(self.__angleHead[0], self.__angleHead[1], self.__angleHead[2])

            elif listbuf[0] == b'*':  # если сообщение начинается с *
                print("COMMENT: " + str(message))  # вывод комментария

            elif listbuf[0] == b'start':
                self.eventDict.get("START").push()
                self.startFlag = True
                self.state = State.Reading

            elif listbuf[0] == b'stop':  # если пришло сообщение stop(была нажата кнопка stop)
                self.primatyData = [0, 0, 0]  # сбросить текущие углы
                self.eventDict.get("STOP").push()  # вызвать событие нажатия кнопки стоп
                self.state = State.Stopped
        except:
            self.eventDict.get("ERROR").push()

    def run(self):
        while self.state is not State.Exit:
            self.port.write(b'g')  # отправляем любой символ - готовы читать
            message = self._readMessage()
            if message is not None:
                self._parseMessage(message)


if __name__ == "__main__":
    def startHandler():
        print("I started!")


    def stopHandler():
        print("I stopped!")


    def readHandler(yaw, pitch, roll):
        print(int(yaw), int(pitch), int(roll))


    glass = Glass("/dev/ttyUSB0")
    glass.connect("START", startHandler)
    glass.connect("STOP", stopHandler)
    glass.connect("READ", readHandler)
    glass.start()

