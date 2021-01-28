import json
import datetime
import gi
import threading
import time

from rise.devices.glass import Glass
from rise.pult.robot import Johny
from rise.devices.joystick import Joystick
from rise.pult.interface.rtspVideo import RtspVideo, VIDEO_IN_LAUNCH

#from rise.pult.interface.videowindow import VideoWindow

gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, GLib, Gdk


class _SettingsWindow:
    def __init__(self, pult):
        self._owner = pult
        self._builder = Gtk.Builder()
        self._builder.add_from_file("rise/pult/interface/interface.glade")
        self._settingsWindow = self._builder.get_object("settingsWindow")
        self._calibrateButton = self._builder.get_object("calibrateButton")
        self._videoSwitch = self._builder.get_object("videoSwitch")
        self._settingsChooserButton = self._builder.get_object("settingsChooserButton")

        self._settingsChooserButton.connect("file-set", self.__confFilePathChange)
        self._calibrateButton.connect("clicked", self.__calibrateButtonClick)
        self._videoSwitch.connect("state-set", self.__videoSwitchClick)
        self._settingsWindow.connect("delete-event", self.__delete_event)

        self._calibrateButton.set_property("sensitive", self._owner.isConnected)
        self._videoSwitch.set_property("sensitive", self._owner.isConnected)
        self._settingsChooserButton.set_property("sensitive", not self._owner.isConnected)

        self._settingsWindow.show_all()


    def __calibrateButtonClick(self, w):
        self._owner.robot.calibrateHead()

    def __videoSwitchClick(self, w, state):
        self._owner.robot.videoState(state)
        time.sleep(1)
        """
        try:
            if state:
                if self._owner._videoWindow is None:
                    #self._owner._videoWindow = VideoWindow(self._owner._configuration["robot"]["ip"])
                    #self._owner._videoWindow.start()
            else:
                if self._owner._videoWindow is not None:
                    self._owner._videoWindow.stop()
                    del self._owner._videoWindow
                    self._owner._videoWindow = None
        except Exception as e:
            self._owner.printLog("Произошла ошибка при работе с видео: " + str(e))
        time.sleep(2)
        """

    def __confFilePathChange(self, w):
        try:
            self._owner.setConfigurationFromFile(w.get_uri()[6:])
        except:
            self._owner.printLog("Ошибка чтения файла конфигурации, проверьте его корректность")
        else:
            self._owner._onoffButton.set_property("sensitive", True)

    def __delete_event(self, widget, event, data=None):
        self._owner._settingsButton.set_property("sensitive", True)

    def destroy(self):
        self._settingsWindow.destroy()


