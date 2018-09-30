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

from STMsGPU import *

# --------------------------------------------------------- #
def main():
    gpu = sGPU()

    # different speeds can be found in module STMsGPU.py
    # On Posix use '/dev/tty.X' or 'COMX' on Windows
    # BAUD_SPEED_1M = 1,000,000 bod/s
    # BAUD_SPEED_57600 = 57,600 bod/s
    # gpu.begin('/dev/tty.SLAB_USBtoUART ', BAUD_SPEED_1M)
    gpu.begin('/dev/tty.TP2_BT-DevB', BAUD_SPEED_57600)

    # sGPU add *.bmp extension automatically
    # supported only BMP16 and BMP24 (whith no alpha channel)!
    gpu.printBMP(0, 0, 'tree1') # draw bmp24 picture at position
    gpu.printBMP(bmpFileName = 'tree2')    # \__ draw bmp24 and bmp16 pics
    gpu.printBMP(bmpFileName = 'tree36')   # /   at 0,0 position

if __name__ == "__main__":
    main()
