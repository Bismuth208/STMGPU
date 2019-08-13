#!/usr/bin/python
# coding:utf-8

# For STM32_sGPU Project
# Created: 10.04.2020
# Last edit: 10.04.2020
# 
# author: Antonov Alexandr (Bismuth208)
#
# Tested on:
#  Python 2.7.15 and PySerial 3.4
#  Python 3.6.4 and PySerial 3.2.1

import time
from STMsGPU import *

gpu = SoftGPU()


# --------------------------------------------------------- #


def drawRandPixels():
    x, y = gpu.get_rnd_pos_x(1), gpu.get_rnd_pos_y(1)
    gpu.drawPixel(x, y, gpu.get_rnd_color_in_565(32, 255))


# --------------------------------------------------------- #

def main():
    # different speeds can be found in module STMsGPU.py
    # On Posix use '/dev/tty.X' or 'COMX' on Windows
    # BAUD_SPEED_1M = 1,000,000 bod/s
    # BAUD_SPEED_57600 = 57,600 bod/s
    gpu.begin('/dev/tty.SLAB_USBtoUART', gpu.BAUD_SPEED_57600)
    # gpu.begin('/dev/tty.TP2_BT-DevB', gpu.BAUD_SPEED_57600)
    # gpu.setProtocolVer(SGPU_PROTOCOL_VER_V1)

    while 1:
        # drawRandPixels()
        gpu.fillScreen(gpu.get_rnd_color_in_565())
        # time.sleep(.05)


if __name__ == "__main__":
    main()
