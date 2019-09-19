#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef STM32F40XX
#include <stm32f4xx.h>
#else
#include <stm32f10x.h>
#endif /* STM32F40XX */

#include <systicktimer.h>

#include "ili9341.h"

#if USE_FSMC
#include <fsmcdrv.h>
#else
#include <spi.h>
#endif

#define RELOAD_TIMx_VAL(TIMx, freq) (SystemCoreClock / (freq * TIMx->PSC) - 1)

//-------------------------------------------------------------------------------------------//

uint32_t _ulWidth = ILI9341_TFTWIDTH;
uint32_t _ulHeight = ILI9341_TFTHEIGHT;

uint16_t curBrightnessValue = 0xff; // max value
#if USE_USER_FUNCTION
uint16_t newBrightnessValue = 0xff; // max value
uint8_t stepBrightnessValue = 0;
uint8_t dirBrightnessFade = 0;
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
  SEND_DATA16(c);
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
    if (count-- == 0)
      break;
    writeCommand(*addr++);
    while (count--) {
      writeData(*addr++);
    }
  }

  writeCommand(ILI9341_SLPOUT);    // Exit Sleep
  _delayMS(50);
  writeCommand(ILI9341_DISPON);    // Display on
}

#if defined(STM32F10X_MD) || defined(STM32F10X_HD)
void hardRstTFT(void)
{
  // toggle RST low to reset
  SET_TFT_RES_HI; _delayMS(1);
  SET_TFT_RES_LOW; _delayMS(5);
  SET_TFT_RES_HI; _delayMS(1);
}

void initTFT_GPIO()
{
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;     // Mode: output "Push-Pull"
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;// Set speed
#if USE_FSMC
  GPIO_InitStruct.GPIO_Pin = TFT_RES_PIN;
#else
  GPIO_InitStruct.GPIO_Pin = TFT_DC_PIN | TFT_RES_PIN | TFT_SS_PIN;
#endif
  GPIO_Init(GPIOB, &GPIO_InitStruct);        // Aply settings to port B
}
#endif

// TODO: on Pro version add backlight control
void init_LCDBacklight()
{
#if defined(STM32F10X_MD) || defined(STM32F10X_HD)
  RCC->APB1ENR |= RCC_APB1Periph_TIM4;        // enable TIM4 peripheral

  // configure GPIO
  GPIO_InitTypeDef backlightPort;
  backlightPort.GPIO_Mode = GPIO_Mode_AF_PP;
  backlightPort.GPIO_Speed = GPIO_Speed_2MHz;
  backlightPort.GPIO_Pin = LCD_BACKLIGHT_PIN;
  GPIO_Init(BACKLIGHT_GPIO, &backlightPort);

  // Configure timer
  TIM_TimeBaseInitTypeDef timer4Init;
  timer4Init.TIM_CounterMode = TIM_CounterMode_Up; /* Select the Counter Mode */
  timer4Init.TIM_Period = 1000; /* Set the Autoreload value */
  timer4Init.TIM_Prescaler = SystemCoreClock / 400000; /* Set the Prescaler value */
  timer4Init.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_ARRPreloadConfig(TIM4, ENABLE); /* Set the ARR Preload Bit */
  TIM_TimeBaseInit(TIM4, &timer4Init); // apply and generate update event to reload prescaler value immediately

  TIM_OCInitTypeDef timer4OC1init;
  timer4OC1init.TIM_OCMode = TIM_OCMode_PWM1; /* Select the Output Compare Mode */
  timer4OC1init.TIM_OCPolarity = TIM_OCPolarity_Low; /* Set the Output Compare Polarity */
  timer4OC1init.TIM_OutputState = TIM_OutputState_Enable; /* Set the Output State */
  timer4OC1init.TIM_Pulse = 500; /* Set the Capture Compare Register value */
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable); /* Enable the Output Compare Preload feature */
  TIM_OC1Init(TIM4, &timer4OC1init); // apply and TIM4 CH1 output compare enable

  TIM_Cmd(TIM4, ENABLE);
