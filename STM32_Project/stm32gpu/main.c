/*
*
* STM32_GPU Project
* Creation start: 10.04.2016 20:21 (UTC+4)
*
* Created by: Antonov Alexandr (Bismuth208)
*
* For addition info look read.me
*
*/

#include <string.h>
#include <stdint.h>

#include <stm32f10x.h>

#include <gfx.h>
#if USE_FSMC
 #include <fsmcdrv.h>
#else
 #include <spi.h>
#endif
#include <sdcard_spi.h>
#include <systicktimer.h>

#include "sdLoader.h"
#include "gpuMain.h"
#include "gpuTiles.h"


//===========================================================================//


void init_GPIO_RCC(void)
{
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOA); // GPIOA
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOB); // GPIOB
  
#ifdef STM32F10X_MD
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOC); // GPIOC
#endif

#ifdef STM32F10X_HD
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOE); // GPIOE
  
 #if USE_FSMC
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOC); // GPIOC
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOD); // GPIOD
 #endif  
#endif
}

void startupInit(void)
{
  /*
  * NOTE!
  * STM32 confugired for 72Mhz Sys clock!
  * to change that go to "system_stm32f10x.c"
  * and find "#define SYSCLK_FREQ_72MHz  72000000"
  * select what you need
  */
  SystemInit(); // init clocks and another s...
  initSysTickTimer();
  
  init_GPIO_RCC();
  initRand();
  initLCD();         /* initialize a ILI9341 chip */
  
  setRotation(1); // horizontal
  loadDefaultPalette();
  initRaycasterPointers();
  drawBootLogo();
  
  // Init SD and SPI_2 after all inited
  sd_spi_init(); // init SPI_2 for SD card
  init_sdCard();
}

//------------------------- yep, here's how it all began... -------------------//
__noreturn void main(void)
{
  startupInit();
  
  init_GPU();
  sync_CPU();   /* make a sync whith a CPU */
  run_GPU();
}
//-----------------------------------------------------------------------------//

#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif
