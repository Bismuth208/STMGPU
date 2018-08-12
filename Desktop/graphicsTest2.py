#!/usr/bin/python
#coding:utf-8

#***************************************************
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
#****************************************************/

# For STM32_sGPU Project
# Created: 8.08.2018
# Last edit: 12.08.2018
# 
# author: Antonov Alexandr (Bismuth208)
#
# Tested on:
#  Python 2.7.15 and PySerial 3.4
#  Python 3.6.4 and PySerial 3.2.1

# import time
import random
from STMsGPU import *

gpu = sGPU()

# --------------------------------------------------------- #
def testFillScreen():
  gpu.fillScreen(COLOR_RED)
  gpu.fillScreen(COLOR_GREEN)
  gpu.fillScreen(COLOR_BLUE)
  gpu.fillScreen(COLOR_BLACK)

def testText():
  gpu.setCursor(0, 0)

  gpu.setTextColor(COLOR_WHITE)
  gpu.setTextSize(1)
  gpu.println("Hello World!")
  
  gpu.setTextColor(COLOR_YELLOW)
  gpu.setTextSize(2)
  gpu.println(1234.56)

  gpu.setTextColor(COLOR_RED)
  gpu.setTextSize(3)
  gpu.println(0xDEADBEEF, HEX)
  
  gpu.println()
  gpu.setTextColor(COLOR_GREEN)
  gpu.setTextSize(5)
  gpu.println("Groop")
  
  gpu.setTextSize(2)
  gpu.println("I implore thee,")
  
  gpu.setTextSize(1)
  gpu.println("my foonting turlingdromes.")
  gpu.println("And hooptiously drangle me")
  gpu.println("with crinkly bindlewurdles,")
  gpu.println("Or I will rend thee")
  gpu.println("in the gobberwarts")
  gpu.println("with my blurglecruncheon,")
  gpu.println("see if I don't!")

def testHLinesHelp(x0, y0, y1):
  for x1 in range(0, gpu.width(), 6):
    gpu.drawLine(x0, y0, x1, y1, COLOR_CYAN)

def testVLinesHelp(x0, y0, x1):
  for y1 in range(0, gpu.height(), 6):
    gpu.drawLine(x0, y0, x1, y1, COLOR_CYAN)

def testLines():
  w = gpu.width()
  h = gpu.height()

  testHLinesHelp(0, 0, h)
  testVLinesHelp(0, 0, w)

  gpu.fillScreen(COLOR_BLACK)
  testHLinesHelp(w, 0, h)
  testVLinesHelp(w, 0, 0)

  gpu.fillScreen(COLOR_BLACK)
  testHLinesHelp(0, h, 0)
  testVLinesHelp(0, h, w)

  gpu.fillScreen(COLOR_BLACK)
  testHLinesHelp(w, h, 0)
  testVLinesHelp(w, h, 0)

def testFastLines():
  w = gpu.width()
  h = gpu.height()

  for y in range(0, h, 5):
    gpu.drawFastHLine(0, y, w, COLOR_RED)

  for x in range(0, w, 5):
    gpu.drawFastVLine(x, 0, h, COLOR_BLUE)

def testRects():
  cx = int(gpu.width()  / 2)
  cy = int(gpu.height() / 2)
  n = min(gpu.width(), gpu.height())

  for i in range(2, n, 6):
    i2 = int(i / 2)
    gpu.drawRect(cx-i2, cy-i2, i, i, COLOR_GREEN)

def testFilledRects():
  cx = int(gpu.width()  / 2) #- 1
  cy = int(gpu.height() / 2) #- 1
  n = min(gpu.width(), gpu.height())

  for i in range(n, 0, -6):
    i2 = int(i / 2)
    gpu.fillRect(cx-i2, cy-i2, i, i, COLOR_YELLOW)
    gpu.drawRect(cx-i2, cy-i2, i, i, COLOR_MAGENTA)

def testCircles():
  radius = 10
  r2 = radius + radius
  w = gpu.width()  + radius
  h = gpu.height() + radius

  # Screen is not cleared for this one
  for x in range(0, w, r2):
    for y in range(0, h, r2):
      gpu.drawCircle(x, y, radius, COLOR_WHITE)

def testFilledCircles():
  w = gpu.width()
  h = gpu.height()
  radius = 10
  r2 = radius + radius

  for x in range(radius, w, r2):
    for y in range(radius, h, r2):
      gpu.fillCircle(x, y, radius, COLOR_MAGENTA)
  testCircles()

def testTriangles():
  cx = int(gpu.width()  / 2) - 1
  cy = int(gpu.height() / 2) - 1
  n = min(cx, cy)

  for i in range(0, n, 5):
    gpu.drawTriangle(
      cx    , cy - i, # peak
      cx - i, cy + i, # bottom left
      cx + i, cy + i, # bottom right
      gpu.color565(i, i, i))

def testFilledTriangles():
  cx = int(gpu.width()  / 2) #- 1
  cy = int(gpu.height() / 2) #- 1

  for i in range(min(cx,cy), 10, -5):
    gpu.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      gpu.color565(0, i*10, i*10))

    gpu.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      gpu.color565(i*10, i*10, 0))

def testRoundRects():
  cx = int(gpu.width()  / 2) #- 1
  cy = int(gpu.height() / 2) #- 1
  w = min(gpu.width(), gpu.height())

  for i in range(0, w, 6):
    i2 = int(i / 2)
    gpu.drawRoundRect(cx-i2, cy-i2, i, i, i/8, gpu.color565(i, 0, 0))

def testFilledRoundRects():
  cx = int(gpu.width()  / 2) #- 1
  cy = int(gpu.height() / 2) #- 1

  for i in range(min(gpu.width(), gpu.height()), 20, -6):
    i2 = int(i / 2)
    gpu.fillRoundRect(cx-i2, cy-i2, i, i, i/8, gpu.color565(0, i, 0))

def testRotation():
  for rotation in range(0, 4):
    gpu.setRotation(rotation)
    gpu.fillScreen(COLOR_BLACK)
    testText()
  gpu.setRotation(1)

# --------------------------------------------------------- #
def run():
  testFuctions = {
    'testFillScreen': testFillScreen,
    'testText': testText,
    'testLines': testLines,
    'testFastLines': testFastLines,
    'testFilledRects': testFilledRects,
    'testFilledCircles': testFilledCircles,
    'testTriangles': testTriangles,
    'testFilledTriangles': testFilledTriangles,
    'testRoundRects': testRoundRects,
    'testFilledRoundRects': testFilledRoundRects,
    'testRotation': testRotation
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
