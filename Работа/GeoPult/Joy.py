from pioneer_sdk import Pioneer
import pygame
import numpy as np
import cv2
import sys

#pioneer_mini = Pioneer()


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




FPS = 30
W = 640  # ширина экрана
H = 480  # высота экрана
WHITE = (255, 255, 255)
BLUE = (0, 70, 225)





pygame.init() # Иницилизация пугейм
pygame.key.set_repeat(1, 20) # Включение обработки зажатой клавиши

sc = pygame.display.set_mode((W, H))
size = W, H

square=pygame.Surface((480, 640))


clock = pygame.time.Clock()

# координаты и радиус круга
x = W // 2
y = H // 2
r = 50

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
            # print(event)
            if event.axis == 1:
                if event.value > 0.1:
                    print("up", event.value)
                elif event.value < -0.1:
                    print("down", event.value)

        # Обработка нажатия кнопок джойстика
        elif event.type == pygame.JOYBUTTONDOWN:
            # Крестик
            if event.button == crossButton:
                print("crossButton")
            #
            if event.button == circleButton:
                print("circleButton")

            if event.button == squareButton:
                print("squareButton")

            if event.button == triangleButton:
                print("triangleButton")




    #frame_cv2 = cv2.imdecode(np.frombuffer(pioneer_mini.get_raw_video_frame(), dtype=np.uint8), cv2.IMREAD_COLOR)

    #frame_pygame = pygame.image.frombuffer(frame_cv2.tostring(), (640, 480), "RGB")


    sc.fill(WHITE)
    pygame.draw.circle(sc, BLUE, (x, y), r)
    #sc.blit(frame_pygame, (0,0))
    pygame.display.update()
    clock.tick(FPS)


pygame.quit() #Завершение работы