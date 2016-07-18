# STMGPU
2D GPU for Arduino

Today a lot of code is written for AVR (Arduino), and we all know limitations of these MCUs, for example extremely slow drawing on LCD screens, not enough ROM, huge SD libs and other!
But what if You want to use these MCUs longer, and You absolutely doesn’t want to waste time for porting sources, looking for hundreds of mans and investigate STM32?

That is why this GPU coprocessor project was created!
Created for simple MCU_CPU (like AVR) and help to accelerate 2d graphics on LCDs by using powerful MCU like ARM STM32.

For maximum simplicity USART was selected as main communication interface. There are tons of realizations of this interface (as hardware as software) for all type of MCUs.

Flexible Baud rate selection allows matching from low speed to hi speed transfer (some MCUs has no resonators and required low speed).
Only two lines is used to communicate (even one line possible).

GPU coprocessor can save ROM space, RAM, free MCU_CPU from executing code for LCD and left more resources for main application!

Just send commands to USART like it's an LCD and GPU do it's job.

MCU_CPU lib is written on C, so it can be simply ported to where You want!

All this allow to use MCU_CPU even like AVR ATtiny family, isn’t cool huh?

// --------------------------------------------------------------------------------------------------------------------- //

Code for STM32 is writed in IAR 7.40;

Used MCU for GPU: STM32F103C8T6 ( Mapple mini);

Used MCU for CPU: Atmel AVR ATmega328p (Arduino Pro mini), but it can be defferent;

GPIO pinout can be find in "readme_STM32_GPU.docx".
