#!/usr/bin/env python3
# -*- coding:utf-8 vi:ts=4:noexpandtab
# Simple RTSP server. Run as-is or with a command-line to replace the default pipeline

"""
Документация на библиотеку ImageDraw https://pillow.readthedocs.io/en/stable/reference/ImageDraw.html
"""





import RPi.GPIO as GPIO
from PIL import Image       # библиотеки для рисования на дисплее
from PIL import ImageDraw
from PIL import ImageFont
import Adafruit_SSD1306



if __name__ == '__main__':
    disp = Adafruit_SSD1306.SSD1306_128_64(rst=None)
    disp.begin()  # запускаем дисплей
    disp.clear()  # очищаем буффер изображения
    width, height = disp.width, disp.height  # получаем высоту и ширину дисплея
    image = Image.new('1', (width, height))  # создаем изображение из библиотеки PIL для вывода на экран. 1 = картинка черно-белая, далее размер изображения
    draw = ImageDraw.Draw(image)  # создаем объект, которым будем рисовать
    draw.rectangle((0, 0, width, height), outline=0, fill=0)  # прямоугольник, залитый черным - очищаем дисплей

    font = ImageFont.load_default();

    # надпись "hello world"
    draw.text((0, 0), "Hello world", font=font, fill=255)  # формируем текст

    # рисование сердца. Рисуется линиями
    draw.line((30, 35, 50, 55), 255, 3)
    draw.line((50, 55, 70, 35), 255, 3)
    draw.line((30, 35, 40, 20), 255, 3)
    draw.line((70, 35, 60, 20), 255, 3)
    draw.line((60, 20, 50, 35), 255, 3)
    draw.line((40, 20, 50, 35), 255, 3)

    # вывод на дисплей
    disp.image(image)
    disp.display()

