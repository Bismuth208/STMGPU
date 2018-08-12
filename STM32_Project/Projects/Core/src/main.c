/*
 * STM32_GPU Project
 * Creation start: 10.04.2016 20:21 (UTC+4)
 * Last edit: 01.08.2018
 *
 * author: Antonov Alexandr (Bismuth208)
 *
 * For addition info look read.me
 *
 */

#include <string.h>
#include <stdint.h>

#ifdef STM32F40XX
#include <stm32f4xx.h>
#else
#include <stm32f10x.h>
#endif /* STM32F40XX */

#include <gfx.h>
#include <sdcard_spi.h>
#include <systicktimer.h>
#if USE_FSMC
 #include <fsmcdrv.h>
#else
 #include <spi.h>
#endif

#include "sdLoader.h"
#include "gpuMain.h"
#include "tiles.h"

//===========================================================================//


void init_GPIO_RCC(void)
{
#if defined(STM32F10X_MD) || defined(STM32F10X_HD)
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
#else
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
#endif

#ifdef STM32F10X_MD
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
#endif

#ifdef STM32F10X_HD
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

 #if USE_FSMC
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
 #endif
#endif
}

void startupInit(void)
{
  /*
   * NOTE!
   * As different MCU is used (F1 and F4) then different clocks must be inited!
   *
   * For example:
   *  - stm32f1xx works on 72Mhz Sys clock and use "#define SYSCLK_FREQ_72MHz  72000000";
   *  - stm32f4xx work on 168MHz Sys clock and use a lot of manual settings in PLLs.
   *
   * Both initialization located in "system_stm32fyxx.c", where "y" mean model f1 or f4;
   */

  SystemInit(); // init clocks and another s...
  initSysTickTimer();

  init_GPIO_RCC();
//  initRand();
  initLCD();         /* initialize a ILI9341 chip */

  setRotation(1); // horizontal
  loadDefaultPalette();
  initRaycasterPointers();
  drawBootLogo();

  // Init SD and SPI_2 after all inited
  sd_spi_init(); // init SPI_2 or SPI_3 for SD card
}

//------------------------- yep, here's how it all began... -------------------//
int main(void) // __noreturn void
{
  startupInit();

  init_GPU();
  sync_CPU();   /* make a sync whith a CPU */
  run_GPU();
}
//-----------------------------------------------------------------------------//
