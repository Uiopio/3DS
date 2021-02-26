from pioneer_sdk import Pioneer
import pygame
import math
import numpy as np
import cv2



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
FPS = 15
W = 640  # ширина экрана
H = 480  # высота экрана


###########################
"""Инициализация Пионера"""
###########################
pioneer_mini = Pioneer() # инициализируем пионера
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

# Определение количества подключенных джойстиков и их индексов
joystick_count = pygame.joystick.get_count()
ps4 = None
cobra = None
xBox = None


# Выдача джойстикам соответствующих индексов
for i in range (joystick_count):
    joystick = pygame.joystick.Joystick(i)
    print(joystick.get_name())
    if joystick.get_name() == "PS4 Controller":
        ps4 = i
    if joystick.get_name() == "Defender COBRA M5 USB Joystick":
        cobra = i
    if joystick.get_name() == "Xbox 360 Controller":
        xBox = i



if ps4 != None:
    joystick0 = pygame.joystick.Joystick(ps4)  # создаем объект джойстик
    joystick0.init()  # инициализируем джойстик
    print('Джойстик 0 подключен: ' + joystick0.get_name())

else: print("Джойстик не найден")


if cobra != None:
    joystick1 = pygame.joystick.Joystick(cobra)  # создаем объект джойстик
    joystick1.init()  # инициализируем джойстик
    print('Джойстик 1 подключен: ' + joystick1.get_name())

else: print("Джойстик не найден")

if xBox != None:
    joystick2 = pygame.joystick.Joystick(xBox)  # создаем объект джойстик
    joystick2.init()  # инициализируем джойстик
    print('Джойстик 2 подключен: ' + joystick2.get_name())

else: print("Джойстик не найден")




########################
"""Обработка Xbox 360"""
########################
thresholdPS4 = 0.2 # Определяет зону чувствительности стиков: от (-1 ; -thresholdPS4) и (thresholdPS4 ; 1)

def joyMotion2(x, y, z, yaw):
    axis1 = joystick2.get_axis(1)
    axis2 = joystick2.get_axis(2)
    global new_command

    #  левый стик, движение взад вперед
    if (axis1 > thresholdPS4) or (axis1 < -thresholdPS4):
        x += increment_xy * axis1 * math.sin(yaw)
        y += increment_xy * -axis1 * math.cos(yaw)
        new_command = True

    #  правый стик, движение влво вправо
    elif (axis2 > thresholdPS4) or (axis2 < -thresholdPS4):
        x -= increment_xy * axis2 * math.cos(yaw)
        y += increment_xy * axis2 * math.sin(yaw)
        new_command = True

    hat = joystick2.get_hat(0)

    if hat[1] > 0:
        z += increment_z
        new_command = True

    elif hat[1] < 0:
        z -= increment_z
        new_command = True

    elif hat[0] > 0:
        yaw -= increment_deg
        new_command = True

    elif hat[0] < 0:
        yaw += increment_deg
        new_command = True

    return x, y, z, yaw


###################
"""Обработка ps4"""
###################
thresholdPS4 = 0.2 # Определяет зону чувствительности стиков: от (-1 ; -thresholdPS4) и (thresholdPS4 ; 1)

def joyMotion0(x, y, z, yaw):
    axis1 = joystick0.get_axis(1)
    axis2 = joystick0.get_axis(2)
    global new_command

    #  левый стик, движение взад вперед
    if (axis1 > thresholdPS4) or (axis1 < -thresholdPS4):
        x += increment_xy * axis1 * math.sin(yaw)
        y += increment_xy * -axis1 * math.cos(yaw)
        new_command = True

    #  правый стик, движение влво вправо
    elif (axis2 > thresholdPS4) or (axis2 < -thresholdPS4):
        x -= increment_xy * axis2 * math.cos(yaw)
        y += increment_xy * axis2 * math.sin(yaw)
        new_command = True

    if joystick0.get_button(11):
        z += increment_z
        new_command = True

    elif joystick0.get_button(12):
        z -= increment_z
        new_command = True

    elif joystick0.get_button(13):
        yaw += increment_deg
        new_command = True

    elif joystick0.get_button(14):
        yaw -= increment_deg
        new_command = True

    return x, y, z, yaw


#############################
"""Обработка авиаджойстика"""
#############################
threshold = 0.4
def joyMotion1(x, y, z, yaw):

    axis0 = joystick1.get_axis(1) # влево вправо
    axis1 = joystick1.get_axis(0) # взад вперед
    axis2 = joystick1.get_axis(2) # вокруг осей

    global new_command
    global threshold

    # Обработка движения по оси y.
    if (axis0 > threshold) or (axis0 < -threshold):
        x += increment_xy * axis0 * math.sin(yaw)
        y += increment_xy * -axis0 * math.cos(yaw)
        new_command = True


    # Обработка движения по оси х.
    elif (axis1 > threshold) or (axis1 < -threshold):
        x -= increment_xy * axis1 * math.cos(yaw)
        y += increment_xy * axis1 * math.sin(yaw)
        new_command = True


    # Обработка разворота
    elif (axis2 > threshold) or (axis2 < -threshold):
        yaw += increment_deg * -axis2
        new_command = True

    hat = joystick1.get_hat(0)

    if hat[1] > 0:
        z += increment_z
        new_command = True

    if hat[1] < 0:
        z -= increment_z
        new_command = True

    return x, y, z, yaw



###################
"""основной цикл"""
###################
running = True
while running:
    # обработка остановочных нажатий
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                running = False

    # полученеи координат с двух джойстиков
    if ps4 != None:
        x, y, z, yaw = joyMotion0(x, y, z, yaw)

    if cobra != None:
        x, y, z, yaw = joyMotion1(x, y, z, yaw)

    if xBox != None:
        x, y, z, yaw = joyMotion2(x, y, z, yaw)

    print(round(x, 2), round(y, 2), round(z, 2), round((yaw * 180 / 3.14), 2)) # Вывод координат (просто для удобства)

    # если координаты изменились, то отправляем их
    if new_command:
        pioneer_mini.go_to_local_point(x = round(x, 2), y = round(y, 2), z = round(z, 2), yaw = round(yaw, 2))
        new_command = False

    # вывод картинки с камеры
    frame_cv2 = cv2.imdecode(np.frombuffer(pioneer_mini.get_raw_video_frame(), dtype=np.uint8), cv2.IMREAD_COLOR)
    frame_pygame = pygame.image.frombuffer(frame_cv2.tostring(), (640, 480), "RGB")
    sc.blit(frame_pygame, (0, 0))

    pygame.display.update()

    clock.tick(FPS)

pygame.quit() #Завершение работы
pioneer_mini.land()

