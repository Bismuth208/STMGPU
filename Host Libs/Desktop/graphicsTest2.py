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
# Last edit: 17.08.2018
# 
# author: Antonov Alexandr (Bismuth208)
#
# Tested on:
#  Python 2.7.15 and PySerial 3.4
#  Python 3.6.4 and PySerial 3.2.1

import time
from collections import OrderedDict
from STMsGPU import *

gpu = sGPU()

# --------------------------------------------------------- #
def testFillScreen():
    t = time.time()

    for color in [COLOR_BLACK, COLOR_RED, 
            COLOR_GREEN, COLOR_BLUE, 
            COLOR_BLACK]:
        gpu.fillScreen(color)

    gpu.pingCommand()
    return time.time()-t

def testText():
    gpu.fillScreen(COLOR_BLACK)
    t = time.time()
    
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

    gpu.pingCommand()
    return time.time()-t

def testHLinesHelp(x0, y0, y1):
    for x1 in range(0, gpu.width(), 6):
        gpu.drawLine(x0, y0, x1, y1, COLOR_CYAN)

def testVLinesHelp(x0, y0, x1):
    for y1 in range(0, gpu.height(), 6):
        gpu.drawLine(x0, y0, x1, y1, COLOR_CYAN)

def testLines():
    gpu.fillScreen(COLOR_BLACK)
    
    w, h = gpu.width(), gpu.height()

    start = time.time()
    testHLinesHelp(0, 0, h)
    testVLinesHelp(0, 0, w)
    gpu.pingCommand()
    t = time.time() - start

    gpu.fillScreen(COLOR_BLACK)
    start = time.time()
    testHLinesHelp(w, 0, h)
    testVLinesHelp(w, 0, 0)
    gpu.pingCommand()
    t += time.time() - start

    gpu.fillScreen(COLOR_BLACK)
    start = time.time()
    testHLinesHelp(0, h, 0)
    testVLinesHelp(0, h, w)
    gpu.pingCommand()
    t += time.time() - start

    gpu.fillScreen(COLOR_BLACK)
    start = time.time()
    testHLinesHelp(w, h, 0)
    testVLinesHelp(w, h, 0)
    gpu.pingCommand()
    t += time.time() - start
    
    return t

def testFastLines():
    gpu.fillScreen(COLOR_BLACK)
    
    w, h = gpu.width(), gpu.height()

    t = time.time()
    for y in range(0, h, 5):
        gpu.drawFastHLine(0, y, w, COLOR_RED)

    for x in range(0, w, 5):
        gpu.drawFastVLine(x, 0, h, COLOR_BLUE)
    gpu.pingCommand()
    return time.time()-t

def testRects():
    gpu.fillScreen(COLOR_BLACK)
    
    cx = int(gpu.width()  / 2)
    cy = int(gpu.height() / 2)
    n = min(gpu.width(), gpu.height())

    t = time.time()
    for i in range(2, n, 6):
        i2 = int(i / 2)
        gpu.drawRect(cx-i2, cy-i2, i, i, COLOR_GREEN)
    gpu.pingCommand()
    return time.time()-t

def testFilledRects():
    gpu.fillScreen(COLOR_BLACK)
    
    cx = int(gpu.width()  / 2) #- 1
    cy = int(gpu.height() / 2) #- 1
    n = min(gpu.width(), gpu.height())

    t = time.time()
    for i in range(n, 0, -6):
        i2 = int(i / 2)
        gpu.fillRect(cx-i2, cy-i2, i, i, COLOR_YELLOW)
        gpu.drawRect(cx-i2, cy-i2, i, i, COLOR_MAGENTA)
    gpu.pingCommand()
    return time.time()-t

def testCircles():
    radius = 10
    r2 = radius + radius
    w = gpu.width()  + radius
    h = gpu.height() + radius

    # Screen is not cleared for this one
    t = time.time()
    for x in range(0, w, r2):
        for y in range(0, h, r2):
            gpu.drawCircle(x, y, radius, COLOR_WHITE)
    gpu.pingCommand()
    return time.time()-t

def testFilledCircles():
    gpu.fillScreen(COLOR_BLACK)
    
    w, h = gpu.width(), gpu.height()
    radius = 10
    r2 = radius + radius

    t = time.time()
    for x in range(radius, w, r2):
        for y in range(radius, h, r2):
            gpu.fillCircle(x, y, radius, COLOR_MAGENTA)
    gpu.pingCommand()
    return time.time()-t

