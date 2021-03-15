import pygame
import math

class Joystick:


    def __init__(self):
        self.joystick_count = pygame.joystick.get_count() # количество подключенных джойстиков
        self.joyInd = [None, None, None] # массив хранения индексов джойстиков. Порядок храннения: PS4, cobra, xBox

        # Выдача джойстикам соответствующих индексов
        for i in range(self.joystick_count):
            joystick = pygame.joystick.Joystick(i)
            print(joystick.get_name())
            if joystick.get_name() == "PS4 Controller":
                self.joyInd[0] = i
            if joystick.get_name() == "Defender COBRA M5 USB Joystick":
                self.joyInd[1] = i
            if joystick.get_name() == "Xbox 360 Controller":
                self.joyInd[2] = i

        self.joystick = [None, None, None] # Массив хрнения джойстиков. Порядок храннения: PS4, cobra, xBox
        for i in range(3):
            if self.joyInd[i] != None:
                self.joystick[i] = pygame.joystick.Joystick(self.joyInd[i])  # создаем объект джойстик
                self.joystick[i].init()  # инициализируем джойстик
                print('Джойстик 0 подключен: ' + self.joystick[i].get_name())
            else:
                print("Джойстик не найден")

        # скорость изменения координат
        self.increment_xy = float(0.1)
        self.increment_z = float(0.025)
        self.increment_deg = math.radians(float(8))
        self.new_command = False


    def joyhandler(self, x, y, z, yaw):
        return self.ps4_xbox(0.2, x, y, z, yaw)


    def ps4_xbox(self, threshold, x, y, z, yaw):
        for i in range(self.joystick_count):
            if self.joyInd != None:
                axis0 = self.joystick[i].get_axis(0)
                axis1 = self.joystick[i].get_axis(1)
                axis2 = self.joystick[i].get_axis(2)
                axis3 = self.joystick[i].get_axis(3)

                #  левый стик, движение влево вправо
                if (axis1 > threshold) or (axis1 < -threshold):
                    x += self.increment_xy * axis1 * math.sin(yaw)
                    y += self.increment_xy * -axis1 * math.cos(yaw)
                    self.new_command = True

                #  левый стик, движение вперед назад
                elif (axis0 > threshold) or (axis0 < -threshold):
                    x += self.increment_xy * axis0 * math.cos(yaw)
                    y += self.increment_xy * axis0 * math.sin(yaw)
                    self.new_command = True

                #  правый стик, движение влево вправо
                if (axis2 > threshold) or (axis2 < -threshold):
                    yaw += self.increment_deg * axis2
                    self.new_command = True

                #  правый стик, движение вверх вниз
                elif (axis3 > threshold) or (axis3 < -threshold):
                    z += self.increment_z * -axis3
                    self.new_command = True

        return x,y,z,yaw


