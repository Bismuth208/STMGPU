#!/usr/bin/python
#coding:utf-8

# For STM32_sGPU Project
# Created: 12.08.2018
# Last edit: 12.08.2018
# 
# author: Antonov Alexandr (Bismuth208)
#
# Tested on:
#  Python 2.7.15 and PySerial 3.4
#  Python 3.6.4 and PySerial 3.2.1

# import time
from random import *
from STMsGPU import *

gpu = sGPU()

TEST_SAMPLE_SIZE  = 2000
TEST_SAMPLE_SCREENS = 2

# --------------------------------------------------------- #
def RND_POSX(offset):
  return randint(0, gpu.width()-offset)

def RND_POSY(offset):
  return randint(0, gpu.height()-offset)

def RND_565COLOR():
  MIN_COLOR = 32
  MAX_COLOR = 255
  COLOR_RANGE = (((MAX_COLOR + 1) - MIN_COLOR) + MIN_COLOR)
  RND_COLOR = randint(0, COLOR_RANGE)
  return (((RND_COLOR & 0xF8) << 8) | ((RND_COLOR & 0xFC) << 3) | (RND_COLOR >> 3))

# --------------------------------------------------------- #

def drawRandPixels():
  for i in range(TEST_SAMPLE_SIZE):
    posX = RND_POSX(1)
    posY = RND_POSY(1)
    gpu.drawPixel(posX, posY, RND_565COLOR())

def drawRandLines():
  for i in range(TEST_SAMPLE_SIZE):
    posX = RND_POSX(1)
    posY = RND_POSY(1)
    posX1 = RND_POSX(1)
    posY1 = RND_POSY(1)
    gpu.drawLine(posX, posY, posX1, posY1, RND_565COLOR())

def drawRandRect():
  for i in range(TEST_SAMPLE_SIZE):
    posX = RND_POSX(4)
    posY = RND_POSY(4)
    widght = RND_POSX(posX-4)
    height = RND_POSY(posY-4)

    if widght <= 1:
      widght = 2
    if height <= 1:
      height = 2

    gpu.drawRect(posX, posY, widght, height, RND_565COLOR())

def drawRandFillRect():
  for i in range(TEST_SAMPLE_SIZE):
    posX = RND_POSX(4)
    posY = RND_POSY(4)
    widght = RND_POSX(posX-4)
    height = RND_POSY(posY-4)

    if widght <= 1:
      widght = 2
    if height <= 1:
      height = 2

    gpu.fillRect(posX, posY, widght, height, RND_565COLOR())

def drawRandTriangles():
  for i in range(TEST_SAMPLE_SIZE):
    x1 = RND_POSX(4)
    y1 = RND_POSY(4)
    x2 = RND_POSX(4)
    y2 = RND_POSY(4)
    x3 = RND_POSX(4)
    y3 = RND_POSY(4)
    gpu.fillTriangle(x1, y1, x2, y2, x3, y3, RND_565COLOR())

def drawRandRoundRect():
  for i in range(TEST_SAMPLE_SIZE):
    posX = RND_POSX(1)
    posY = RND_POSY(1)
    widght = RND_POSX(posX-1)
    height = RND_POSY(posY-1)
    r = (randint(0, 6)+4); # for raduis

    if widght <= 1:
      widght = 2
    if height <= 1:
      height = 2

    gpu.drawRoundRect(posX, posY, widght, height, r, RND_565COLOR())

def drawRandRoundFillRect():
  for i in range(TEST_SAMPLE_SIZE):
    posX = RND_POSX(1)
    posY = RND_POSY(1)
    gpu.fillRoundRect(posX, posY, (gpu.width() - posX), (gpu.height() - posY), (randint(0, 6)+4), RND_565COLOR())

def drawRandCircle():
  for i in range(TEST_SAMPLE_SIZE):
    posX = RND_POSX(1)
    posY = RND_POSY(1)
    r = int(randint(0, gpu.height())/4) # for radius
    gpu.drawCircle(posX, posY, r, RND_565COLOR())

def drawRandFillCircle():
  for i in range(TEST_SAMPLE_SIZE):
    posX = RND_POSX(1)
    posY = RND_POSY(1)
    r = int(randint(0, gpu.height())/4) # for radius
    gpu.fillCircle(posX, posY, r, RND_565COLOR())

def matrixScreen():
  colX = int(gpu.width() / 5)
  rowsY = int(gpu.height() / 8)
  gpu.setTextSize(1)
  
  for iScr in range(TEST_SAMPLE_SCREENS):
    for i in range(TEST_SAMPLE_SIZE):
      gpu.cp437(randint(0, 1))

      gpu.drawChar(randint(0, colX) * 6, randint(0, rowsY) * 8,  # pos X and Y
                   randint(0, 255),                                # number of char
                   (((randint(0, 192) + 32) & 0xFC) << 3),         # text color
                   COLOR_BLACK, 1)                                   # BG color and size

def drawText():
  gpu.setCursor(0, 0)
  gpu.setTextWrap(1)

  gpu.setTextSize(1)
  gpu.setTextColor(COLOR_WHITE)
  Loremipsum2 = "\
    Lorem ipsum dolor sit amet, consectetur adipiscing elit. \
    Curabitur adipiscing ante sed nibh tincidunt feugiat. \
    Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. \
    Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. \
    Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. \
    In vestibulum purus a tortor imperdiet posuere.\n\n"
  gpu.printg(Loremipsum2)

  gpu.setTextSize(2)
  gpu.setTextColor(COLOR_YELLOW)
  gpu.printg("Text size 2\n")

  gpu.setTextSize(3);
  gpu.setTextColor(COLOR_RED)
  gpu.printg("Even bigger 3\n")

  gpu.setTextSize(4);
  gpu.setTextColor(COLOR_GREEN)
  gpu.printg("Seruious txt\n")

# --------------------------------------------------------- #
def run():
  testFuctions = {
    'drawRandPixels': drawRandPixels,
    'drawRandLines': drawRandLines,
    'drawRandRect': drawRandRect,
    'drawRandFillRect': drawRandFillRect,
    'drawRandTriangles': drawRandTriangles,
    # 'drawRandRoundRect': drawRandRoundRect,
    # 'drawRandRoundFillRect': drawRandRoundFillRect,
    'drawRandCircle': drawRandCircle,
    'drawRandFillCircle': drawRandFillCircle,
    'matrixScreen': matrixScreen,
    'drawText': drawText
}

  # different speeds can be found in module STMsGPU.py
  # On Posix use '/dev/tty.X'
  # On Windows use 'COMX'
  # gpu.begin('/dev/tty.SLAB_USBtoUART ', BAUD_SPEED_1M) # BAUD_SPEED_1M = 1,000,000 bod/s
  gpu.begin('/dev/tty.TP2_BT-DevB', BAUD_SPEED_57600) # BAUD_SPEED_57600 = 57,600 bod/s

  while 1:
    for func in testFuctions:
      gpu.fillScreen(COLOR_BLACK)
      testFuctions[func]()
      # time.sleep(.5)

if __name__ == "__main__":
  run()
