/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/


#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

#include <stm32f10x.h>
#include <stm32f10x_dma.h>
#endif

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <systicktimer.h>
#ifdef __AVR__
#include <avrspi.h>
#else
#include <spi.h>
#endif

#include "ili9341.h"

#if USE_DMA
#include <gfxDMA.h>
#endif

//-------------------------------------------------------------------------------------------//

int16_t _width  = ILI9341_TFTWIDTH;
int16_t _height = ILI9341_TFTHEIGHT;

//-------------------------------------------------------------------------------------------//

void writeCommand(uint8_t c)
{
#if USE_FSMC
  FSMC_SEND_CMD(c);
#else
  ENABLE_CMD();
  
  sendData8_SPI1(c);
  RELEASE_TFT();
#endif
}

void writeData(uint8_t c)
{
#if USE_FSMC
  FSMC_SEND_DATA(c);
#else
  ENABLE_DATA();
  
  sendData8_SPI1(c);
  RELEASE_TFT();
#endif
}

void writeWordData(uint16_t c)
{
#if USE_FSMC
  FSMC_SEND_DATA(c);
#else
  ENABLE_DATA();
  
  sendData16_SPI1(c);
  RELEASE_TFT();
#endif
}

void commandList(const uint8_t *addr)
{
  uint8_t count;
  while (1) {
    count = pgm_read_byte(addr++);
    if (count-- == 0) break;
    writeCommand(pgm_read_byte(addr++));
    while (count--) {
      writeData(pgm_read_byte(addr++));
    }
  }
}

#if USE_FSMC
void initTFT_FSMC_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
   // Init GPIO for FSMC
  GPIO_InitStruct.GPIO_Pin = FSMC_PIN_D2 | FSMC_PIN_D3 | FSMC_PIN_RD | FSMC_PIN_WR | FSMC_PIN_CS | FSMC_PIN_D13 | FSMC_PIN_D14 | FSMC_PIN_D15 | FSMC_PIN_RS | FSMC_PIN_D0 | FSMC_PIN_D1;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  
  GPIO_InitStruct.GPIO_Pin = FSMC_PIN_D4 | FSMC_PIN_D5 | FSMC_PIN_D6 | FSMC_PIN_D7 | FSMC_PIN_D8 | FSMC_PIN_D9 | FSMC_PIN_D10 | FSMC_PIN_D11 | FSMC_PIN_D12;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStruct);
  
  // Reset
  GPIO_InitStruct.GPIO_Pin = TFT_RES_PIN;
  GPIO_Init(GPIOE, &GPIO_InitStruct);
  
  
  /////////////////////////////////
  // CS -> 1
  // Reset -> 0
  // RD -> 1
  // WR -> 1
  
  GPIO_SET_PIN(GPIOD, FSMC_PIN_CS);
  GPIO_SET_PIN(GPIOE, TFT_RES_PIN);
  GPIO_SET_PIN(GPIOD, FSMC_PIN_RD);
  GPIO_SET_PIN(GPIOD, FSMC_PIN_WR);

  //GPIO_SetBits(GPIOD, FSMC_PIN_CS);
  //GPIO_ResetBits(GPIOE, TFT_RES_PIN);
  //GPIO_SetBits(GPIOD, FSMC_PIN_RD);
  //GPIO_SetBits(GPIOD, FSMC_PIN_WR);
}

