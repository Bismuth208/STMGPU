#include <stm32f10x.h>
#include <stm32f10x_dma.h>

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <systicktimer.h>
#include <spi.h>

#include "ili9341.h"

#include <gfxDMA.h>

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
#endif
}

void writeData(uint8_t c)
{
#if USE_FSMC
  FSMC_SEND_DATA(c);
#else
  ENABLE_DATA();
  
  sendData8_SPI1(c);
#endif
}

void writeWordData(uint16_t c)
{
#if USE_FSMC
  FSMC_SEND_DATA(c);
#else
  ENABLE_DATA();
  
  sendData16_SPI1(c);
#endif
}

void commandList(const uint8_t *addr)
{
  uint8_t count;
  while (1) {
    count = *addr++;
    if (count-- == 0) break;
    writeCommand(*addr++);
    while (count--) {
      writeData(*addr++);
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
#if USE_FSMC
  initTFT_FSMC();
#else
  GPIO_InitTypeDef GPIO_InitStruct;
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;     // Mode: output "Push-Pull"
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;     // Set speed
  GPIO_InitStruct.GPIO_Pin = TFT_DC_PIN | TFT_RES_PIN | TFT_SS_PIN;
  GPIO_Init(GPIOB, &GPIO_InitStruct);        // Aply settings to port B
#endif // USE_FSMC
}

void tftBegin(void)
{
  initTFT_GPIO();
  SET_TFT_RES_LOW;
  
#if TFT_CS_ALWAS_ACTIVE
  GRAB_TFT_CS; // maybe remove this? And connect CS to Vcc?
#endif
  
  SET_TFT_CS_HI;
  SET_TFT_DC_HI;
  
  // toggle RST low to reset
  SET_TFT_RES_HI;
  _delayMS(1);
  SET_TFT_RES_LOW;
  _delayMS(1);
  SET_TFT_RES_HI;
  _delayMS(5);
  
  commandList(init_commands);
  
  writeCommand(ILI9341_SLPOUT);    //Exit Sleep
  _delayMS(5);
  writeCommand(ILI9341_DISPON);    //Display on
  
  init_DMA1_SPI1();
}

// blow your mind
void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{    
#if USE_FSMC
  FSMC_SEND_CMD(ILI9341_CASET);    // Column addr set
  FSMC_SEND_DATA(x0);           // XSTART
  FSMC_SEND_DATA(x1);           // XEND
  
  FSMC_SEND_CMD(ILI9341_RASET);    // Row addr set
  FSMC_SEND_DATA(y0);           // YSTART
  FSMC_SEND_DATA(y1);           // YEND
  
  FSMC_SEND_CMD(ILI9341_RAMWR);     // write to RAM
  
#else
  wait_DMA1_SPI1_busy();
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_CASET); // Column addr set
  
  SET_TFT_DC_HI;          // writeData:
  sendData32_SPI1(x0, x1); // XSTART, XEND
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RASET); // Row addr set
  
  SET_TFT_DC_HI;           // writeData:
  sendData32_SPI1(y0, y1); // YSTART, YEND
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RAMWR); // write to RAM
  
  SET_TFT_DC_HI;  // ready accept data
#endif // USE_FSMC  
}

// square window
void setSqAddrWindow(uint16_t x0, uint16_t y0, uint16_t size)
{
#if USE_FSMC
  FSMC_SEND_CMD(ILI9341_CASET);    // Column addr set
  FSMC_SEND_DATA(x0);           // XSTART
  FSMC_SEND_DATA(x0+size);           // XEND
  
  FSMC_SEND_CMD(ILI9341_RASET);    // Row addr set
  FSMC_SEND_DATA(y0);           // YSTART
  FSMC_SEND_DATA(y0+size);           // YEND
  
  FSMC_SEND_CMD(ILI9341_RAMWR);     // write to RAM
  
#else
  wait_DMA1_SPI1_busy();
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_CASET); // Column addr set
  
  SET_TFT_DC_HI;          // writeData:
  sendData32_SPI1(x0, x0+size); // XSTART, XEND
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RASET); // Row addr set
  
  SET_TFT_DC_HI;           // writeData:
  sendData32_SPI1(y0, y0+size); // YSTART, YEND
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RAMWR); // write to RAM
  
  SET_TFT_DC_HI;  // ready accept data
#endif // USE_FSMC 
}

void setVAddrWindow(uint16_t x0, uint16_t y0, uint16_t y1)
{
#if USE_FSMC
  FSMC_SEND_CMD(ILI9341_CASET);    // Column addr set
  FSMC_SEND_DATA(x0);           // XSTART
  FSMC_SEND_DATA(x0);           // XEND
  
  FSMC_SEND_CMD(ILI9341_RASET);    // Row addr set
  FSMC_SEND_DATA(y0);           // YSTART
  FSMC_SEND_DATA(y1);           // YEND
  
  FSMC_SEND_CMD(ILI9341_RAMWR);     // write to RAM  
  
#else
  wait_DMA1_SPI1_busy();
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_CASET); // Column addr set
  
  SET_TFT_DC_HI;          // writeData:
  sendData32_SPI1(x0, x0); // XSTART, XEND
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RASET); // Row addr set
  
  SET_TFT_DC_HI;           // writeData:
  sendData32_SPI1(y0, y1); // YSTART, YEND
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RAMWR); // write to RAM
  
  SET_TFT_DC_HI;  // ready accept data
