#!/usr/bin/python
#coding:utf-8

# For STM32_sGPU Project
# Created: 13.08.2018
# Last edit: 13.08.2018
# 
# author: Antonov Alexandr (Bismuth208)
#
# Tested on:
#  Python 2.7.15 and PySerial 3.4
#  Python 3.6.4 and PySerial 3.2.1

import time
from collections import OrderedDict
from random import randrange
from STMsGPU import *

gpu = sGPU()

TEST_SAMPLE_SIZE  = 12000
TEST_SAMPLE_SCREENS = 80

# this is need to load textures from *.tle file 
# located on SD card - correctly
MAX_TILES  = 70
RAM_BASE   = 0
TLE_START  = 0
TILE_SET_W = 10 # width of tileSet in tiles ( one tile width == 8 pixels)
TILE_SET_H = 7  # real number (1-8)

TILE_SHOW_W = 3
TILE_SHOW_H = 3

TLE_16X16_SIZE = 16 # this is single tile size, each tile 16x16 in px

# --------------------------------------------------------- //
# draw random tile at random position
def testDrawTiles():
    widght = gpu.width()
    height = gpu.height()

    for i in range(TEST_SAMPLE_SIZE):
        rndPosX = randrange(0, widght, 1)
        rndPosY = randrange(0, height, 1)
        
        # draw random tile from 0 to MAX_TILES, at random position
        gpu.drawTile16x16(rndPosX, rndPosY, randrange(0, MAX_TILES, 1))

# Draw on screen limited range of tiles on screen
def drawRamTileSet16x16():
    count =0
    
    # draw 10 tiles
    for countY in range(TILE_SHOW_W):
        for countX in range(TILE_SHOW_H):
            # 50 is default position in px on screen
            posX = (50 + ( countX * TLE_16X16_SIZE ))
            posY = (50 + ( countY * TLE_16X16_SIZE ))
            
            gpu.drawTile16x16(posX, posY, count)
            count += 1

# Fill whole screen by random tiles
def fillScreenByTiles():
    maxXSize = int(gpu.width() / TLE_16X16_SIZE)  # \__ calculate how much
    maxYSize = int(gpu.height() / TLE_16X16_SIZE) # /   tiles in x and y axis

    for i in range(TEST_SAMPLE_SCREENS):
        for yStep in range(maxYSize):
            for xStep in range(maxXSize):
                # draw random tile form 0 to MAX_TILES
                gpu.drawTile16x16(
                        xStep*TLE_16X16_SIZE, yStep*TLE_16X16_SIZE,
                        randrange(0, MAX_TILES, 1))

# --------------------------------------------------------- #
def main():
    testFuctions = OrderedDict([
        ('Draw Tiles': testDrawTiles),
        ('Draw RAM Tileset': drawRamTileSet16x16),
        ('Fill Screen by Tiles': fillScreenByTiles)
    ])

    # different speeds can be found in module STMsGPU.py
    # On Posix use '/dev/tty.X' or 'COMX' on Windows
    # BAUD_SPEED_1M = 1,000,000 bod/s
    # BAUD_SPEED_57600 = 57,600 bod/s
    # gpu.begin('/dev/tty.SLAB_USBtoUART ', BAUD_SPEED_1M)
    gpu.begin('/dev/tty.TP2_BT-DevB', BAUD_SPEED_57600)

    #  load MAX_TILES tiles to sGPU's RAM at RAM_BASE position in it's RAM,
    #  from tileFileName,
    #  located on SD card attached to STM32 sGPU
    #  TLE_START - number of tile in tileset from which tiles will be loaded
    #  file name must respond to 8.3 name system
    #  8 chars max for filename, 3 chars max for file extension
    #  sGPU add *.tle extension automatically
    gpu.loadTileSet8x8('pcs16x16', TILE_SET_W-1, RAM_BASE, TLE_START, MAX_TILES)

    while 1:
        for func in testFuctions:
            testFuctions[func]()

            time.sleep(.5)  # little delay to see what happend on screen
            gpu.fillScreen(COLOR_BLACK) # clear screen by black color

if __name__ == '__main__':
    main()
