#!/usr/bin/python
# coding:utf-8
#
# For STM32_sGPU Project to test Python performance
# Created: 11.08.2018
# Last edit: 10.08.2019
# 
# author: Antonov Alexandr (Bismuth208)
#
# Tested on:
#  Python 2.7.15 and PySerial 3.4
#  Python 3.6.4 and PySerial 3.2.1

import time
from STMsGPU import *

gpu = SoftGPU()

TEST_SAMPLE_SIZE = 200  # number of commands to send


# --------------------------------------------------------- #
def main():
    # different speeds can be found in module STMsGPU.py
    # On Posix use '/dev/tty.X' or 'COMX' on Windows
    # BAUD_SPEED_1M = 1,000,000 bod/s
    # BAUD_SPEED_57600 = 57,600 bod/s
    gpu.begin('/dev/tty.SLAB_USBtoUART', gpu.BAUD_SPEED_57600)
    # gpu.begin('/dev/tty.TP2_BT-DevB')  # by default BAUD_SPEED_57600

    count = 0
    py_time_exec = 0.0
    gpu_time_exec = 0.0
    avg_py_time_exec = 0.0
    avg_gpu_time_exec = 0.0

    while 1:
        count += 1
        t = time.time()

        for i in range(TEST_SAMPLE_SIZE):
            gpu.fillScreen(gpu.get_rnd_color_in_565(32, 255))

        py_time_exec = time.time() - t
        # thanks to this command we can measure real execution time on sGPU
        gpu.pingCommand()
        gpu_time_exec = time.time() - t

        avg_py_time_exec += py_time_exec
        avg_gpu_time_exec += gpu_time_exec

        text = "No: %d; avgPy = %d; avgGPU = %d" % (
            count,
            (avg_py_time_exec / count) * 1000,
            (avg_gpu_time_exec / count) * 1000)
        gpu.printAt(0, 0, text)
        print(text)

        time.sleep(1)


if __name__ == "__main__":
    main()
