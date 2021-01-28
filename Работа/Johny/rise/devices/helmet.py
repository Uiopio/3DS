#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from math import *
import time
from openhmd import PyOpenHMD
import numpy as np


class Helmet:
    """ Класс шлема VR """

    def __init__(self):
        self._hmd = PyOpenHMD()
        self._start = True     # флаг, означающий, что очки могут определять углы

    def getRawAngles(self):
        # TODO: перенести это все в СИшный код
        self._hmd.poll()
        qx, qy, qz, qw = self._hmd.rotation[0:4]

        pitch = atan2(2 * (qw * qx + qy * qz), 1 - 2 * (qx ** 2 + qy ** 2)) * (180 / pi)
        yaw = asin(2 * (qw * qy - qz * qx)) * (180 / pi)
        roll = atan2(2 * (qw * qz + qx * qy), 1 - 2 * (qy ** 2 + qz ** 2)) * (180 / pi)

        return yaw, pitch, roll

    def getAngles(self):
        yaw, pitch, roll = self.getRawAngles()
        if self._start:
            return yaw, pitch, roll
        else:
            return 0, 0, 0

    def setZeroNow(self):
        """ установка начального значения по текущим значениям с очков """
        self._hmd.setZero()
        self._start = not self._start
        # self._zeroAngles = self.getRawAngles()

    def reset(self):
        self._hmd.reset()


if __name__ == "__main__":
    hmd = PyOpenHMD()
    while True:
        hmd.poll()
        time.sleep(0.2)
