import struct

protocolConfig = {
    "startBytes": b'\xaa\xaa',
    "checksum": "crc16",
    "formatDesc": 'B',  # дескриптор пакета, определяет, как будет читаться пакет
    "formatChecksum": 'H'  # 2 байта
}

protocolFormatDescription = {  # описание форматов пакетов по дескриптору
    0: '',  # онлайн метка
    1: '=2B',  # пакет данных c кодом ошибки и доп. данными
    2: '=fff',   # пакет данных со значением трех углов поворота головы робота
    3: '',   # пустой пакет, сообщает о калибровке головы робота
    4: '=?',   # включить/выключить видео
    5: '=b',    # движение вперед/назад, скорость [-100 : +100]
    6: '=b',    # поворот на месте, скорость [-100 : +100]
}

_headFormat = '=' + protocolConfig["formatChecksum"] + protocolConfig["formatDesc"]
_headSize = struct.calcsize(_headFormat)


def readPackage(readArrFun):
    """ Чтение пакета по протоколу """
    temp = bytearray(len(protocolConfig["startBytes"]))
    while temp != protocolConfig["startBytes"]:  # ищем вхождение
        temp.pop(0)
        temp += readArrFun(1)
    head = struct.unpack(_headFormat, readArrFun(_headSize))  # читаем заголовок

    fmt = protocolFormatDescription[head[1]]  # получаем формат пакета
    data = struct.unpack(fmt, readArrFun(struct.calcsize(fmt)))  # читаем данные
    if check(head[0], data):  # проверяем данные
        return head, data
    else:
        return None


def sendPackage(sendArrFun, desc, data):
    """ Отправка пакета по протоколу """
    pack = struct.pack(protocolFormatDescription[desc], *data)
    ch = checksum(pack)
    head = protocolConfig["startBytes"] + struct.pack(_headFormat, ch, desc)
    sendArrFun(head + pack)


def check(checksum, data):
    return True  # TODO: доделать


def checksum(data):
    return 0  # TODO: доделать
