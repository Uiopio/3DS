import threading
import smbus
import time
from edubot import EduBot

if __name__ == "__main__":
    bus = smbus.SMBus(1)
    bot = EduBot(bus)
    bot.start()
    print(bot.whoIam())
    bot.setMotorMode(MotorMode.MOTOR_MODE_PID)

    # движение вперед в течении 5 секунд
    bot.setParrot0(-45)
    bot.setParrot1(45)
    time.sleep(3)

    # Разворот вокруг оси
    bot.setParrot0(30)
    bot.setParrot1(30)
    time.sleep(2)

    # Разворот вокруг оси
    bot.setParrot0(-30)
    bot.setParrot1(-30)
    time.sleep(2)

    # остановка моторов
    bot.setParrot0(0)
    bot.setParrot1(0)
    time.sleep(1)


    # Движение камеры
    for i in range(0, 200, 8):
        bot.setServo0(i)
        time.sleep(0.1)

    # начальное положение сервы
    bot.setServo0(120)
    time.sleep(5)
    bot.exit()