#else
  GPIO_InitTypeDef backlightPort;
  backlightPort.GPIO_Mode = GPIO_Mode_OUT;
  backlightPort.GPIO_OType = GPIO_OType_PP;
  backlightPort.GPIO_Pin = GPIO_Pin_1;
  backlightPort.GPIO_PuPd = GPIO_PuPd_NOPULL;
  backlightPort.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &backlightPort);

  GPIO_SetBits(GPIOB, GPIO_Pin_1); // enable backlight
#endif
}

void initLCD(void)
{
#if USE_FSMC
  init_FSMC();
  init_DMA_FSMC(); //TODO: make it work!!!
#else
  init_SPI1();
  init_DMA1_SPI1();
#endif
#if defined(STM32F10X_MD) || defined(STM32F10X_HD)
  initTFT_GPIO();

  SET_TFT_RES_LOW;
  GRAB_TFT_CS;   // maybe remove this? And connect CS to Vcc?
  SET_TFT_DC_HI;// set data

  hardRstTFT();
#endif

  init_LCDBacklight();
  execCommands(initSequence);
}

// blow your mind
void setAddrWindow(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1)
{
  WAIT_DMA_BSY;              // wait untill DMA transfer end

  WRITE_CMD(ILI9341_CASET);  // Column addr set

  SET_DATA();                // writeData:
  SEND_2_DATA(x0, x1);       // XSTART, XEND

  WRITE_CMD(ILI9341_RASET);  // Row addr set

  SET_DATA();                // writeData:
  SEND_2_DATA(y0, y1);       // YSTART, YEND

  WRITE_CMD(ILI9341_RAMWR);  // write to RAM

  SET_DATA();                // ready accept data 
}

// square window
void setSqAddrWindow(uint32_t x0, uint32_t y0, uint32_t size)
{
  WAIT_DMA_BSY;              // wait until DMA transfer end

  WRITE_CMD(ILI9341_CASET);  // Column addr set

  SET_DATA();                // writeData:
  SEND_2_DATA(x0, x0 + size);  // XSTART, XEND

  WRITE_CMD(ILI9341_RASET);  // Row addr set

  SET_DATA();                // writeData:
  SEND_2_DATA(y0, y0 + size);  // YSTART, YEND

  WRITE_CMD(ILI9341_RAMWR);  // write to RAM

  SET_DATA();                // ready accept data
}

void setVAddrWindow(uint32_t x0, uint32_t y0, uint32_t y1)
{
  WAIT_DMA_BSY;              // wait until DMA transfer end

  WRITE_CMD(ILI9341_CASET);  // Column addr set

  SET_DATA();                // writeData:
  SEND_2_DATA(x0, x0);       // XSTART, XEND

  WRITE_CMD(ILI9341_RASET);  // Row addr set

  SET_DATA();                // writeData:
  SEND_2_DATA(y0, y1);       // YSTART, YEND

  WRITE_CMD(ILI9341_RAMWR);  // write to RAM

  SET_DATA();                // ready accept data
}

void setHAddrWindow(uint32_t x0, uint32_t y0, uint32_t x1)
{
  WAIT_DMA_BSY;              // wait until DMA transfer end

  WRITE_CMD(ILI9341_CASET); // Column addr set

  SET_DATA();                // writeData:
  SEND_2_DATA(x0, x1);       // XSTART, XEND

  WRITE_CMD(ILI9341_RASET);  // Row addr set

  SET_DATA();                // writeData:
  SEND_2_DATA(y0, y0);       // YSTART, YEND

  WRITE_CMD(ILI9341_RAMWR);  // write to RAM

  SET_DATA();                // ready accept data
}

void setAddrPixel(uint32_t x0, uint32_t y0)
{
  WAIT_DMA_BSY;              // wait until DMA transfer end

  WRITE_CMD(ILI9341_CASET);  // Column addr set

  SET_DATA();                // writeData:
  SEND_2_DATA(x0, x0);       // XSTART, XEND

  WRITE_CMD(ILI9341_RASET);  // Row addr set

  SET_DATA();                // writeData:
  SEND_2_DATA(y0, y0);       // YSTART, YEND

  WRITE_CMD(ILI9341_RAMWR);  // write to RAM

  SET_DATA();                // ready accept data
}

