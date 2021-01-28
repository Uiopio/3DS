import socket
import threading
import time
from rise.rtx.urtx import proto
from rise.utility import eventmaster


class SocketUrtx(threading.Thread):
    """ Класс, использующий сокеты и протокол urtx """
    def __init__(self, **kwargs):
        threading.Thread.__init__(self, daemon=True)
        self._sock = socket.socket(**kwargs)
        self._host = None  # (ip, port)
        self.__exit = False  # метка выхода
        self._eventDict = {"onReceive": eventmaster.Event("onReceive")}  # создаем словарь событий
        self._eventMaster = eventmaster.EventMaster()  # создаем мастера событий
        self._eventMaster.append(self._eventDict["onReceive"])  # добавляем событие в мастер событий
        for desc in proto.protocolFormatDescription.keys():
            self._eventDict.update({desc: eventmaster.Event(desc)})  # добавляем события приема сообщений с
            # определенным дескриптором
            self._eventMaster.append(self._eventDict[desc])
        self._eventMaster.start()

    def connect(self, host):
        """ метод подключения к хосту """
        self._host = host
        self._connect(host)

    def _connect(self, host):
        """ метод для перегрузки классом наследником """
        pass

    def disconnect(self):
        """ метод отключения от хоста """
        self._sock.shutdown(socket.SHUT_RDWR)
        self._sock.close()

    def exit(self):
        """ метод выхода из потока """
        self.__exit = True

    def _readArray(self, size):
        """ метод для перегрузки классом наследником """
        return None

    def _sendArray(self, data):
        """ метод для перегрузки классом наследником """
        pass

    def _readPackage(self):
        """ Читаем пакет и вызываем события: общее и для определенного дескриптора """
        package = proto.readPackage(self._readArray)
        self._eventDict["onReceive"].push(package)  # вызываем общее событие о приеме
        desc = package[0][1]
        data = package[1]
        self._eventDict[desc].push(data)  # вызываем событие о приеме пакета по определенному дескриптору
        return package

    def sendPackage(self, desc, data):
        """ отправляем пакет форматом, описываемым дескриптором """
        proto.sendPackage(self._sendArray, desc, data)

    def run(self):
        while not self.__exit:
            try:
                self._readPackage()
            except:
                time.sleep(1)

    def subscribe(self, event, handler):
        ev = self._eventDict[event]
        if not ev:
            raise eventmaster.EventError("Такого события нет")
        ev.connect(handler)


class TcpServer(SocketUrtx):
    """ Сервер, написанный на основе SocketUrtx """
    def __init__(self, **kwargs):
        SocketUrtx.__init__(self, type=socket.SOCK_STREAM, **kwargs)
        self.clientAddr = None

    def open(self, host):
        self._sock.bind(host)
        self._sock.listen(1)

    def _connect(self, host):
        self.__rec, self.clientAddr = self._sock.accept()
        self.__rec.settimeout(3.0)

    def _readArray(self, size):
        out = b''
        for i in range(size):
            b = self.__rec.recv(1)
            if b is None:   # соединение разорвано
                return None
            out += b
        return out

    def _sendArray(self, data):
        self.__rec.send(data)


class TcpClient(SocketUrtx):
    def __init__(self, **kwargs):
        SocketUrtx.__init__(self, type=socket.SOCK_STREAM, **kwargs)
        self._sock.settimeout(5)

    def _connect(self, host):
        self._sock.connect(host)

    def _readArray(self, size):
        """ ф-ия чтения массива, размером size """
        out = b''
        for i in range(size):
            b = self._sock.recv(1)
            if b is None:   # соединение разорвано
                return None
            out += b
        return out

    def _sendArray(self, data):
        self._sock.send(data)


if __name__ == "__main__":

    def th():
        cl = TcpClient()
        cl.connect(("localhost", 9094))
        cl.start()

        def onReceive(data):
            print("onRec", data)

        def onDesc3(data):
            print(3, data)

        def onDesc2(data):
            print(1, data)

        cl.subscribe("onReceive", onReceive)
        cl.subscribe(3, onDesc3)
        cl.subscribe(1, onDesc2)

        cl.sendPackage(0, (5,))
        cl.sendPackage(1, (1, 2, 3, 4, 5, 6, 11, 12, 13, 14, 15, 21))

        while True:
            time.sleep(10)


    t = threading.Thread(target=th)
    t.start()

    serv = TcpServer()
    serv.connect(("localhost", 9094))
    serv.start()


    def onReceive(data):
        print("onRecServer", data)


    def onDesc1(data):
        print("0 Server", data)


    def onDesc2(data):
        print("1 Server", data)


    serv.subscribe("onReceive", onReceive)
    serv.subscribe(0, onDesc1)
    serv.subscribe(1, onDesc2)

    serv.sendPackage(3, ())
    serv.sendPackage(1, (1, 2, 3, 4, 5, 6, 11, 12, 13, 14, 15, 21))

    while True:
        time.sleep(10)
