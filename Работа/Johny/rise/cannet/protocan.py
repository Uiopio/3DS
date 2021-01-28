import can
import struct


def getRequestMessage():
    """ выдает сообщение - запрос """
    return can.Message(arbitration_id=0x500, extended_id=False, data=[])


def getOnlineMessage():
    """ выдает сообщение - онлайн метку """
    return can.Message(arbitration_id=0x600, extended_id=False, data=[])


def getDefaultMessage(arbitration_id, data):
    """ выдать сообщение по умолчанию """
    return can.Message(arbitration_id=arbitration_id, extended_id=False, data=data)


def isRequestAnswer(msg):
    """ проверяет, является ли сообщение ответом на запрос"""
    return (msg.arbitration_id == proto["requestAnswer"]["arbitration_id"]) and (
            msg.dlc == proto["requestAnswer"]["dlc"])


proto = {
    "requestAnswer": {"arbitration_id": 0x501,      # формат сообщения - ответа на запрос
                      "dlc": 7,
                      "format": "=2H 3B"},
    "headParamFormat": "=2B ",   # формат заголовка пакета
    "headCommandFormat": "=B ",   # формат заголовка пакета
    "headParamSize": None,    # размер заголовка, считается в следующей строке
    "headCommandSize": None  # размер заголовка, считается в следующей строке
}

proto["headParamSize"] = struct.calcsize(proto["headParamFormat"])
proto["headCommandSize"] = struct.calcsize(proto["headCommandFormat"])


testConnectionNumber = 42   # число приходящее с контроллера, при проверке подключения его к сети