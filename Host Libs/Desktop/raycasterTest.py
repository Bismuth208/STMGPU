#!/usr/bin/python
# coding:utf-8

# ***************************************************
# This is our GFX example for the Adafruit ILI9341 Breakout and Shield
# ----> http://www.adafruit.com/products/1651
#
# Check out the links above for our tutorials and wiring diagrams
# These displays use SPI to communicate, 4 or 5 pins are required to
# interface (RST is optional)
# Adafruit invests time and resources providing this open source code,
# please support Adafruit and open-source hardware by purchasing
# products from Adafruit!
#
# Written by Limor Fried/Ladyada for Adafruit Industries.
# MIT license, all text above must be included in any redistribution
# ****************************************************/

# For STM32_sGPU Project
# Created: 12.08.2017
# Last edit: 24.10.2019
#
# author: Antonov Alexandr (Bismuth208)
#
# Tested on:
#  Python 2.7.15 and PySerial 3.4
#  Python 3.6.4 and PySerial 3.2.1

import time
from collections import OrderedDict
from STMsGPU import *

gpu = SoftGPU()

# --------------------------------------------------------- #
# this is need to load textures from *.tle file
# located on SD card - correctly
MAX_TILES = 5  #
RAM_BASE = 0
TLE_START = 0
TILE_SET_W = 5  # this is width of tileSet in tiles ( one tile width == 8 pixels)


# --------------------------------------------------------- #
def main():
    # different speeds can be found in module STMsGPU.py
    # On Posix use '/dev/tty.X' or 'COMX' on Windows
    # BAUD_SPEED_1M = 1,000,000 bod/s
    # BAUD_SPEED_57600 = 57,600 bod/s
    gpu.begin('/dev/tty.SLAB_USBtoUART', gpu.BAUD_SPEED_57600)
    # gpu.begin('/dev/tty.TP2_BT-DevB', gpu.BAUD_SPEED_57600)

    gpu.loadTileSet16x16("w00L00t", TILE_SET_W, RAM_BASE, TLE_START, MAX_TILES)
    # gpu.loadTileMap("w00L00m"); # future feature

    # say to gpu: "render single frame"
    # force render, otherwise user will see something only after moving sticks
    gpu.fillScreen(gpu.COLOR_DARKGREY)  # make borders around render window
    gpu.renderFrame()


if __name__ == "__main__":
    main()
