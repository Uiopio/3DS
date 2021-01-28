from rise.cannet import basecontroller


class WorkMode:
    """ Класс, хранящий режимы работы контроллера """
    NONE = 0
    FREE_ROTATION = 1
    CONTROL_POSITION = 2


class CalibrationMode:
    """ Класс, хранящий режимы работы калибровки"""
    FULL = 0
    FAST = 1


class StepperController(basecontroller.BaseController):
    """ Контроллер 3х шаговых двигателей """
    def __init__(self, owner, addr):
        basecontroller.BaseController.__init__(self, owner, addr, "StepperController")

        self._commandDict.update({
            0xCC: {"name": 'Set work mode', type: "BB"},  # Номер мотора, режим работы
            0xCD: {"name": 'Set velocity', type: "Bh"},  # Номер мотора, скорость
            0xCE: {"name": 'Robot drive', type: "HH"},  # Скорость, угол
            0xCF: {"name": 'Set position', type: "BH"},  # Номер мотора, позиция
            0xD0: {"name": 'Calibration', type: "BB"},  # Номер мотора, тип калибровки
            0xD1: {"name": 'Set all position', type: "HHH"},  # Три позиции
        })

        self._paramDict.update({
            # Первый ШД
            0x01: {"name": 'Debug info mask', type: "B"},
            0x02: {"name": 'Calibrate step length', type: "H"},
            0x03: {"name": 'Min step length', type: "h"},
            0x04: {"name": 'Max step length', type: "h"},
            0x05: {"name": 'Accel brake step', type: "H"},

            0x06: {"name": 'Motor state', type: "B"},
            0x07: {"name": 'Motor mode', type: "B"},
            0x08: {"name": 'Set velocity', type: "h"},
            0x09: {"name": 'Current velocity', type: "h"},

            0x0A: {"name": 'Set position', type: "H"},
            0x0B: {"name": 'Current position', type: "h"},
            0x0C: {"name": 'Motor calibrate state', type: "B"},  #
            0x0D: {"name": 'Calibrate range', type: "H"},

            # Второй ШД
            0x0E: {"name": 'Debug info mask', type: "B"},
            0x0F: {"name": 'Calibrate step length', type: "H"},
            0x10: {"name": 'Min step length', type: "h"},
            0x11: {"name": 'Max step length', type: "h"},
            0x12: {"name": 'Accel brake step', type: "H"},

            0x13: {"name": 'Motor state', type: "B"},
            0x14: {"name": 'Motor mode', type: "B"},
            0x15: {"name": 'Set velocity', type: "h"},
            0x16: {"name": 'Current velocity', type: "h"},

            0x17: {"name": 'Set position', type: "H"},
            0x18: {"name": 'Current position', type: "h"},
            0x19: {"name": 'Motor calibrate state', type: "B"},  #
            0x1A: {"name": 'Calibrate range', type: "H"},

            # Третий ШД
            0x1B: {"name": 'Debug info mask', type: "B"},
            0x1C: {"name": 'Calibrate step length', type: "H"},
            0x1D: {"name": 'Min step length', type: "h"},
            0x1E: {"name": 'Max step length', type: "h"},
            0x1F: {"name": 'Accel brake step', type: "H"},

            0x20: {"name": 'Motor state', type: "B"},
            0x21: {"name": 'Motor mode', type: "B"},
            0x22: {"name": 'Set velocity', type: "h"},
            0x23: {"name": 'Current velocity', type: "h"},

            0x24: {"name": 'Set position', type: "H"},
            0x25: {"name": 'Current position', type: "h"},
            0x26: {"name": 'Motor calibrate state', type: "B"},  #
            0x27: {"name": 'Calibrate range', type: "H"},
        })

    def setWorkMode(self, stepNum, workMode):
        self.sendCommand(0xCC, stepNum, workMode)

    def setVelocity(self, stepNum, velocity):
        self.sendCommand(0xCD, stepNum, velocity)

    def setPosition(self, stepNum, position):
        self.sendCommand(0xCF, stepNum, position)

    def setAllPosition(self, pos0, pos1, pos2):
        self.sendCommand(0xD1, pos0, pos2, pos1)

    def calibrate(self, stepNum, calibType=CalibrationMode.FAST):
        self.sendCommand(0xD0, stepNum, calibType)


