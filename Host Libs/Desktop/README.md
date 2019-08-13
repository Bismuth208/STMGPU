# *STMsGPU.py*
***

This is a core module what represent all supported protocol for *sGPU* and 
allow you to control it on Desktop via serial port on any supported OS.

Also this module require to be installed [PySerial](https://github.com/pyserial/pyserial) (no lower v3.2.1).

#####There are few setup must be done in any test file before run:
  * set your serial port ('/dev/tty.x' or 'COMx')
  * set baudrate (9600, 57600, 115200 or 1M)

#####Tests:
  * graphicsTest.py
  * graphicsTest2.py
  * SDBitmap.py
  * speedTest.py
  * sprites.py
  * tileMap.py
  * tiles8x8.py
  * tiles16x16.py
  * tiles32x32.py

#####To run any test listed above type in terminal/cmd: 
```
python testName.py
```

#####In terminal/cmd window will appear messages like:
```
Sync...
Sync...
Sync...
Ok!
```
It's mean what program is trying to synchronize with *sGPU*.
After 'Ok!' message test code will start.
There is no need to press 'reset' on *sGPU* board, *sGPU* resets by itself on reconnection.

***
#### Tested on:
  * Python 2.7.15 and PySerial 3.4
  * Python 3.6.4 and PySerial 3.2.1
  * USB2UART cp2102
  * Bluetooth2UART HC-06
  * graphicsTest2.py
  * 57600 baudrate

***
###Results over cp2102 (USB2UART)
####*sGPU Pro:*
```
Benchmark               Time (milliseconds)
FillScreen              : 28
Text                    : 82
Lines                   : 725
Horiz/Vert Lines        : 176
Rectangles (outline)    : 77
Rectangles (filled)     : 154
Circles (filled)        : 301
Circles (outline)       : 346
Triangles (outline)     : 64
Triangles (filled)      : 116
Rounded rects (outline) : 92
Rounded rects (filled)  : 84
Rotation                : 334
Done!
```

***
###Results over Bluetooth
####*sGPU Pro:*
```
Benchmark               Time (milliseconds)
FillScreen              : 63
Text                    : 101
Lines                   : 854
Horiz/Vert Lines        : 213
Rectangles (outline)    : 101
Rectangles (filled)     : 187
Circles (filled)        : 326
Circles (outline)       : 373
Triangles (outline)     : 92
Triangles (filled)      : 148
Rounded rects (outline) : 151
Rounded rects (filled)  : 128
Rotation                : 504
Done!
```

#####*sGPU Mini:*
```
Benchmark               Time (milliseconds)
FillScreen              : 172
Text                    : 107
Lines                   : 1089
Horiz/Vert Lines        : 206
Rectangles (outline)    : 103
Rectangles (filled)     : 414
Circles (filled)        : 351
Circles (outline)       : 373
Triangles (outline)     : 98
Triangles (filled)      : 252
Rounded rects (outline) : 133
Rounded rects (filled)  : 458
Rotation                : 601
Done!
```

#### KNOWNING ISSUES:
```
- Sometimes serial port cannot be open:
    - just try again;

 - Sometimes *sGPU* not resets by itself on reconnection:
    - in this case press 'reset' on dev. board;

 - 'Sync...' message is always spaming:
    - check Tx and Rx wires (or connection);
    - check your serial port settings (baud and port);
    - try to reconnect or press 'reset' on *sGPU*;

 - After a few time on *sGPU* screen apper broken image:
    - ensure what Rx is still connected;
    - ensure what you NOT enable 'Harware busy' protection;
```

> ###### :exclamation: ATTENTION! :exclamation:
> ###### * This API is still unstable and and not ready for production! :beetle:
> ###### * Any changes in main protocol are possible at any time,
> ###### * and no backward capability is guaranteed!