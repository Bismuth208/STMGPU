# STMGPU
*2D software GPU for Arduino (or PC)*
***

Today a lot of code is written for *AVR(~~Arduino~~)*, and we all know limitations of these *MCUs*,  
for example extremely slow drawing on *LCD* screens, not enough *ROM*, huge *SD* libs and other!

But what if You want to use these *MCUs* longer, and you absolutely doesn't want to waste time for porting sources,  
looking for hundreds of manuals and investigate *STM32*?

That is why this *sGPU coprocessor* project was created!  
Created for simple *MCU_CPU (like AVR)* and help to accelerate 2d graphics on *LCDs*  
by using powerful *MCU* like *ARM STM32*.

For maximum simplicity *serial port (UART)* was selected as main communication interface.  
There are lot of realizations of this interface (as hardware as software) for all type of *MCUs*.

Flexible Baud rate selection allows to match from lowest to highest transfer speed (some *MCUs* has no resonators and required low speed).  
Only two lines is used to communicate (even one line possible).

*sGPU coprocessor* can save *ROM* space, *RAM*, release *MCU_CPU* from executing code for *LCD* and left more  
resources for main application!

Just send commands to *UART* like it's an *LCD* and *sGPU* do it's job.

***MCU_CPU* libs was written on C and C++, so it can be simply ported to where you want!**

All this allow to use *MCU_CPU* even like *AVR ATtiny* family, isn't cool huh?

[To see how does it work check this link](https://www.youtube.com/channel/UCDXVQ9ZfQl8Ddeu_3qiwSiA "My YouTube channel")
***

* Code for *STM32* was writed in *IAR 7.40* with *SPL*

* Used *MCU* for *sGPU*:
  * STM32F103C8T6 (Mapple mini) *as mini version*
  * STM32F103VET6 (ministm32) *as pro version*
  
* As *CPU* can be used:
  * Any capable board with *Arduino IDE*: Uno, Mega, nano, STM32, e.t.c.
  * *PC* with *USBtoCOM* dongle
  * Anything what have *Rx*, *Tx* and GND lines
  * All above must be 3v-5v tolerant
  * Support as software as hardware serial port (UART)
  
* There are two *MCU* libs for *Arduino IDE*:
  * **C version**(*C_AVR_Libs*), mostly optimized for *ATmega328p* or similar
  * **C++ version**(*Arduino_lib*), support most of know *MCUs*, even STM32!
  
* *GPIO* pinout for *sGPU* can be find in:
  * [STM32_GPU_GPIO_Pinout.txt](/STM32_Project/STM32_GPU_GPIO_Pinout.txt)
  * [STM32_GPU_GPIO_Pinout.png](/STM32_Project/STM32F103C8T6_pinout.PNG)

> ### ATTENTION!
>  * This project is still unstable and in develop!
>  * Any changes in main protocol are possible at any time,
>  * and no backward capability is guaranteed!
  