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