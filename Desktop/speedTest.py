#!/usr/bin/python
#coding:utf-8

# For STM32_sGPU Project to test Python perfomance
# Created: 11.08.2018
# Last edit: 12.08.2018
# 
# author: Antonov Alexandr (Bismuth208)
#
# Tested on:
#  Python 2.7.15 and PySerial 3.4
#  Python 3.6.4 and PySerial 3.2.1

import time
from random import *
from STMsGPU import *

gpu = sGPU()

TEST_SAMPLE_SIZE  = 200

# --------------------------------------------------------- #
def RND_565COLOR():
  MIN_COLOR = 32
  MAX_COLOR = 255
  COLOR_RANGE = (((MAX_COLOR + 1) - MIN_COLOR) + MIN_COLOR)
  return (((randrange(0, COLOR_RANGE, 1) & 0xF8) << 8) 
    | ((randrange(0, COLOR_RANGE, 1) & 0xFC) << 3) 
    | (randrange(0, COLOR_RANGE, 1) >> 3))

def run():
  # different speeds can be found in module STMsGPU.py
  # On Posix use '/dev/tty.X'
  # On Windows use 'COMX'
  # gpu.begin('/dev/tty.SLAB_USBtoUART ', BAUD_SPEED_1M) # BAUD_SPEED_1M = 1,000,000 bod/s
  gpu.begin('/dev/tty.TP2_BT-DevB', BAUD_SPEED_57600) # BAUD_SPEED_57600 = 57,600 bod/s

  count = 0
  totalTime = 0.0
  t1 = 0

  while 1:
    count = count + 1
    t = time.time()

    for i in range(TEST_SAMPLE_SIZE):
      gpu.fillScreen(RND_565COLOR())
    
    t1 = time.time()-t
    totalTime = totalTime + t1
    text = "No: %d; t = %.4f; avg = %.4f" % (count, t1, (totalTime/count))
    gpu.setCursor(0, 0)
    gpu.println(text)
    print( text)
    time.sleep(5)

if __name__ == "__main__":
  run()