__attribute__((optimize("O2"))) void setRotation(uint32_t m)
{
  uint32_t rotation = m % 4; // can't be higher than 3
  uint32_t madctlParam = 0;

  switch (rotation)
  {
    case 0: {
      madctlParam = (MADCTL_MX | MADCTL_BGR);
      _ulWidth = ILI9341_TFTWIDTH;
      _ulHeight = ILI9341_TFTHEIGHT;
    }
    break;
    case 1: {
      madctlParam = (MADCTL_MV | MADCTL_BGR);
      _ulWidth = ILI9341_TFTHEIGHT;
      _ulHeight = ILI9341_TFTWIDTH;
    }
    break;
    case 2: {
      madctlParam = (MADCTL_MY | MADCTL_BGR);
      _ulWidth = ILI9341_TFTWIDTH;
      _ulHeight = ILI9341_TFTHEIGHT;
    }
    break;
    case 3: {
      madctlParam = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
      _ulWidth = ILI9341_TFTHEIGHT;
      _ulHeight = ILI9341_TFTWIDTH;
    }
    break;
  }

  writeCommand(ILI9341_MADCTL);
  writeData(madctlParam);
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
    if ((++yStart) == (_ulHeight - TFT_BOT_FIXED_AREA))
      yStart = TFT_TOP_FIXED_AREA;
    scrollAddress(yStart);
  }

  return yStart;
}

uint16_t scrollScreenSmooth(uint16_t lines, uint16_t yStart, uint8_t wait)
{
  for (uint16_t i = 0; i < lines; i++) {
    if ((++yStart) == (_ulHeight - TFT_BOT_FIXED_AREA))
      yStart = TFT_TOP_FIXED_AREA;
    scrollAddress(yStart);
    _delayMS(wait);
  }

  return yStart;
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
  writeCommand(mode ? ILI9341_IDLEON : ILI9341_IDLEOFF);
}

void setDispBrightness(uint16_t brightness)
{
  /*
   writeCommand(ILI9341_WRDBR);
   writeWordData(brightness);
   */

  curBrightnessValue = brightness;

  // calc reload value
#if defined(STM32F10X_MD) || defined(STM32F10X_HD)
  TIM4->ARR = RELOAD_TIMx_VAL(TIM4, brightness);
  TIM4->CCR2 = TIM4->ARR >> 1; // set 50% duty cycle
#else
  // TODO: add timer for backlight
#endif
}

#if USE_USER_FUNCTION
void makeFadeBrightness(void)
{
  bool disableRunTime = false;

  if (dirBrightnessFade) { // fade in
    if (curBrightnessValue >= newBrightnessValue) {
      disableRunTime = true;
      ;
    } else {
      if ((curBrightnessValue + stepBrightnessValue) > 0xffff) {
        curBrightnessValue = 0xffff;
        disableRunTime = true;
      } else {
        curBrightnessValue += stepBrightnessValue;
      }
    }
  } else { // fade out
    if (curBrightnessValue <= newBrightnessValue) {
      disableRunTime = true;
    } else {
      if ((curBrightnessValue - stepBrightnessValue) < 0x00) {
        curBrightnessValue = 0x0;
        disableRunTime = true;
      } else {
        curBrightnessValue -= stepBrightnessValue;
      }
    }
  }

  if (disableRunTime) {
    setSysTimerRuntine(NULL);
  }

  // calc reload value
#if defined(STM32F10X_MD) || defined(STM32F10X_HD)
  TIM4->ARR = RELOAD_TIMx_VAL(TIM4, curBrightnessValue);
  TIM4->CCR2 = TIM4->ARR >> 1; // set 50% duty cycle
#else
  // TODO: add timer for backlight
#endif
}

void setDispBrightnessFade(uint8_t dir, uint16_t newValue, uint8_t step)
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
  WRITE_DATA(value);
}