void initTFT_FSMC(void)
{
  FSMC_NORSRAMInitTypeDef fsmc;
  FSMC_NORSRAMTimingInitTypeDef fsmcTiming;

  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
  
  initTFT_FSMC_GPIO();
  
  // setup FSMC
  fsmcTiming.FSMC_AddressSetupTime = 0x02;
  fsmcTiming.FSMC_AddressHoldTime = 0x00;
  fsmcTiming.FSMC_DataSetupTime = 0x05;
  fsmcTiming.FSMC_BusTurnAroundDuration = 0x00;
  fsmcTiming.FSMC_CLKDivision = 0x00;
  fsmcTiming.FSMC_DataLatency = 0x00;
  fsmcTiming.FSMC_AccessMode = FSMC_AccessMode_B;


  fsmc.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  fsmc.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  fsmc.FSMC_MemoryType = FSMC_MemoryType_SRAM; //FSMC_MemoryType_NOR;
  fsmc.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  fsmc.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  fsmc.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  fsmc.FSMC_WrapMode = FSMC_WrapMode_Disable;
  fsmc.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  fsmc.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  fsmc.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  fsmc.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  fsmc.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  fsmc.FSMC_ReadWriteTimingStruct = &fsmcTiming;
  fsmc.FSMC_WriteTimingStruct = &fsmcTiming;
  
  FSMC_NORSRAMInit(&fsmc);
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}
#endif

void initTFT_GPIO()
{
#if defined (__AVR__)
  SET_BIT(TFT_DDRX, TFT_RES_PIN);
  SET_TFT_RES_LOW;
  
  SET_BIT(TFT_DDRX, TFT_DC_PIN);
#else
  

#if USE_FSMC
  initTFT_FSMC();
#else
  GPIO_InitTypeDef GPIO_InitStruct;
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;     // Mode: output "Push-Pull"
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;     // Set speed
  GPIO_InitStruct.GPIO_Pin = TFT_DC_PIN | TFT_RES_PIN | TFT_SS_PIN;
  GPIO_Init(GPIOB, &GPIO_InitStruct);        // Aply settings to port B
#endif // USE_FSMC
#endif // __AVR__
}

void tftBegin(void)
{
#if defined (__AVR__)
  initTFT_GPIO();
#else
  initTFT_GPIO();
  SET_TFT_RES_LOW;
#endif
  
#if TFT_CS_ALWAS_ACTIVE
  GRAB_TFT_CS;
#endif
  
  SET_TFT_CS_HI;
  SET_TFT_DC_HI;
        
  // toggle RST low to reset
  SET_TFT_RES_HI;
  _delayMS(5);
  SET_TFT_RES_LOW;
  _delayMS(20);
  SET_TFT_RES_HI;
  _delayMS(150);
  
  commandList(init_commands);
  
  writeCommand(ILI9341_SLPOUT);    //Exit Sleep
  _delayMS(120);
  writeCommand(ILI9341_DISPON);    //Display on

#if USE_DMA
  init_DMA1_SPI1();
#endif
}

// blow your mind
void tftSetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{  
#if USE_DMA  
  //while(DMA1_SPI1_busy());
  wait_DMA1_SPI1_busy();
#endif
  
#if USE_FSMC
  FSMC_SEND_CMD(ILI9341_CASET);    // Column addr set
  FSMC_SEND_DATA(x0);           // XSTART
  FSMC_SEND_DATA(x1);           // XEND
  
  FSMC_SEND_CMD(ILI9341_RASET);    // Row addr set
  FSMC_SEND_DATA(y0);           // YSTART
  FSMC_SEND_DATA(y1);           // YEND
  
  FSMC_SEND_CMD(ILI9341_RAMWR);     // write to RAM
#else
  
  ENABLE_CMD();             // grab TFT CS and writecommand:
  sendData8_SPI1(ILI9341_CASET); // Column addr set
  
  SET_TFT_DC_HI;          // writeData:
#ifdef __AVR__
  sendData16_SPI1(x0);    // XSTART
  sendData16_SPI1(x1);    // XEND
#else
  sendData32_SPI1(x0, x1);
#endif  // __AVR__
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RASET); // Row addr set
  
  SET_TFT_DC_HI;           // writeData:
#ifdef __AVR__
  sendData16_SPI1(y0);     // YSTART
  sendData16_SPI1(y1);     // YEND
#else
  sendData32_SPI1(y0, y1);
#endif // __AVR__
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RAMWR); // write to RAM
  
  SET_TFT_DC_HI;  // ready accept data
  
  //SET_TFT_CS_HI; // disable in other func
#endif // USE_FSMC  
}

