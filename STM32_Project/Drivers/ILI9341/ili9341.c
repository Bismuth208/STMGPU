#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <stm32f10x.h>

#include <systicktimer.h>
#if USE_FSMC
 #include <fsmcdrv.h>
#else
 #include <spi.h>
#endif

#include "ili9341.h"


#define RELOAD_TIMx_VAL(TIMx, freq) (SystemCoreClock / (freq * TIMx->PSC) - 1)

//-------------------------------------------------------------------------------------------//

int16_t _width  = ILI9341_TFTWIDTH;
int16_t _height = ILI9341_TFTHEIGHT;


uint8_t curBrightnessValue = 0xff; // max value
#if USE_USER_FUNCTION
uint8_t newBrightnessValue = 0xff; // max value
uint8_t stepBrightnessValue =0;
uint8_t dirBrightnessFade =0;
#endif

//-------------------------------------------------------------------------------------------//

void writeCommand(uint8_t c)
{
  WRITE_CMD(c);
}

void writeData(uint8_t c)
{
  WRITE_DATA(c);
}

void writeWordData(uint16_t c)
{
#if USE_FSMC
  FSMC_SEND_DATA(c);
#else

  SET_DATA();
  sendData16_SPI1(c);
#endif
}

/*
uint8_t readData8()
{
  return readData8_SPI1(ILI9341_NOP);
}
*/

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
  
  writeCommand(ILI9341_SLPOUT);    // Exit Sleep
  _delayMS(3);
  writeCommand(ILI9341_DISPON);    // Display on
}

void hardRstTFT(void)
{
  // toggle RST low to reset
  SET_TFT_RES_HI;  _delayMS(1);
  SET_TFT_RES_LOW; _delayMS(5);
  SET_TFT_RES_HI;  _delayMS(1);
}

void initTFT_GPIO()
{
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;     // Mode: output "Push-Pull"
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    // Set speed
#if USE_FSMC
  GPIO_InitStruct.GPIO_Pin = TFT_RES_PIN;
#else
  GPIO_InitStruct.GPIO_Pin = TFT_DC_PIN | TFT_RES_PIN | TFT_SS_PIN;
#endif
  GPIO_Init(GPIOB, &GPIO_InitStruct);        // Aply settings to port B
}

void init_LCDBacklight()
{
  RCC->APB1ENR |= RCC_APB1Periph_TIM4;        // enable TIM4 peripheral
  
  // configure GPIO
  GPIO_InitTypeDef backlightPort;
  backlightPort.GPIO_Mode  = GPIO_Mode_AF_PP;
  backlightPort.GPIO_Speed = GPIO_Speed_2MHz;
  backlightPort.GPIO_Pin   = LCD_BACKLIGHT_PIN;
  GPIO_Init(BACKLIGHT_GPIO, &backlightPort);
  
  // Configure timer
  TIM_TimeBaseInitTypeDef timer4Init;
  timer4Init.TIM_CounterMode = TIM_CounterMode_Up;        /* Select the Counter Mode */
  timer4Init.TIM_Period =  1000;                          /* Set the Autoreload value */
  timer4Init.TIM_Prescaler =  SystemCoreClock / 4000000;  /* Set the Prescaler value */
  TIM_ARRPreloadConfig(TIM4, ENABLE);                     /* Set the ARR Preload Bit */
  TIM_TimeBaseInit(TIM4, &timer4Init);                    // apply and generate update event to reload prescaler value immediately
  
  TIM_OCInitTypeDef timer4OC1init;
  timer4OC1init.TIM_OCMode = TIM_OCMode_PWM1;             /* Select the Output Compare Mode */
  timer4OC1init.TIM_OCPolarity = TIM_OCPolarity_Low;      /* Set the Output Compare Polarity */
  timer4OC1init.TIM_OutputState = TIM_OutputState_Enable; /* Set the Output State */
  timer4OC1init.TIM_Pulse = 500;                          /* Set the Capture Compare Register value */
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);       /* Enable the Output Compare Preload feature */
  TIM_OC1Init(TIM4, &timer4OC1init);                      // apply and TIM4 CH1 output compare enable
}

void initLCD(void)
{
#if USE_FSMC
  initFSMC();
#else
  init_SPI1();
  init_DMA1_SPI1();
#endif
  initTFT_GPIO();
  
  //init_LCDBacklight();
  
  SET_TFT_RES_LOW;
  GRAB_TFT_CS;   // maybe remove this? And connect CS to Vcc?
  SET_TFT_DC_HI; // set data
  
  hardRstTFT();
  execCommands(initSequence);
}

