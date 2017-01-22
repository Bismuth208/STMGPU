#include <stm32f10x.h>
#include <stm32f10x_dma.h>

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <systicktimer.h>

#include "ili9341.h"

#if USE_FSMC
 #include <fsmcdrv.h>
#else
 #include <spi.h>
#endif

#include <gfxDMA.h>

//-------------------------------------------------------------------------------------------//

int16_t _width  = ILI9341_TFTWIDTH;
int16_t _height = ILI9341_TFTHEIGHT;

//-------------------------------------------------------------------------------------------//

void writeCommand(uint8_t c)
{
#if USE_FSMC
  sendCMD8_FSMC(c);
#else

  ENABLE_CMD();
  sendData8_SPI1(c);
#endif
}

void writeData(uint8_t c)
{
#if USE_FSMC
  sendData8_FSMC(c);
#else

  ENABLE_DATA();
  sendData8_SPI1(c);
#endif
}

void writeWordData(uint16_t c)
{
#if USE_FSMC
  sendData16_FSMC(c);
#else

  ENABLE_DATA();
  sendData16_SPI1(c);
#endif
}

void execCommands(const uint8_t *addr)
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

void initTFT_GPIO()
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;     // Mode: output "Push-Pull"
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;     // Set speed
  GPIO_InitStruct.GPIO_Pin = TFT_DC_PIN | TFT_RES_PIN | TFT_SS_PIN;
  GPIO_Init(GPIOB, &GPIO_InitStruct);        // Aply settings to port B
}

void initLCD(void)
{
#if USE_FSMC
  initFSMC_GPIO();
  initFSMC();
#else
  initTFT_GPIO();
#endif
  
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
  
  execCommands(initSequence);
  
  writeCommand(ILI9341_SLPOUT);    //Exit Sleep
  _delayMS(5);
  writeCommand(ILI9341_DISPON);    //Display on
  
  init_DMA1_SPI1();
}

// blow your mind
void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{    
#if USE_FSMC
  sendCMD8_FSMC(ILI9341_CASET);    // Column addr set
  sendData32_FSMC(x0, x1); // XSTART, XEND
  
  sendCMD8_FSMC(ILI9341_RASET);    // Row addr set
  sendData32_FSMC(y0, y1); // YSTART, YEND
  
  sendCMD8_FSMC(ILI9341_RAMWR);     // write to RAM
  
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
  sendCMD8_FSMC(ILI9341_CASET);    // Column addr set
  sendData32_FSMC(x0, x0+size); // XSTART, XEND
  
  sendCMD8_FSMC(ILI9341_RASET);    // Row addr set
  sendData32_FSMC(y0, y0+size); // YSTART, YEND
  
  sendCMD8_FSMC(ILI9341_RAMWR);     // write to RAM
  
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
  sendCMD8_FSMC(ILI9341_CASET);    // Column addr set
  sendData32_FSMC(x0, x0); // XSTART, XEND
  
  sendCMD8_FSMC(ILI9341_RASET);    // Row addr set
  sendData32_FSMC(y0, y1); // YSTART, YEND
  
  sendCMD8_FSMC(ILI9341_RAMWR);     // write to RAM  
  
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
  sendCMD8_FSMC(ILI9341_CASET);    // Column addr set
  sendData32_FSMC(x0, x1); // XSTART, XEND
  
  sendCMD8_FSMC(ILI9341_RASET);    // Row addr set
  sendData32_FSMC(y0, y0); // YSTART, YEND
  
  sendCMD8_FSMC(ILI9341_RAMWR);     // write to RAM
  
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
  sendCMD8_FSMC(ILI9341_CASET);    // Column addr set
  sendData32_FSMC(x0, x0); // XSTART, XEND
  
  sendCMD8_FSMC(ILI9341_RASET);    // Row addr set
  sendData32_FSMC(y0, y0); // YSTART, YEND
  
  sendCMD8_FSMC(ILI9341_RAMWR);     // write to RAM
  
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
#if USE_FSMC
  sendData8_FSMC(value);
#else
  sendData8_SPI1(value);
#endif
}
