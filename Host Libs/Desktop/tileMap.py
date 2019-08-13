#!/usr/bin/python
# coding:utf-8

# For STM32_sGPU Project
# Created: 13.08.2018
# Last edit: 10.08.2019
# 
# author: Antonov Alexandr (Bismuth208)
#
# Tested on:
#  Python 2.7.15 and PySerial 3.4
#  Python 3.6.4 and PySerial 3.2.1

from STMsGPU import *

# this is need to load textures from *.tle file 
# located on SD card - correctly
MAX_TILES = 36
RAM_BASE = 0
TLE_START = 0
TILE_SET_W = 7  # width of tileSet in tiles ( one tile width == 8 pixels)


# --------------------------------------------------------- #
def main():
    gpu = SoftGPU()

    # different speeds can be found in module STMsGPU.py
    # On Posix use '/dev/tty.X' or 'COMX' on Windows
    # BAUD_SPEED_1M = 1,000,000 bod/s
    # BAUD_SPEED_57600 = 57,600 bod/s
    # gpu.begin('/dev/tty.SLAB_USBtoUART ', gpu.BAUD_SPEED_1M)
    gpu.begin('/dev/tty.TP2_BT-DevB', gpu.BAUD_SPEED_57600)

    #  load MAX_TILES tiles to sGPU's RAM at RAM_BASE position in it's RAM,
    #  from tileFileName,
    #  located on SD card attached to STM32 sGPU
    #  TLE_START - number of tile in tileset from which tiles will be loaded
    #  file name must respond to 8.3 name system
    #  8 chars max for filename, 3 chars max for file extension
    #  sGPU add *.tle extension automatically
    gpu.loadTileSet8x8("mario", TILE_SET_W, RAM_BASE, TLE_START, MAX_TILES)

    #  load tiled background to to GPU's RAM
    #  file must be located on SD card attached to STM32 GPU!
    #  sGPU add *.map extension automatically
    #
    gpu.loadTileMap("mario")

    # draw previously loaded tile map
    gpu.drawTileMap()


if __name__ == "__main__":
    main()
