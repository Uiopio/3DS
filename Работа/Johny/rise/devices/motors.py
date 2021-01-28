import time
import can

from rise.cannet.motorcontroller import WorkMode


class Motors:
    def __init__(self, motorController):
        self._motorController = motorController

    def start(self):
        """ Инициализация драйвера моторов """
        self._motorController.setParamRequest(0x01, 0)
        time.sleep(0.1)
        self._motorController.setWorkMode(WorkMode.PID)
        time.sleep(0.1)

    def move(self, speed):
        """ Движение вперед/назад """
        speed = min(max(-100, speed), 100)
        self._motorController.setAllSpeed(speed, speed)

    def rotate(self, speed):
        """ поворот на месте (знак скорости = направление) """
        speed = min(max(-100, speed), 100)
        self._motorController.setAllSpeed(speed, -speed)

    @property
    def voltage(self):
        return self._motorController.getActiveVoltage()


if __name__ == "__main__":
    from rise.cannet.motorcontroller import MotorController
    #from can.interfaces.seeedstudio import seeedstudio
    from rise.cannet.bot import Robot
    bus = can.interface.Bus(channel="can0", bustype='socketcan_native')
    time.sleep(2)
    robot = Robot(bus)
    robot.online = True
    mot = MotorController(robot, 0x200)
    robot.addDevice(mot)
    robot.start()
    motors = Motors(mot)
    motors.start()

    motors.move(0)
    for i in range(10):
        motors.move(30)
        time.sleep(0.2)
    time.sleep(0.5)
    motors.move(0)
    time.sleep(3)
    motors.move(0)
    time.sleep(3)
    motors.rotate(30)
    robot.online = False
    print(motors.voltage)
    time.sleep(6)
    robot.online = True
    print(motors.voltage)
    motors.move(0)
    time.sleep(5)
    motors.move(30)
