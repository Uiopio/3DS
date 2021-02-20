from pioneer_sdk import Pioneer
import pygame
import math
import numpy as np
import cv2

#######################
"""Параметры Пионера"""
#######################
command_x = float(0)
command_y = float(0)
command_z = float(1)
command_yaw = math.radians(float(0))

increment_xy = float(0.2) # прирост координат
increment_z = float(0.1)

increment_deg = math.radians(float(90))
new_command = False


########################################
"""Номера кнопок для джойстика от PS4"""
########################################
crossButton = 0
circleButton = 1
squareButton = 2
triangleButton = 3
shareButton = 4
psButton = 5
optionsButton = 6
lStick = 7
rStick = 8
leftBumper = 9
rightBumper = 10
up = 11
down = 12
left = 13
right = 14
touchPadClick = 15



#######################
"""Параметры экрана"""
#######################
FPS = 30
W = 640  # ширина экрана
H = 480  # высота экрана
WHITE = (255, 255, 255)
BLUE = (0, 70, 225)


###########################
"""Инициализация Пионера"""
###########################
pioneer_mini = Pioneer()
print('start')
pioneer_mini.arm()
pioneer_mini.takeoff()

pygame.init() # Иницилизация пугейм
pygame.key.set_repeat(1, 20) # Включение обработки зажатой клавиши
sc = pygame.display.set_mode((W, H))
clock = pygame.time.Clock()



#############################
"""Инициализация джойстика"""
#############################
try:
    joystick = pygame.joystick.Joystick(0) # создаем объект джойстик
    joystick.init() # инициализируем джойстик
    print('Enabled joystick: ' + joystick.get_name())
except pygame.error:
    print('no joystick found.')


running = True
while running:

    #######################
    """Обработка нажатий"""
    #######################
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

        # Обработка нажатия кнопок клавиатуры
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_DOWN:
                y += 3
            elif event.key == pygame.K_UP:
                y -= 3
            elif event.key == pygame.K_LEFT:
                x -= 3
            elif event.key == pygame.K_RIGHT:
                x += 3

        # Обработка движения стиков джойстика
        elif event.type == pygame.JOYAXISMOTION:  # перемещение стиков

            if event.axis == 1: # Левый сосочек, вверх вниз

                if event.value > 0.1:
                    command_y += increment_xy * event.value
                    new_command = True
                    print("upL", event.value)

                elif event.value < -0.1:
                    command_y -= increment_xy * event.value
                    new_command = True
                    print("downL", event.value)

            if event.axis == 2: # Правый сосочек, влево вправо
                if event.value > 0.1:
                    command_x += increment_xy * event.value
                    new_command = True
                    print("rightR", event.value)

                elif event.value < -0.1:
                    command_x -= increment_xy * event.value
                    new_command = True
                    print("leftR", event.value)

        # Обработка нажатия кнопок джойстика
        elif event.type == pygame.JOYBUTTONDOWN:
            # нажатие на тачпад
            if event.button == touchPadClick:
                running = False

            elif event.button == crossButton:
                running = False
                print("crossButton")

            elif event.button == circleButton:
                running = False
                print("circleButton")

            elif event.button == squareButton:
                running = False
                print("squareButton")

            elif event.button == triangleButton:
                running = False
                print("triangleButton")

            elif event.button == up:
                command_z += increment_z
                new_command = True
                print("up")

            elif event.button == down:
                command_z -= increment_z
                new_command = True
                print("down")

            elif event.button == left:
                command_yaw += increment_deg
                new_command = True
                print("left")

            elif event.button == right:
                command_yaw -= increment_deg
                new_command = True
                print("right")


    if new_command:
        print("newCommand")
        pioneer_mini.go_to_local_point(x=command_x, y=command_y, z=command_z, yaw=command_yaw)
        new_command = False


    frame_cv2 = cv2.imdecode(np.frombuffer(pioneer_mini.get_raw_video_frame(), dtype=np.uint8), cv2.IMREAD_COLOR)
    frame_pygame = pygame.image.frombuffer(frame_cv2.tostring(), (640, 480), "RGB")
    sc.blit(frame_pygame, (0, 0))

    pygame.display.update()
    clock.tick(FPS)

pioneer_mini.land()
pygame.quit() #Завершение работы