void tftSetVAddrWindow(uint16_t x0, uint16_t y0, uint16_t y1)
{
#if USE_DMA  
  //while(DMA1_SPI1_busy());
  wait_DMA1_SPI1_busy();
#endif
  
#if USE_FSMC
  FSMC_SEND_CMD(ILI9341_CASET);    // Column addr set
  FSMC_SEND_DATA(x0);           // XSTART
  FSMC_SEND_DATA(x0);           // XEND
  
  FSMC_SEND_CMD(ILI9341_RASET);    // Row addr set
  FSMC_SEND_DATA(y0);           // YSTART
  FSMC_SEND_DATA(y1);           // YEND
  
  FSMC_SEND_CMD(ILI9341_RAMWR);     // write to RAM  
#else
  
  ENABLE_CMD();             // grab TFT CS and writecommand:
  sendData8_SPI1(ILI9341_CASET); // Column addr set
  
  SET_TFT_DC_HI;          // writeData:
#ifdef __AVR__
  sendData16_SPI1(x0);    // XSTART
  sendData16_SPI1(x0);    // XEND
#else
  sendData32_SPI1(x0, x0);
#endif // __AVR__
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RASET); // Row addr set
  
  SET_TFT_DC_HI;           // writeData:
#ifdef __AVR__
  sendData16_SPI1(y0);     // YSTART
  sendData16_SPI1(y1);     // YEND
#else
  sendData32_SPI1(y0, y1);
#endif // __AVR__
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RAMWR); // write to RAM
  
  SET_TFT_DC_HI;  // ready accept data
  
  //SET_TFT_CS_HI; // disable in other func
#endif // USE_FSMC
}

void tftSetHAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1)
{
#if USE_DMA  
  //while(DMA1_SPI1_busy());
  wait_DMA1_SPI1_busy();
#endif
  
#if USE_FSMC
  FSMC_SEND_CMD(ILI9341_CASET);    // Column addr set
  FSMC_SEND_DATA(x0);           // XSTART
  FSMC_SEND_DATA(x1);           // XEND
  
  FSMC_SEND_CMD(ILI9341_RASET);    // Row addr set
  FSMC_SEND_DATA(y0);           // YSTART
  FSMC_SEND_DATA(y0);           // YEND
  
  FSMC_SEND_CMD(ILI9341_RAMWR);     // write to RAM
#else
  
  ENABLE_CMD();             // grab TFT CS and writecommand:
  sendData8_SPI1(ILI9341_CASET); // Column addr set
  
  SET_TFT_DC_HI;          // writeData:
#ifdef __AVR__
  sendData16_SPI1(x0);    // XSTART
  sendData16_SPI1(x1);    // XEND
#else
  sendData32_SPI1(x0, x1);
#endif // __AVR__
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RASET); // Row addr set
  
  SET_TFT_DC_HI;           // writeData:
#ifdef __AVR__
  sendData16_SPI1(y0);     // YSTART
  sendData16_SPI1(y0);     // YEND
#else
  sendData32_SPI1(y0, y0);
#endif // __AVR__
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RAMWR); // write to RAM
  
  SET_TFT_DC_HI;  // ready accept data
  
  //SET_TFT_CS_HI; // disable in other func
#endif // USE_FSMC
}

void tftSetAddrPixel(uint16_t x0, uint16_t y0)
{
#if USE_DMA  
  //while(DMA1_SPI1_busy());
  wait_DMA1_SPI1_busy();
#endif
  
#if USE_FSMC
  FSMC_SEND_CMD(ILI9341_CASET);    // Column addr set
  FSMC_SEND_DATA(x0);           // XSTART
  FSMC_SEND_DATA(x0);           // XEND
  
  FSMC_SEND_CMD(ILI9341_RASET);    // Row addr set
  FSMC_SEND_DATA(y0);           // YSTART
  FSMC_SEND_DATA(y0);           // YEND
  
  FSMC_SEND_CMD(ILI9341_RAMWR);     // write to RAM
#else
  
  ENABLE_CMD();             // grab TFT CS and writecommand:
  sendData8_SPI1(ILI9341_CASET); // Column addr set
  
  SET_TFT_DC_HI;          // writeData:
#ifdef __AVR__
  sendData16_SPI1(x0);    // XSTART
  sendData16_SPI1(x0);    // XEND
#else
  sendData32_SPI1(x0, x0);
#endif // __AVR__
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RASET); // Row addr set
  
  SET_TFT_DC_HI;           // writeData:
