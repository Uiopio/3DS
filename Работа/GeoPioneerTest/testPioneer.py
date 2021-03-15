from pioneer_sdk import Pioneer
import cv2
import numpy as np
import time
import requests

print('Инициализация пионера')
pioneer_mini = Pioneer(logger=False)  # инициализируем пионера


def check(question):
    print(question)
    answer = input()

    if answer == "+":
        return True
    else:
        return False


######################
"""Проверка моторов"""
######################
def arm(pioneer, table):
    table.append("Arm: ")
    print('Проверка включения моторов')

    pioneer.arm()  # запуск моторов

    result = check("Запустились ли моторы?")
    table.append(result)


def disarm(pioneer, table):
    table.append("Disarm:")
    print('Проверка выключения моторов')

    pioneer.disarm()  # остановка моторов

    result = check("Остановились ли моторы?")
    table.append(result)


##########################
"""Проверка светодиодов"""
##########################
def led(pioneer, table):
    table.append("Led: ")
    print('Проверка работы светодиодов')

    pioneer.led_control(r=255, g=0, b=0)
    time.sleep(2)
    pioneer.led_control(r=0, g=255, b=0)
    time.sleep(2)
    pioneer.led_control(r=0, g=0, b=255)
    time.sleep(1)
    pioneer.led_control(r=0, g=0, b=0)

    result = check("Светодиоды меняли цвет?")
    table.append(result)


#####################
"""Проверка камеры"""
#####################
def cam(pioneer, table):
    table.append("Cam: ")
    print('Проверка работы камеры')
    print('Нажмите ESC дял закрытия окна с видео')

    while True:
        camera_frame = cv2.imdecode(np.frombuffer(pioneer.get_raw_video_frame(), dtype=np.uint8), cv2.IMREAD_COLOR)
        cv2.imshow('pioneer_camera_stream', camera_frame)

        key = cv2.waitKey(1)
        if key == 27:  # esc
            print('esc pressed')
            cv2.destroyAllWindows()
            break

    result = check("Появилось ли видео с камеры?")
    table.append(result)


##################################################
"""Проверка оптического потка, взлета и посадки"""
##################################################
def takeoff(pioneer, table):
    table.append("Takeoff, vertical: ")
    print('Проверка оптического полета и взлета')

    pioneer.arm()
    pioneer.takeoff()

    flight = check("Пионер взлетел и завис в воздухе?")
    vertical = check("Пионер взлетел вертикально? (пионер должен находиться над точкой взлета)")

    result = [flight, vertical]
    table.append(result)


def land(pioneer, table):
    table.append("land: ")
    print('Проверка посадки')
    pioneer.land()


#############################
"""Проверка полета в точку"""
#############################
def flight(pioneer, table):
    table.append("Flight to point: ")
    print('Проверка полета в точку')

    pioneer.go_to_local_point(x=0, y=1, z=1, yaw=0)
    p_r = False
    startTime = time.time()

    while True:
        deltaTime = time.time() - startTime
        if pioneer.point_reached():
            p_r = True
            break
        if deltaTime > 5:
            break

    flight = check("Пионер полетел вперед на ~1 метр?")
    result = [p_r, flight]
    table.append(result)



##################################
"""Проверка запуска луа скрипта"""
##################################
def luaScript(pioneer, table):
    table.append("Lua script: ")

    print("Проверка запуска луа скрипта для мигания светодиодами")
    pioneer.lua_script_control('Start')

    lua = check("Светодиоды мигают?")
    pioneer.lua_script_control('Stop')
    pioneer.led_control(r=0, g=0, b=0)
    table.append(lua)


###########################################
"""Проверка управления с помощью каналов"""
##########################################
# h, yaw, pitch, roll, mode
def rcChannels(pioneer, table):
    table.append("RC_Channels: ")

    print("Проверка управления по RC каналу")
    h = 1500
    yaw = 1500
    pitch = 1500
    roll = 1500
    mode = 2000

    pioneer.arm()  # запуск моторов
    time.sleep(1)
    pioneer.takeoff()
    time.sleep(2)
    startTime = time.time()
    while True:
        pioneer.send_rc_channels(h, yaw, pitch, roll, mode, 0xFFFF, 0xFFFF, 0xFFFF)

        deltaTime = time.time() - startTime

        if (deltaTime > 1) and (deltaTime < 3):
            h= 1700
        elif (deltaTime > 3) and (deltaTime < 7):
            h = 1500
            yaw = 1700
        elif(deltaTime > 7):
            yaw = 1500
            pioneer.land()
            break

        time.sleep(0.05)

    rc = check("Коптер взлетел вверх и сделал оборот?")
    table.append(rc)


######################################
"""Проверка работы сенора дистанции"""
######################################
def distSensor(pioneer, table):
    table.append("Dist sensor: ")
    print("Проверка работы сенсора высоты")
    print("Возьмите коптер в руку и медленно поднимити его вертикально. В зависимости от высоты сетодиоды будут менять цвет")
    print(" >30см = красный цвет")
    print(" >50см = зеленый цвет")
    print(" >100см = синий цвет")
    low = 0.3
    mid = 0.5
    high = 0.75
    stop = 1.0
    r = 0
    g = 0
    b = 0
    startTime = time.time()
    deltaTime = 0.05
    while True:
        if time.time()-startTime > deltaTime:
            tof_data = pioneer.get_dist_sensor_data()
            if tof_data is not None:
                if low <= tof_data < mid:
                    r = 255
                    g = 0
                    b = 0
                if mid <= tof_data < high:
                    r = 0
                    g = 255
                    b = 0
                if tof_data >= high:
                    r = 0
                    g = 0
                    b = 255
                if tof_data >= stop:
                    break

            pioneer.led_control(r=r, g=g, b=b)
            startTime = time.time()

    dist = check("Светодиоды меняли цвет в зависимости от высоты?")
    table.append(dist)
    pioneer.led_control(r=0, g=0, b=0)


def disconnect(pioneer, table):
    pass


if __name__ == '__main__':

    version = requests.get('http://192.168.4.1/info')
    print("ВЕРСИЯ ТЕСТИРУЕМОЙ ПРОШИВКИ: ", version.text)
    
    print('Для положительного ответа нажмите введите "+" и нажмите enter')
    table = []

    led(pioneer_mini, table)
    distSensor(pioneer_mini, table)
    cam(pioneer_mini, table)
    arm(pioneer_mini, table)
    disarm(pioneer_mini, table)
    takeoff(pioneer_mini, table)
    flight(pioneer_mini, table)
    luaScript(pioneer_mini, table)
    rcChannels(pioneer_mini, table)


    for i in range(0, len(table), 2):
        print(table[i], table[i + 1])

    land(pioneer_mini)
    print("конец теста")
