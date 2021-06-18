from pioneer_sdk import Pioneer
import pygame
import math
import numpy as np
import cv2

from Joystick import Joystick



#######################
"""Параметры Пионера"""
#######################
# начальные координаты(при взлете) в метрах
x = float(0)
y = float(0)
z = float(1)
yaw = math.radians(float(0))

# прирост координат
increment_xy = float(0.1)
increment_z = float(0.025)
increment_deg = math.radians(float(8))

# флаг отправки команды перемещения
new_command = False


#######################
"""Параметры экрана"""
#######################
FPS = 30
W = 640  # ширина экрана
H = 480  # высота экрана


###########################
"""Инициализация Пионера"""
###########################
pioneer_mini = Pioneer(logger= True) # инициализируем пионера
print('start')
pioneer_mini.arm() # запуск моторов
pioneer_mini.takeoff() # предстартовые проверки


pygame.init() # Иницилизация пугейм
pygame.key.set_repeat(1, 20) # Включение обработки зажатой клавиши

sc = pygame.display.set_mode((W, H))
clock = pygame.time.Clock()


##############################
"""Инициализация джойстиков"""
##############################
joy = Joystick()



if __name__ == '__main__':
    running = True
    while running:
        # обработка остановочных нажатий
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_ESCAPE:
                    running = False

        x, y, z, yaw = joy.joyhandler(x, y, z, yaw) # получение координат

        print(round(x, 2), round(y, 2), round(z, 2), round((yaw * 180 / 3.14), 2))  # Вывод координат (просто для удобства)

        # если координаты изменились, то отправляем их
        if joy.new_command:
            pioneer_mini.go_to_local_point(x=round(x, 2), y=round(y, 2), z=round(z, 2), yaw=round(yaw, 2))
            joy.new_command = False

        # вывод картинки с камеры
        frame_cv2 = cv2.imdecode(np.frombuffer(pioneer_mini.get_raw_video_frame(), dtype=np.uint8), cv2.IMREAD_COLOR)
        frame_pygame = pygame.image.frombuffer(frame_cv2.tostring(), (640, 480), "RGB")
        sc.blit(frame_pygame, (0, 0))

        pygame.display.update()

        clock.tick(FPS)

    pygame.quit()  # Завершение работы
    pioneer_mini.land()


