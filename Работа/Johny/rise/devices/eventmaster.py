#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import threading
import queue


class EventError(Exception):
    """ исключение события """
    pass


class Event:
    """ Класс представляющий событие """
    def __init__(self, name=None, foo=None):
        self._name = name  # имя события
        self._foo = foo  # функция-обработчик события
        self.__f = lambda: None  # дополнительная переменная - хранение ссылки на обработчик события
        self._eventMaster = None    # ссылка на EventMaster, сделано для оптимизации

    def connect(self, foo):
        """ Установка обработчика события """
        if callable(foo):   # если привязывается ф-ия
            self.__f = foo
        else:
            raise TypeError("parameter must be a function")

    def push(self, *args):
        """ Вызов события """
        self._foo = lambda: self.__f(*args)     # оборачиваем вызов ф-ии с параметрами в пустую ф-ию
        if self._eventMaster:
            self._eventMaster.pull(self._foo)

    def _attachEventMaster(self, eventMaster):
        """ Устанавливает ссылку на мастер событий """
        if not isinstance(eventMaster, EventMaster):    # тип должен быть EventMaster
            raise AttributeError("parameter must be a EventMaster")
        if self._eventMaster is not None:   # если мастер событий уже привязан
            raise AttributeError("EventMaster already attached")
        else:
            self._eventMaster = eventMaster


class EventMaster(threading.Thread):
    """ Класс отвечающий за работу событий """
    def __init__(self):
        threading.Thread.__init__(self, daemon=True)
        self._eventList = []  # список всех событий, добавленных в EventMaster
        self._eventHandlersQueue = queue.Queue()  # очередь на выполнения обработчиков событий
        self.__exit = False  # метка выхода из потока

    def run(self):  # функция потока EM
        while not self.__exit:  # пока нет метки выхода из потока
            threading.Thread(daemon=True, target=self._eventHandlersQueue.get()).start()    # вызов обработчика события
            # в отдельном потоке с блокированием цикла

    def exit(self):
        """ функция выхода из потока """
        self.__exit = True
        # TODO: Если не будет выходить из-за блокировки - сделать пустое событие и вызывать его тут

    def append(self, event):
        """ добавления нового элемента EventBlock в список возможных событий """
        if isinstance(event, Event):    # тип должен быть Event
            event._attachEventMaster(self)   # привязвть событие к мастеру событий а мастер событий
            #  к обработчику событий, приходится нарушать инкапсуляцию, пока не знаю, как решить такую проблему
            self._eventList.append(event)
        else:
            raise TypeError("parameter must be a Event")

    def pull(self, handler):
        """ перетащить событие в очередь на выполнение обработчика """
        if callable(handler):    # handler должен быть вызываемым
            self._eventHandlersQueue.put(handler)     # поставить событие в очередь на выполнение обработчиков
        else:
            raise TypeError("parameter must be a callable")