#ifdef __AVR__
  sendData16_SPI1(y0);     // YSTART
  sendData16_SPI1(y0);     // YEND
#else
  sendData32_SPI1(y0, y0);
#endif // __AVR__
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RAMWR); // write to RAM
  
  SET_TFT_DC_HI;  // ready accept data
  
  //SET_TFT_CS_HI; // disable in other func
#endif // USE_FSMC
}

void tftSetRotation(uint8_t m)
{
  writeCommand(ILI9341_MADCTL);
  uint8_t rotation = m % 4; // can't be higher than 3
  
  switch (rotation) {
  case 0:
    writeData(MADCTL_MX | MADCTL_BGR);
    _width  = ILI9341_TFTWIDTH;
    _height = ILI9341_TFTHEIGHT;
    break;
  case 1:
    writeData(MADCTL_MV | MADCTL_BGR);
    _width  = ILI9341_TFTHEIGHT;
    _height = ILI9341_TFTWIDTH;
    break;
  case 2:
    writeData(MADCTL_MY | MADCTL_BGR);
    _width  = ILI9341_TFTWIDTH;
    _height = ILI9341_TFTHEIGHT;
    break;
  case 3:
    writeData(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
    _width  = ILI9341_TFTHEIGHT;
    _height = ILI9341_TFTWIDTH;
    break;
  }
}

// how much to scroll
void tftScrollAddress(uint16_t VSP)
{
  writeCommand(ILI9341_VSCRSADD); // Vertical scrolling start address
  writeWordData(VSP);
}

// set scrollong zone
void tftSetScrollArea(uint16_t TFA, uint16_t BFA)
{
  writeCommand(ILI9341_VSCRDEF); // Vertical scroll definition
  writeWordData(TFA);
  writeWordData(ILI9341_TFTHEIGHT - TFA - BFA);
  writeWordData(BFA);
}

uint16_t tftScroll(uint16_t lines, uint16_t yStart)
{
  for (uint16_t i = 0; i < lines; i++) {
    if ((++yStart) == (_height - TFT_BOT_FIXED_AREA)) yStart = TFT_TOP_FIXED_AREA;
    tftScrollAddress(yStart);
  }
  return  yStart;
}

uint16_t tftScrollSmooth(uint16_t lines, uint16_t yStart, uint8_t wait)
{
  for (uint16_t i = 0; i < lines; i++) {
    if ((++yStart) == (_height - TFT_BOT_FIXED_AREA)) yStart = TFT_TOP_FIXED_AREA;
    tftScrollAddress(yStart);
    _delayMS(wait);
  }
  return  yStart;
}

void tftSetSleep(bool enable)
{
  if (enable) {
    writeCommand(ILI9341_SLPIN);
    writeCommand(ILI9341_DISPOFF);
  } else {
    writeCommand(ILI9341_SLPOUT);
    writeCommand(ILI9341_DISPON);
    _delayMS(5);
  }
}

void tftSetIdleMode(bool mode)
{
  if (mode) {
    writeCommand(ILI9341_IDLEON);
  } else {
    writeCommand(ILI9341_IDLEOFF);
  }
}

void tftSetDispBrightness(uint8_t brightness)
{
  writeCommand(ILI9341_WRDBR);
  writeWordData(brightness);
}

void tftSetInvertion(bool i)
{
  writeCommand(i ? ILI9341_INVON : ILI9341_INVOFF);
}

void tftSetAdaptiveBrightness(uint8_t value)
{
  /*
  b00   Off
  b01   User Interface Image
  b10   Static Picture
  b11   Moving Image
  */
  writeCommand(ILI9341_WRCABC);
  sendData8_SPI1(value);
}