class Pult:
    def __init__(self):
        """ развертываем интерфейс из glade """
        self._defaultConfigurationFilePath = "conf.json"
        self._configuration = None
        self._isConnected = False
        self.__exit = False
        self.robot = Johny(None)
        self._joystick = None
        self._helmet = Glass("/dev/ttyUSB0")

        self.video = RtspVideo()

        self._helmet.connect("READ", lambda y, p, r: None)

        self._helmet.start()

        self._builder = Gtk.Builder()
        self._builder.add_from_file("rise/pult/interface/interface.glade")

        self._mainWindow = self._builder.get_object("mainWindow")
        self._onoffButton = self._builder.get_object("onoffButton")
        self._settingsButton = self._builder.get_object("settingsButton")
        self._logTextview = self._builder.get_object("logTextview")

        #self._videoWindow = None

        self._mainWindow.connect("delete-event", self.__delete_event)

        self._onoffButton.connect("toggled", self.__onoffButtonClick)
        self._settingsButton.connect("clicked", self.__settingsButtonClick)
        # self.printLog("*** Hello, I'm Johny! ***")

        try:
            self.setConfigurationFromFile(self._defaultConfigurationFilePath)
        except FileNotFoundError:
            self.printLog("Файл конфигурации по умолчанию не найден, проверьте его наличие или выберете другой")
        except:
            self.printLog("Ошибка чтения файла конфигурации, проверьте его корректность")
        else:
            self._onoffButton.set_property("sensitive", True)

        threading.Thread(daemon=True,
                         target=self.__cyclicSending).start()  # запускаем поток циклических отправок данных
        threading.Thread(daemon=True,
                         target=self.__cyclicCheckError).start()  # запускаем поток циклической проверки ошибок
        self._mainWindow.show_all()
        Gtk.main()

    @property
    def isConnected(self):
        return self._isConnected

    def printLog(self, string):
        end_iter = self._logTextview.get_buffer().get_end_iter()  # получение итератора конца строки
        self._logTextview.get_buffer().insert(end_iter, str(datetime.datetime.now()) + "\t" + string + "\n")

    def __onoffButtonClick(self, w):
        state = w.get_active()
        if state:
            try:
                self.__openJoystick(self._configuration["joystick"]["path"])
                self.printLog("Джойстик подключен")
            except:
                self.printLog("Не удалось открыть джойстик")
            try:
                #self._helmet.reset()
                self.printLog("Попытка подключения к роботу с host:" + self.robot.host.__repr__() + "...")
                self.robot.connect()
                self._isConnected = True
                self.printLog("Попытка подключения удалась")
                #self._videoWindow =  (self._configuration["robot"]["ip"])
                self.video.start(VIDEO_IN_LAUNCH)

                time.sleep(1)
                #self._videoWindow.start()
                self.printLog("HDMI шлема VR найден и подключен")
                self.__robotOn()
            except ConnectionError:
                self.printLog("Не удается подключиться к роботу с адресом: " + self.robot.host.__repr__())
                w.set_active(False)
            except ValueError:
                self.printLog("Не удалось найти HDMI шлема VR")
                w.set_active(False)
            except Exception as e:
                self.printLog(e.__str__())
                w.set_active(False)
        else:
            try:
                self._isConnected = False
                self.__robotOff()
                self.robot.disconnect()
                self.printLog("Робот отключен")
                """
                if self._videoWindow is not None:
                    self._videoWindow.stop()
                    del self._videoWindow
                    self._videoWindow = None
                """
                self.__closeJoystick()
            except BrokenPipeError:
                self.printLog("Связь была прервана")
            except Exception as e:
                self.printLog(e.__str__())

    def __settingsButtonClick(self, w):
        self._settingsButton.set_property("sensitive", False)
        _SettingsWindow(self)

    def __delete_event(self, widget, event, data=None):
        Gtk.main_quit()

    def setConfigurationFromFile(self, path):
        temp = None
        with open(path, "r") as file:
            self._configuration = json.load(file)
            try:
                self.robot.host = (self._configuration["robot"]["ip"], self._configuration["robot"]["port"])
            except KeyError:
                self.printLog("Файл конфигурации не содержит адрес робота")
                raise KeyError()
            try:
                _ = self._configuration["joystick"]["path"]
            except KeyError:
                self.printLog("Файл конфигурации не содержит имя джойстика")
                raise KeyError()
            try:
                temp = "ROTATE_AXIS"
                _ = self._configuration["joystick"][temp]
                temp = "MOVE_AXIS"
                _ = self._configuration["joystick"][temp]
                temp = "ROTATE_AXIS_PRESC"
                _ = self._configuration["joystick"][temp]
                temp = "MOVE_AXIS_PRESC"
                _ = self._configuration["joystick"][temp]
                temp = "SET_HELMET_ZERO_BTN"
                _ = self._configuration["joystick"][temp]
                temp = "ADD_SPEED_BTN"
                _ = self._configuration["joystick"][temp]
                temp = "SUB_SPEED_BTN"
                _ = self._configuration["joystick"][temp]
                temp = "SPEED_CHANGE_STEP"
                _ = self._configuration["joystick"][temp]
            except:
                self.printLog("В файле конфигурации не назначено значение для " + temp)
                raise KeyError()

    def __robotOn(self):
        """ вызывается после соединения с роботом """
        self.robot.calibrateHead()
        self.robot.videoState(True)
        #self._helmet.setZeroNow()

    def __robotOff(self):
        """ вызывается перед разъединением с роботом """
        self.robot.videoState(False)

    def __openJoystick(self, path):
        temp = None
        self._joystick = Joystick()
        self._joystick.open(path)
        try:
            #temp = self._configuration["joystick"]["SET_HELMET_ZERO_BTN"]
            #_ = self._joystick.buttons[temp]
            #temp = self._configuration["joystick"]["ADD_SPEED_BTN"]
            #_ = self._joystick.buttons[temp]
            #temp = self._configuration["joystick"]["SUB_SPEED_BTN"]
            #_ = self._joystick.buttons[temp]
            temp = self._configuration["joystick"]["ROTATE_AXIS"]
            _ = self._joystick.axis[temp]
            temp = self._configuration["joystick"]["MOVE_AXIS"]
            _ = self._joystick.axis[temp]
        except:
            self.printLog("Не получается найти на джойстике: " + temp)
            self.__closeJoystick()
        else:
            #self._joystick.onButtonClick(self._configuration["joystick"]["SET_HELMET_ZERO_BTN"],
            #                             lambda x: self._helmet.setZeroNow() if x else None)
            #self._joystick.onButtonClick(self._configuration["joystick"]["ADD_SPEED_BTN"],
            #                             lambda x: self.robot.addToSpeed(
            #                                 self._configuration["joystick"]["SPEED_CHANGE_STEP"]) if x else None)
            #self._joystick.onButtonClick(self._configuration["joystick"]["SUB_SPEED_BTN"],
            #                             lambda x: self.robot.addToSpeed(
            #                                 -self._configuration["joystick"]["SPEED_CHANGE_STEP"]) if x else None)
            self._joystick.start()

    def __closeJoystick(self):
        self._joystick.exit()
        del self._joystick
        self._joystick = None

    def __cyclicSending(self):
        print(1)
        i = 0
        j = 0
        while not self.__exit:
            #print(2)
            if self._isConnected:
                try:
                    #yaw, pitch, roll = self.glass.getAngles()
                    angle = self._helmet.angleHead
                    yaw = int(angle[0])
                    pitch = int(angle[1])
                    roll = int(angle[2])
                    # print(yaw, pitch, roll)
                    self.robot.setHeadPosition(yaw, pitch, roll)
                except BrokenPipeError:
                    self.printLog("Ошибка при отправке пакета с данными углов головы робота")
                    self._onoffButton.set_active(False)
                except Exception as e:
                    self.printLog("Ошибка при отправке пакета с данными углов головы робота: " + e.__repr__())
                try:
                    if i > 4:  # в 3 раза меньше пакетов шлется
                        if abs(int(self._joystick.axis[self._configuration["joystick"]["MOVE_AXIS"]] * 100)) > 0:
                            # если ось немного битая, то умножаем на 100
                            self.robot.move(self._configuration["joystick"]["MOVE_AXIS_PRESC"] * \
                                            self._joystick.axis[self._configuration["joystick"]["MOVE_AXIS"]])
                        else:
                            self.robot.rotate(self._configuration["joystick"]["ROTATE_AXIS_PRESC"] * \
                                              self._joystick.axis[self._configuration["joystick"]["ROTATE_AXIS"]])
                        i = 0
                    i = i + 1
                except BrokenPipeError:
                    self.printLog("Ошибка при отправке пакета с данными движения робота")
                    self._onoffButton.set_active(False)
                except Exception as e:
                    self.printLog("Ошибка при отправке пакета с данными движения робота: " + e.__repr__())
                try:
                    if j > 8:  # в 8 раз меньше пакетов шлется
                        self.robot.sendOnline()
                        j = 0
                    j = j + 1
                except BrokenPipeError:
                    self.printLog("Ошибка при отправке пакета с онлайн меткой")
                    self._onoffButton.set_active(False)
                except Exception as e:
                    self.printLog("Ошибка при отправке пакета с онлайн меткой: " + e.__repr__())
            time.sleep(0.1)

    def __cyclicCheckError(self):
        while not self.__exit:
            try:
                for error in self.robot.errorList:
                    end_iter = self._logTextview.get_buffer().get_end_iter()  # получение итератора конца строки
                    self._logTextview.get_buffer().insert(end_iter, str(error["time"]) + "\t" + \
                                                          error["num"].__repr__() + "/" + error["dlc"].__repr__() + \
                                                          "\t" + error["desc"] + "\n")
                self.robot.errorList.clear()
            except:
                self.printLog("Произошла ошибка при сканировании ошибок")
            time.sleep(3)