#endif // USE_FSMC
}

void setHAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1)
{
#if USE_FSMC
  FSMC_SEND_CMD(ILI9341_CASET);    // Column addr set
  FSMC_SEND_DATA(x0);           // XSTART
  FSMC_SEND_DATA(x1);           // XEND
  
  FSMC_SEND_CMD(ILI9341_RASET);    // Row addr set
  FSMC_SEND_DATA(y0);           // YSTART
  FSMC_SEND_DATA(y0);           // YEND
  
  FSMC_SEND_CMD(ILI9341_RAMWR);     // write to RAM
  
#else
  wait_DMA1_SPI1_busy();
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_CASET); // Column addr set
  
  SET_TFT_DC_HI;          // writeData:
  sendData32_SPI1(x0, x1); // XSTART, XEND
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RASET); // Row addr set
  
  SET_TFT_DC_HI;           // writeData:
  sendData32_SPI1(y0, y0); // YSTART, YEND
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RAMWR); // write to RAM
  
  SET_TFT_DC_HI;  // ready accept data
#endif // USE_FSMC
}

void setAddrPixel(uint16_t x0, uint16_t y0)
{ 
#if USE_FSMC
  FSMC_SEND_CMD(ILI9341_CASET);    // Column addr set
  FSMC_SEND_DATA(x0);           // XSTART
  FSMC_SEND_DATA(x0);           // XEND
  
  FSMC_SEND_CMD(ILI9341_RASET);    // Row addr set
  FSMC_SEND_DATA(y0);           // YSTART
  FSMC_SEND_DATA(y0);           // YEND
  
  FSMC_SEND_CMD(ILI9341_RAMWR);     // write to RAM
  
#else
  wait_DMA1_SPI1_busy();
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_CASET); // Column addr set
  
  SET_TFT_DC_HI;          // writeData:
  sendData32_SPI1(x0, x0); // XSTART, XEND
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RASET); // Row addr set
  
  SET_TFT_DC_HI;           // writeData:
  sendData32_SPI1(y0, y0); // YSTART, YEND
  
  SET_TFT_DC_LOW;             // writecommand:
  sendData8_SPI1(ILI9341_RAMWR); // write to RAM
  
  SET_TFT_DC_HI;  // ready accept data
#endif // USE_FSMC
}

void setRotation(uint8_t m)
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
void scrollAddress(uint16_t VSP)
{
  writeCommand(ILI9341_VSCRSADD); // Vertical scrolling start address
  writeWordData(VSP);
}

// set scrollong zone
void setScrollArea(uint16_t TFA, uint16_t BFA)
{
  writeCommand(ILI9341_VSCRDEF); // Vertical scroll definition
  writeWordData(TFA);
  writeWordData(ILI9341_TFTHEIGHT - TFA - BFA);
  writeWordData(BFA);
}

uint16_t scrollScreen(uint16_t lines, uint16_t yStart)
{
  for (uint16_t i = 0; i < lines; i++) {
    if ((++yStart) == (_height - TFT_BOT_FIXED_AREA)) yStart = TFT_TOP_FIXED_AREA;
    scrollAddress(yStart);
  }
  return  yStart;
}

uint16_t scrollScreenSmooth(uint16_t lines, uint16_t yStart, uint8_t wait)
{
  for (uint16_t i = 0; i < lines; i++) {
    if ((++yStart) == (_height - TFT_BOT_FIXED_AREA)) yStart = TFT_TOP_FIXED_AREA;
    scrollAddress(yStart);
    _delayMS(wait);
  }
  return  yStart;
}

void setSleep(bool enable)
{
  if (enable) {
    writeCommand(ILI9341_SLPIN);
    writeCommand(ILI9341_DISPOFF);
  } else {
    writeCommand(ILI9341_SLPOUT);
    writeCommand(ILI9341_DISPON);
    _delayMS(1);
  }
}

void setIdleMode(bool mode)
{
  if (mode) {
    writeCommand(ILI9341_IDLEON);
  } else {
    writeCommand(ILI9341_IDLEOFF);
  }
}

void setDispBrightness(uint8_t brightness)
{
  writeCommand(ILI9341_WRDBR);
  writeWordData(brightness);
}

void setInvertion(bool i)
{
  writeCommand(i ? ILI9341_INVON : ILI9341_INVOFF);
}

void setAdaptiveBrightness(uint8_t value)
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
