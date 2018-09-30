#!/usr/bin/python
#coding:utf-8
#
# For STM32_sGPU Project to test Python perfomance
# Created: 11.08.2018
# Last edit: 17.08.2018
# 
# author: Antonov Alexandr (Bismuth208)
#
# Tested on:
#  Python 2.7.15 and PySerial 3.4
#  Python 3.6.4 and PySerial 3.2.1

import time
from random import randrange
from STMsGPU import *

gpu = sGPU()

TEST_SAMPLE_SIZE  = 200 # number of commands to send

# --------------------------------------------------------- #
def RND_565COLOR():
    MIN_COLOR = 32
    MAX_COLOR = 255
    COLOR_RANGE = (((MAX_COLOR + 1) - MIN_COLOR) + MIN_COLOR)
    return gpu.color565(
                randrange(0, COLOR_RANGE, 1), # r
                randrange(0, COLOR_RANGE, 1), # g
                randrange(0, COLOR_RANGE, 1)) # b

def main():
    # different speeds can be found in module STMsGPU.py
    # On Posix use '/dev/tty.X' or 'COMX' on Windows
    # BAUD_SPEED_1M = 1,000,000 bod/s
    # BAUD_SPEED_57600 = 57,600 bod/s
    # gpu.begin('/dev/tty.SLAB_USBtoUART ', BAUD_SPEED_1M)
    gpu.begin('/dev/tty.TP2_BT-DevB') # by default BAUD_SPEED_57600

    count = 0
    pyTimeExec = 0.0
    gpuTimeExec = 0.0
    avgPyTimeExec = 0.0
    avgGPUTimeExec = 0.0

    while 1:
        count += 1
        t = time.time()

        for i in range(TEST_SAMPLE_SIZE):
            gpu.fillScreen(RND_565COLOR())
        
        pyTimeExec = time.time()-t
        # thanks to this command we can mesure real execution time on sGPU
        gpu.pingCommand()
        gpuTimeExec = time.time()-t

        avgPyTimeExec += pyTimeExec
        avgGPUTimeExec += gpuTimeExec

        text = "No: %d; avgPy = %d; avgGPU = %d" % (
                count,
                (avgPyTimeExec/count)*1000,
                (avgGPUTimeExec/count)*1000)
        gpu.printAt(0, 0, text)
        print( text)

        time.sleep(1)

if __name__ == "__main__":
    main()
