# STMGPU
2D sofware GPU for Arduino

Today a lot of code is written for AVR (Arduino), and we all know limitations of these MCUs, for example extremely slow drawing on LCD screens, not enough ROM, huge SD libs and other!
But what if You want to use these MCUs longer, and You absolutely doesn’t want to waste time for porting sources, looking for hundreds of manuals and investigate STM32?

That is why this sGPU coprocessor project was created!
Created for simple MCU_CPU (like AVR) and help to accelerate 2d graphics on LCDs by using powerful MCU like ARM STM32.

For maximum simplicity USART was selected as main communication interface. There are lot of realizations of this interface (as hardware as software) for all type of MCUs.

Flexible Baud rate selection allows to match from lowest speed to highest speed transfer (some MCUs has no resonators and required low speed).
Only two lines is used to communicate (even one line possible).

sGPU coprocessor can save ROM space, RAM, release MCU_CPU from executing code for LCD and left more resources for main application!

Just send commands to USART like it's an LCD and sGPU do it's job.

MCU_CPU libs was written on C and C++, so it can be simply ported to where You want!

All this allow to use MCU_CPU even like AVR ATtiny family, isn’t cool huh?

// --------------------------------------------------------------------------------------------------------------------- //

Code for STM32 was writed in IAR 7.40;

Used MCU for sGPU: STM32F103C8T6 ( Mapple mini);

Used MCU for CPU: Atmel AVR ATmega328p (Arduino Pro mini), but it can be defferent;

There are two MCU libs for Arduino IDE:
 - C_AVR_Libs:  C version, mostly optimized for ATmega328p or similar;
 - Arduino_lib: C++ version, support most of know MCUs.

GPIO pinout for sGPU can be find in:
"STM32_Project/STM32_GPU_GPIO_Pinout.txt"
or
"STM32_Project/STM32F103C8T6_pinout.png".