// blow your mind
void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{    
  WAIT_DMA_BSY;              // wait untill DMA transfer end
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ILI9341_CASET);  // Column addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(x0, x1);       // XSTART, XEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ILI9341_RASET);  // Row addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(y0, y1);       // YSTART, YEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ILI9341_RAMWR);  // write to RAM
  
  SET_DATA();                // ready accept data 
}

// square window
void setSqAddrWindow(uint16_t x0, uint16_t y0, uint16_t size)
{
  WAIT_DMA_BSY;              // wait untill DMA transfer end
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ILI9341_CASET);  // Column addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(x0, x0+size);  // XSTART, XEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ILI9341_RASET);  // Row addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(y0, y0+size);  // YSTART, YEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ILI9341_RAMWR);  // write to RAM
  
  SET_DATA();                // ready accept data
}

void setVAddrWindow(uint16_t x0, uint16_t y0, uint16_t y1)
{
  WAIT_DMA_BSY;              // wait untill DMA transfer end
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ILI9341_CASET);  // Column addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(x0, x0);       // XSTART, XEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ILI9341_RASET);  // Row addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(y0, y1);       // YSTART, YEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ILI9341_RAMWR);  // write to RAM
  
  SET_DATA();                // ready accept data
}

void setHAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1)
{
  WAIT_DMA_BSY;              // wait untill DMA transfer end
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ILI9341_CASET);  // Column addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(x0, x1);       // XSTART, XEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ILI9341_RASET);  // Row addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(y0, y0);       // YSTART, YEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ILI9341_RAMWR);  // write to RAM
  
  SET_DATA();                // ready accept data
}

void setAddrPixel(uint16_t x0, uint16_t y0)
{ 
  WAIT_DMA_BSY;              // wait untill DMA transfer end
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ILI9341_CASET);  // Column addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(x0, x0);       // XSTART, XEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ILI9341_RASET);  // Row addr set
  
  SET_DATA();                // writeData:
  SEND_2_DATA(y0, y0);       // YSTART, YEND
  
  SET_CMD();                 // writecommand:
  SEND_DATA(ILI9341_RAMWR);  // write to RAM
  
  SET_DATA();                // ready accept data
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
  /*
  writeCommand(ILI9341_WRDBR);
  writeWordData(brightness);
  */
  
  curBrightnessValue = brightness;
  
  // calc reload value
  TIM4->ARR  = RELOAD_TIMx_VAL(TIM4, brightness);
  TIM4->CCR2 = TIM4->ARR >> 1; // set 50% duty cycle
}

#if USE_USER_FUNCTION
void makeFadeBrightness(void)
{
  bool disableRunTime = false;
  
  if(dirBrightnessFade) { // fade in
    if(curBrightnessValue >= newBrightnessValue) {
      disableRunTime = true;;
    } else {
      if((curBrightnessValue + stepBrightnessValue) > 0xff) {
        curBrightnessValue = 0xff;
        disableRunTime = true;
      } else {
        curBrightnessValue += stepBrightnessValue;
      }
    }
  } else { // fade out
    if(curBrightnessValue <= newBrightnessValue) {
      disableRunTime = true;
    } else {
      if((curBrightnessValue - stepBrightnessValue) < 0x00) {
        curBrightnessValue = 0x0;
        disableRunTime = true;
      } else {
        curBrightnessValue -= stepBrightnessValue;
      }
    }
  }
  
  if(disableRunTime) {
    setSysTimerRuntine(NULL);
  }
  
  // calc reload value
  TIM4->ARR  = RELOAD_TIMx_VAL(TIM4, curBrightnessValue);
  TIM4->CCR2 = TIM4->ARR >> 1; // set 50% duty cycle
}

void setDispBrightnessFade(uint8_t dir, uint8_t newValue, uint8_t step)
{
  dirBrightnessFade = dir;
  newBrightnessValue = newValue;
  stepBrightnessValue = step;
  
  setSysTimerRuntine(makeFadeBrightness);
}
#endif

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
  FSMC_SEND_DATA((uint16_t)((0x00<<8)|value));
#else
  sendData8_SPI1(value);
#endif
}
