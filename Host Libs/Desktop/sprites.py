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

import time
from random import *
from STMsGPU import *

gpu = SoftGPU()

TEST_SAMPLE_SIZE = 300

# this is need to load textures from *.tle file 
# located on SD card - correctly
MAX_TILES = 70
RAM_BASE = 0
TLE_START = 0
TILE_SET_W = 9  # width of tileSet in tiles ( one tile width == 8 pixels)

SPRITE_NUMBER = 0


# --------------------------------------------------------- #
# setup single sprite - number SPRITE_NUMBER
def gpuMakeSprite():
    # set tiles for sprite
    # always 4 numbers, even when sprite consist of 1 tile,
    # not used tiles may be set to any value (best is 0)
    gpu.setSpriteTiles(SPRITE_NUMBER, 0, 1, 0, 0)
    # set sprite type: 
    # SPR_2X1_8 mean: 2 sprites high, 1 width, 8x8 pix single tile
    gpu.setSpriteType(SPRITE_NUMBER, gpu.SPR_2X1_8)
    # if you do not set this to 1, then the sprite will not be drawn
    gpu.setSpriteVisible(SPRITE_NUMBER, 1)


# --------------------------------------------------------- //
# draw random tile at random position
def drawRandSprites():
    wight = gpu.width()
    height = gpu.height()

    for i in range(TEST_SAMPLE_SIZE):
        rnd_pos_x = randrange(0, wight, 1)
        rnd_pos_y = randrange(0, height, 1)

        # draw sprite SPRITE_NUMBER at random position
        gpu.drawSpriteAt(SPRITE_NUMBER, rnd_pos_x, rnd_pos_y)


# --------------------------------------------------------- #

def main():
    # different speeds can be found in module STMsGPU.py
    # On Posix use '/dev/tty.X' or 'COMX' on Windows
    # BAUD_SPEED_1M = 1,000,000 bod/s
    # BAUD_SPEED_57600 = 57,600 bod/s
    gpu.begin('/dev/tty.SLAB_USBtoUART', gpu.BAUD_SPEED_57600)
    # gpu.begin('/dev/tty.TP2_BT-DevB', gpu.BAUD_SPEED_57600)

    #  load MAX_TILES tiles to sGPU's RAM at RAM_BASE position in it's RAM,
    #  from tileFileName,
    #  located on SD card attached to STM32 sGPU
    #  TLE_START - number of tile in tileset from which tiles will be loaded
    #  file name must respond to 8.3 name system
    #  8 chars max for filename, 3 chars max for file extension
    #  sGPU add *.tle extension automatically
    gpu.loadTileSet8x8('pcs8x8', TILE_SET_W, RAM_BASE, TLE_START, MAX_TILES)

    gpuMakeSprite()

    while 1:
        drawRandSprites()

        time.sleep(.5)  # little delay to see what happened on screen
        gpu.fillScreen(gpu.COLOR_BLACK)  # clear screen by black color


if __name__ == '__main__':
    main()