def testTriangles():
    gpu.fillScreen(COLOR_BLACK)
    
    cx = int(gpu.width()  / 2) - 1
    cy = int(gpu.height() / 2) - 1
    n = min(cx, cy)

    t = time.time()
    for i in range(0, n, 5):
        gpu.drawTriangle(
          cx    , cy - i, # peak
          cx - i, cy + i, # bottom left
          cx + i, cy + i, # bottom right
          gpu.color565(i, i, i))
    gpu.pingCommand()
    return time.time()-t

def testFilledTriangles():
    gpu.fillScreen(COLOR_BLACK)

    cx = int(gpu.width()  / 2) #- 1
    cy = int(gpu.height() / 2) #- 1

    t = time.time()
    for i in range(min(cx,cy), 10, -5):
        gpu.fillTriangle(
                cx, cy - i,
                cx - i, cy + i,
                cx + i, cy + i,
                gpu.color565(0, i*10, i*10))
        gpu.drawTriangle(
                cx, cy - i,
                cx - i, cy + i,
                cx + i, cy + i,
                gpu.color565(i*10, i*10, 0))

    gpu.pingCommand()
    return time.time()-t

def testRoundRects():
    gpu.fillScreen(COLOR_BLACK)

    cx = int(gpu.width()  / 2) #- 1
    cy = int(gpu.height() / 2) #- 1
    w = min(gpu.width(), gpu.height())

    t = time.time()
    for i in range(0, w, 6):
        i2 = int(i / 2)
        gpu.drawRoundRect(
                cx-i2, cy-i2, # x, y
                i, i, # w, h
                i/8, # r
                gpu.color565(i, 0, 0))
    gpu.pingCommand()
    return time.time()-t

def testFilledRoundRects():
    gpu.fillScreen(COLOR_BLACK)

    cx = int(gpu.width()  / 2) #- 1
    cy = int(gpu.height() / 2) #- 1
    
    t = time.time()
    for i in range(min(gpu.width(), gpu.height()), 20, -6):
        i2 = int(i / 2)
        gpu.fillRoundRect(
                cx-i2, cy-i2, # x, y
                i, i, # w, h
                i/8, # r
                gpu.color565(0, i, 0))
    gpu.pingCommand()
    return time.time()-t

def testRotation():
    gpu.fillScreen(COLOR_BLACK)
    t = time.time()
    for rotation in range(4):
        gpu.setRotation(rotation)
        gpu.fillScreen(COLOR_BLACK)
        testText()
    gpu.setRotation(1)
    gpu.pingCommand()
    return time.time()-t

# --------------------------------------------------------- #
def main():
    testFuctions = OrderedDict([
        ('FillScreen              ', testFillScreen),
        ('Text                    ', testText),
        ('Lines                   ', testLines),
        ('Horiz/Vert Lines        ', testFastLines),
        ('Rectangles (outline)    ', testRects),
        ('Rectangles (filled)     ', testFilledRects),
        ('Circles (filled)        ', testFilledCircles),
        ('Circles (outline)       ', testCircles),
        ('Triangles (outline)     ', testTriangles),
        ('Triangles (filled)      ', testFilledTriangles),
        ('Rounded rects (outline) ', testRoundRects),
        ('Rounded rects (filled)  ', testFilledRoundRects),
        ('Rotation                ', testRotation)  # this one is optional
    ])

    # different speeds can be found in module STMsGPU.py
    # On Posix use '/dev/tty.X' or 'COMX' on Windows
    # BAUD_SPEED_1M = 1,000,000 bod/s
    # BAUD_SPEED_57600 = 57,600 bod/s
    # gpu.begin('/dev/tty.SLAB_USBtoUART ', BAUD_SPEED_1M)
    gpu.begin('/dev/tty.TP2_BT-DevB', BAUD_SPEED_57600)

    # count = 0
    # avgGPUTimeExec = 0.0

    while 1:
        # count += 1
        # gpuTimeExec = time.time()
        print('\nBenchmark               Time (milliseconds)')
        for func in testFuctions:
            print( "%s: %d" % (func, testFuctions[func]()*1000))
            # time.sleep(.5)
        print('Done!')
        # avgGPUTimeExec += (time.time()-gpuTimeExec)
        # text = "No: %d; avgGPU = %d" % (
        #         count, (avgGPUTimeExec/count)*1000000)
        # print( text)
        time.sleep(2)

if __name__ == "__main__":
    main()
