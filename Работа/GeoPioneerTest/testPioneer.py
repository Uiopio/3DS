from pioneer_sdk import Pioneer
import cv2
import numpy as np
import time
import requests
import pandas as pd


def check(question):
    print(question)
    answer = input()

    if answer == "+":
        return True
    else:
        return False


def startFunction(table, nameTest, string):
    print("#################################")
    table.append(nameTest)
    print(string)

######################
"""Проверка моторов"""
######################
def arm(pioneer, table):
    startFunction(table, "Arm: ", "Проверка включения моторов")

    pioneer.arm()  # запуск моторов

    result = check("Запустились ли моторы?")
    table.append(str(result))


def disarm(pioneer, table):
    startFunction(table, "Disarm: ", "Проверка выключения моторов")

    pioneer.disarm()  # остановка моторов

    result = check("Остановились ли моторы?")
    table.append(str(result))


##########################
"""Проверка светодиодов"""
##########################
def led(pioneer, table):
    startFunction(table, "Led: ", "Проверка работы светодиодов")

    pioneer.led_control(r=255, g=0, b=0)
    time.sleep(2)
    pioneer.led_control(r=0, g=255, b=0)
    time.sleep(2)
    pioneer.led_control(r=0, g=0, b=255)
    time.sleep(1)
    pioneer.led_control(r=0, g=0, b=0)

    result = check("Светодиоды меняли цвет?")
    table.append(str(result))


#####################
"""Проверка камеры"""
#####################
def cam(pioneer, table):
    startFunction(table, "Cam: ", "Проверка работы камеры")
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
    table.append(str(result))


##################################################
"""Проверка оптического потка, взлета и посадки"""
##################################################
def takeoff(pioneer, table):
    startFunction(table, "Takeoff: ", "Проверка оптического потока и взлета")

    pioneer.arm()
    pioneer.takeoff()

    result1 = check("Пионер взлетел и завис в воздухе?")
    table.append(str(result1))

    table.append("Vertical: ")
    result2 = check("Пионер взлетел вертикально? (пионер должен ~находиться над точкой взлета)")
    table.append(str(result2))


def land(pioneer, table):
    startFunction(table, "land: ", "Проверка посадки")

    pioneer.land()
    result = check("Пионер сел?")
    table.append(str(result))


#############################
"""Проверка полета в точку"""
#############################
def flight(pioneer, table):
    startFunction(table, "Flight to point: ", "Проверка полета в точку")

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

    pioneer.go_to_local_point(x=0, y=0, z=1, yaw=0)
    result = check("Пионер пролетел вперед и назад ~1 метр?")
    table.append(str(result))

    table.append("P_r: ")
    table.append(str(p_r))



##################################
"""Проверка запуска луа скрипта"""
##################################
def luaScript(pioneer, table):
    startFunction(table, "Lua script: ", "Проверка запуска луа скрипта для мигания светодиодами")

    pioneer.lua_script_control('Start')

    lua = check("Светодиоды мигают?")
    pioneer.lua_script_control('Stop')
    pioneer.led_control(r=0, g=0, b=0)
    table.append(str(lua))


###########################################
"""Проверка управления с помощью каналов"""
##########################################
# h, yaw, pitch, roll, mode
def rcChannels(pioneer, table):
    startFunction(table, "RC_Channels: ", "Проверка управления по RC каналу")

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
            break

        time.sleep(0.05)

    result = check("Коптер взлетел вверх и сделал оборот?")
    table.append(str(result))


######################################
"""Проверка работы сенора дистанции"""
######################################
def distSensor(pioneer, table):
    startFunction(table, "Dist sensor: ", "Проверка работы датчика высоты")

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

    result = check("Светодиоды меняли цвет в зависимости от высоты?")
    table.append(str(result))
    pioneer.led_control(r=0, g=0, b=0)


def disconnect(pioneer, table):
    startFunction(table, "DFailsafe: ", "Проверка посадки при потере сигнала")

    pioneer.arm()
    pioneer.takeoff()

    print("Отключите wifi соединение с комптером, пока он в воздухе")
    time.sleep(3)
    result = check("Коптер совершил посадку?")
    table.append(str(result))



if __name__ == '__main__':

    print('Инициализация пионера\n')
    pioneer_mini = Pioneer(logger=False)  # инициализируем пионера


    table = []

    version = requests.get('http://192.168.4.1/info')


    print('Для положительного ответа нажмите введите "+" и нажмите enter')

    #led(pioneer_mini, table)
    #distSensor(pioneer_mini, table)
    #cam(pioneer_mini, table)
    arm(pioneer_mini, table)
    disarm(pioneer_mini, table)
    #takeoff(pioneer_mini, table)
    #flight(pioneer_mini, table)
    #luaScript(pioneer_mini, table)
    #rcChannels(pioneer_mini, table)
    #disconnect(pioneer_mini, table)
    #land(pioneer_mini, table)

    print("########## РЕЗУЛЬТАТ ##########")

    time = time.strftime("%d-%m-%Y %H:%M:%S", time.localtime())
    print(time)


    versionStr = str(version.text)
    ind = versionStr.find(",")

    print("{:25s} {:6s} ".format("ВЕРСИЯ ESP32:", str(versionStr[1:ind]) ) )
    print("{:25s} {:6s} ".format("Версия автопилота: ", str(versionStr[ind + 2: len(versionStr) - 1]) ) )


    for i in range(0, len(table), 2):
        print("{:25s} {:6s} ".format(table[i], table[i + 1]))

    resultTable = pd.DataFrame(columns=(0,1))

    resultTable.loc[0] = "time", time
    resultTable.loc[1] = "ESP32:", str(versionStr[1:ind])
    resultTable.loc[2] = "autopilot: ", str(versionStr[ind + 2: len(versionStr) - 1])
    line = 3
    for i in range(0, len(table), 2):
        str = []
        str.append(table[i])
        str.append(table[i+1])
        resultTable.loc[line] = str
        line = line + 1

    resultTable.to_csv("./test.csv", index=None, header=False)