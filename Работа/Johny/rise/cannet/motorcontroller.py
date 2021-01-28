from rise.cannet.basecontroller import BaseController


class WorkMode:
    """ Класс, хранящий режимы работы контроллера """
    NONE = 0
    PID = 1
    PWM = 2


class MotorController(BaseController):
    def __init__(self, owner, addr):
        BaseController.__init__(self, owner, addr, "MotorController")

        self._commandDict.update({
            0xCC: {"name": 'work mode', type: "B"},  #
            0xCD: {"name": 'clear odometres', type: ""},  #
            0xCE: {"name": 'PWM pulse', type: ""},  # TODO: тип - неизвестно)
            0xCF: {"name": 'set PWM', type: "Bh"},  #
            0xD0: {"name": 'set speed', type: "Bh"},  #
            0xD1: {"name": 'set all PWM', type: "hh"},
            0xD2: {"name": 'set all speed', type: "hh"},
            0xD3: {"name": 'power out', type: "BB"}
        })

        self._paramDict.update({
            0x01: {"name": 'debug info mask', type: "B"},
            0x02: {"name": 'proportional coefficient', type: "f"},
            0x03: {"name": 'integral coefficient', type: "f"},
            0x04: {"name": 'derivative coefficient', type: "f"},
            0x05: {"name": 'limit summ', type: "h"},
            0x06: {"name": 'time PID', type: "H"},
            0x07: {"name": 'time PWM', type: "H"},
            0x08: {"name": 'pwm DeadZone', type: "B"},
            0x09: {"name": 'accelbreak step', type: "B"},
            0x0A: {"name": 'emergency level', type: "H"},

            # Параметры только для чтения
            0x12: {"name": 'PRM voltage', type: "f", "activeValue": None},
            0x13: {"name": 'Error Code', type: "B"},
            0x14: {"name": 'Work mode', type: "B"},

            # 1 мотор
            0x15: {"name": 'PRM_P_A', type: "f"},
            0x16: {"name": 'PRM_I_A', type: "f"},
            0x17: {"name": 'PRM_D_A', type: "f"},
            0x18: {"name": 'PRM_INT_SUMM_A', type: "i"},
            0x19: {"name": 'PRM_PWM_A', type: "h"},
            0x1A: {"name": 'PRM_PARROT_A', type: "h"},
            0x1B: {"name": 'PRM_ODOM_A', type: "i"},
            0x1C: {"name": 'PRM_SET_PARROT_A', type: "h"},
            0x1D: {"name": 'PRM_SET_PWM_A', type: "h"},

            # 2 мотор
            0x1E: {"name": 'PRM_P_B', type: "f"},
            0x1F: {"name": 'PRM_I_B', type: "f"},
            0x20: {"name": 'PRM_D_B', type: "f"},
            0x21: {"name": 'PRM_INT_SUMM_B', type: "i"},
            0x22: {"name": 'PRM_PWM_B', type: "h"},
            0x23: {"name": 'PRM_PARROT_B', type: "h"},
            0x24: {"name": 'PRM_ODOM_B', type: "i"},
            0x25: {"name": 'PRM_SET_PARROT_B', type: "h"},
            0x26: {"name": 'PRM_SET_PWM_B', type: "h"}
        })

    def setWorkMode(self, workMode=WorkMode.NONE):  # Инициализация режима работы (2 - ШИМ, 1 - ПИД, 0 - пока
        # используется как выключение ручного режима)
        self.sendCommand(0xCC, workMode)

    def setAllSpeed(self, speed1, speed2):  # Установить скорость мотора (0 и 1)
        self.sendCommand(0xD2, speed1, speed2)

    def setSpeed(self, motorNum, speed):  # Установить скорость мотора (0 и 1)
        self.sendCommand(0xD0, (motorNum, speed))

    def setMotorPwm(self, motorNum, pwm):
        self.sendCommand(0xCF, (motorNum, pwm))

    def getActiveVoltage(self):
        return self.getParamByNum(0x12)
