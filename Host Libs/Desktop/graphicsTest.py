#!/usr/bin/python
# coding:utf-8
#
# For STM32_sGPU Project
# Created: 12.08.2018
# Last edit: 10.08.2019
# 
# author: Antonov Alexandr (Bismuth208)
#
# Tested on:
#  Python 2.7.15 and PySerial 3.4
#  Python 3.6.4 and PySerial 3.2.1

# import time
from random import *
from collections import OrderedDict
from STMsGPU import *

gpu = SoftGPU()

TEST_SAMPLE_SIZE = 2000
TEST_SAMPLE_SCREENS = 2

# --------------------------------------------------------- #

def drawRandPixels():
    for i in range(TEST_SAMPLE_SIZE):
        x, y = gpu.get_rnd_pos_x(1), gpu.get_rnd_pos_y(1)
        gpu.drawPixel(x, y, gpu.get_rnd_color_in_565(32, 255))


def drawRandLines():
    for i in range(TEST_SAMPLE_SIZE):
        x, y = gpu.get_rnd_pos_x(1), gpu.get_rnd_pos_y(1)
        x1, y1 = gpu.get_rnd_pos_x(1), gpu.get_rnd_pos_y(1)
        gpu.drawLine(x, y, x1, y1, gpu.get_rnd_color_in_565(32, 255))


def drawRandRect():
    for i in range(TEST_SAMPLE_SIZE):
        x, y = gpu.get_rnd_pos_x(4), gpu.get_rnd_pos_y(4)
        w, h = gpu.get_rnd_pos_x(x - 4), gpu.get_rnd_pos_y(y - 4)

        if w <= 1:
            w = 2
        if h <= 1:
            h = 2

        gpu.drawRect(x, y, w, h, gpu.get_rnd_color_in_565(32, 255))


def drawRandFillRect():
    for i in range(TEST_SAMPLE_SIZE):
        x, y = gpu.get_rnd_pos_x(4), gpu.get_rnd_pos_y(4)
        w, h = gpu.get_rnd_pos_x(x - 4), gpu.get_rnd_pos_y(y - 4)

        if w <= 1:
            w = 2
        if h <= 1:
            h = 2

        gpu.fillRect(x, y, w, h, gpu.get_rnd_color_in_565(32, 255))


def drawRandTriangles():
    for i in range(TEST_SAMPLE_SIZE):
        x1, y1 = gpu.get_rnd_pos_x(4), gpu.get_rnd_pos_y(4)
        x2, y2 = gpu.get_rnd_pos_x(4), gpu.get_rnd_pos_y(4)
        x3, y3 = gpu.get_rnd_pos_x(4), gpu.get_rnd_pos_y(4)
        gpu.fillTriangle(x1, y1, x2, y2, x3, y3, gpu.get_rnd_color_in_565(32, 255))


def drawRandRoundRect():
    for i in range(TEST_SAMPLE_SIZE):
        x, y = gpu.get_rnd_pos_x(1), gpu.get_rnd_pos_y(1)
        w, h = gpu.get_rnd_pos_x(x - 1), gpu.get_rnd_pos_y(y - 1)
        r = (randrange(0, 6, 1) + 4)

        if w <= 1:
            w = 2
        if h <= 1:
            h = 2

        gpu.drawRoundRect(x, y, w, h, r, gpu.get_rnd_color_in_565(32, 255))


def drawRandRoundFillRect():
    w, h = gpu.width(), gpu.height()

    for i in range(TEST_SAMPLE_SIZE):
        x, y = gpu.get_rnd_pos_x(1), gpu.get_rnd_pos_y(1)
        gpu.fillRoundRect(
            x, y,
            (w - x), (h - y),
            (randrange(0, 6, 1) + 4), gpu.get_rnd_color_in_565(32, 255))


def drawRandCircle():
    height = gpu.height()

    for i in range(TEST_SAMPLE_SIZE):
        x, y = gpu.get_rnd_pos_x(1), gpu.get_rnd_pos_y(1)
        r = int(randrange(0, int(height / 4), 1))
        gpu.drawCircle(x, y, r, gpu.get_rnd_color_in_565(32, 255))


def drawRandFillCircle():
    height = gpu.height()

    for i in range(TEST_SAMPLE_SIZE):
        x, y = gpu.get_rnd_pos_x(1), gpu.get_rnd_pos_y(1)
        r = int(randrange(0, int(height / 4), 1))
        gpu.fillCircle(x, y, r, gpu.get_rnd_color_in_565(32, 255))


def matrixScreen():
    col_x = int(gpu.width() / 5)
    rows_y = int(gpu.height() / 8)
    gpu.setTextSize(1)

    for i in range(TEST_SAMPLE_SCREENS * TEST_SAMPLE_SIZE):
        gpu.cp437(randrange(0, 1, 1))

        gpu.drawChar(
            randrange(0, col_x, 1) * 6, randrange(0, rows_y, 1) * 8,  # x , y
            randrange(0, 255, 1),  # get random char
            (((randrange(0, 192, 1) + 32) & 0xFC) << 3),  # char color
            gpu.COLOR_BLACK, 1)  # background color and char size


def drawText():
    gpu.setCursor(0, 0)
    gpu.setTextWrap(1)

    gpu.setTextSize(1)
    gpu.setTextColor(gpu.COLOR_WHITE)
    Loremipsum2 = '\
      Lorem ipsum dolor sit amet, consectetur adipiscing elit. \
      Curabitur adipiscing ante sed nibh tincidunt feugiat. \
      Maecenas enim massa, fringilla sed malesuada et,\
      malesuada sit amet turpis. \
      Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. \
      Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. \
      In vestibulum purus a tortor imperdiet posuere.\n\n'
    gpu.printg(Loremipsum2)

    gpu.setTextSize(2)
    gpu.setTextColor(gpu.COLOR_YELLOW)
    gpu.printg("Text size 2\n")

    gpu.setTextSize(3)
    gpu.setTextColor(gpu.COLOR_RED)
    gpu.printg("Even bigger 3\n")

    gpu.setTextSize(4)
    gpu.setTextColor(gpu.COLOR_GREEN)
    gpu.printg("Serious txt\n")


# --------------------------------------------------------- #
def main():
    test_functions = OrderedDict([
        ('drawRandPixels', drawRandPixels),
        ('drawRandLines', drawRandLines),
        ('drawRandRect', drawRandRect),
        ('drawRandFillRect', drawRandFillRect),
        ('drawRandTriangles', drawRandTriangles),
        # ('drawRandRoundRect', drawRandRoundRect),
        # ('drawRandRoundFillRect', drawRandRoundFillRect),
        ('drawRandCircle', drawRandCircle),
        ('drawRandFillCircle', drawRandFillCircle),
        ('matrixScreen', matrixScreen),
        ('drawText', drawText)
    ])

    # different speeds can be found in module STMsGPU.py
    # On Posix use '/dev/tty.X' or 'COMX' on Windows
    # BAUD_SPEED_1M = 1,000,000 bod/s
    # BAUD_SPEED_57600 = 57,600 bod/s
    gpu.begin('/dev/tty.SLAB_USBtoUART', gpu.BAUD_SPEED_57600)
    # gpu.begin('/dev/tty.TP2_BT-DevB', gpu.BAUD_SPEED_57600)

    while 1:
        for func in test_functions:
            gpu.fillScreen(gpu.COLOR_BLACK)
            test_functions[func]()
            # time.sleep(.5)


if __name__ == "__main__":
    main()
