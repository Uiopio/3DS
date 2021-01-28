from rise.devices.head import Head
from rise.devices.motors import Motors
from rise.cannet.steppercontroller import StepperController
from rise.cannet.motorcontroller import MotorController
from rise.utility.video import Video, VIDEO_OUT_LAUNCH


class JohnyHandle:
    """ Класс, обрабатывающий сообщения и управляющий роботом на самом роботе """

    def __init__(self, robotHandle):
        self._robot = robotHandle
        self._step = StepperController(self._robot, 0x230)
        self._mot = MotorController(self._robot, 0x200)
        self._robot.addDevice(self._step)
        self._robot.addDevice(self._mot)
        self._head = Head(self._step)
        self._motors = Motors(self._mot)
        #self._video = Video()

    def __del__(self):
        try:
            pass
            #self._video.stop()
        except Exception as e:
            print("On destructor", e)

    def start(self):
        self._head.start()
        self._motors.start()

    def setHeadPosition(self, yaw, pitch, roll):
        """ Установка позиции головы робота """
        self._head.setAllPosition(yaw, pitch, roll)

    def setVideoState(self, dev, host, state):
        """ включение/выключение видео """
        if state:
            pass
        else:
            pass

    def calibrateHead(self):
        """ калибровка головы робота """
        self._head.calibrate()

    def move(self, speed):
        """ движение вперед/назад """
        self._motors.move(speed)

    def rotate(self, speed):
        """ поворот на месте """
        self._motors.rotate(speed)

    @property
    def voltage(self):
        return self._motors.voltage

