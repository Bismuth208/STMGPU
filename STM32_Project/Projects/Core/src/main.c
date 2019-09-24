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
#include "tone.h"
#include "gpio_list.h"

//===========================================================================//

void vInitRCC(void)
{
#ifdef STM32F10X_MD
  RCC->APB1ENR |= (
      RCC_APB1Periph_TIM3  // clock TIM3 peripheral
      | RCC_APB1Periph_TIM4  // clock TIM4 peripheral
      | RCC_APB1Periph_SPI2  // clock for SPI2
  );

  RCC->APB2ENR |= (
      RCC_APB2Periph_AFIO        // enable remap config for GPIO
      | RCC_APB2ENR_SPI1EN         // clock for SPI1
      | RCC_APB2Periph_USART1
      | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC
  );

  RCC->AHBENR |= (
      RCC_AHBPeriph_DMA1    // enable clocks to DMA1
//      | RCC_AHBPeriph_DMA2
  );

#ifdef USART1_USE_PROTOCOL_V1
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
#endif

#else
  RCC_AHB1PeriphClockCmd(
      RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOB
          | RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

#ifdef USART1_USE_PROTOCOL_V1
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
#endif

   // TODO: add magic for "Pro-Pill" in timer
#endif
}

void vInitGPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  // ================= init buzzer =================
#ifdef STM32F10X_MD
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_Pin   = SOUND_PIN;
  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); // remap PA7 to PB5

#elif STM32F40XX
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
#endif

  GPIO_Init(SOUND_GPIO, &GPIO_InitStruct);


  // ================= init common sGPU =================
#ifdef STM32F10X_MD
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;      // Mode: output "Push-Pull"
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;// Set speed
  GPIO_InitStruct.GPIO_Pin = GPU_BSY_PIN;
  GPIO_Init(GPU_BSY_PORT, &GPIO_InitStruct);// Apply settings

  GPIO_InitStruct.GPIO_Pin = GPU_BSY_LED_PIN;// LED BSY PIN
  GPIO_Init(GPU_BSY_LED_PORT, &GPIO_InitStruct);// Apply settings
  GPIO_SET_PIN(GPU_BSY_LED_PORT, GPU_BSY_LED_PIN);// turn off

  // Now init baud rate selection GPIO
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;// Mode: input "Pull-down"
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;// Set speed
  GPIO_InitStruct.GPIO_Pin = (GPU_BAUD_SELECT_PIN_0 | GPU_BAUD_SELECT_PIN_1 | GPU_BAUD_SELECT_PIN_2);
  GPIO_Init(GPU_BAUD_SELECT_PORT, &GPIO_InitStruct);// Apply settings

#elif STM32F40XX
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Pin = GPU_BSY_PIN;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPU_BSY_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = GPU_BSY_LED_PIN;
  GPIO_Init(GPU_BSY_LED_PORT, &GPIO_InitStruct);
  GPIO_SET_PIN(GPU_BSY_LED_PORT, GPU_BSY_LED_PIN);      // turn off

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
//  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Pin = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif


  // ================= init UART =================
#ifdef STM32F10X_MD
  // set PA10 as input UART (RxD)
  GPIO_InitStruct.GPIO_Pin = GPU_UART_RX_PIN;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPU_UART_PORT, &GPIO_InitStruct);

  // set PA9 as output UART (TxD)
  GPIO_InitStruct.GPIO_Pin = GPU_UART_TX_PIN;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPU_UART_PORT, &GPIO_InitStruct);
#elif STM32F40XX
  // set PA10 as input UART (RxD)
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  // set PA9 as output UART (TxD)
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
#endif

  // ================= init SPI1 =================
  //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //disable JTAG, SW left enabled
  //GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;      // Alternate fun mode
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_SPI_LCD_MOSI | GPIO_Pin_SPI_LCD_SCK;
  GPIO_Init(GPIO_SPI_LCD, &GPIO_InitStruct);        // apply settings


  // ================= init TFT =================
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;     // Mode: output "Push-Pull"
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;// Set speed
#if USE_FSMC
  GPIO_InitStruct.GPIO_Pin = TFT_RES_PIN;
#else
  GPIO_InitStruct.GPIO_Pin = TFT_DC_PIN | TFT_RES_PIN | TFT_SS_PIN;
#endif
  GPIO_Init(BACKLIGHT_GPIO, &GPIO_InitStruct);        // Aply settings to port B


  // configure backlight
#ifdef STM32F10X_MD
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_Pin = LCD_BACKLIGHT_PIN;
  GPIO_Init(BACKLIGHT_GPIO, &GPIO_InitStruct);
#endif

  // ================= init SD_card SPI =================
#ifdef STM32F10X_MD
  /* Configure I/O for Flash Chip select */
  GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_SPI_SD_CS;
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_SPI_SD_CS, &GPIO_InitStruct);

  /* Configure SPI pins: SCK and MOSI with default alternate function (not re-mapped) push-pull */
  GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_SPI_SD_SCK | GPIO_Pin_SPI_SD_MOSI;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_Init(GPIO_SPI_SD, &GPIO_InitStruct);
  /* Configure MISO as Input with internal pull-up */
  GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_SPI_SD_MISO;
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
  GPIO_Init(GPIO_SPI_SD, &GPIO_InitStruct);
#elif STM32F40XX
  /* Configure I/O for Flash Chip select */
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_SPI_SD_CS;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_SPI_SD_CS, &sd_gpio_init);

  /* Configure SPI pins: SCK and MOSI with default alternate function (not re-mapped) push-pull */
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_SPI_SD_SCK | GPIO_Pin_SPI_SD_MOSI;;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_SPI_SD_CS, &GPIO_InitStruct);

  /* Configure MISO as Input with internal pull-up */
  GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_SPI_SD_MISO;
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIO_SPI_SD, &GPIO_InitStruct);
#endif
}

void vInitCore(void)
{
  /*
   * NOTE!
   * As different MCU is used (F1 and F4) then different clocks must be inited!
   *
   * For example:
   *  - stm32f1xx works on 72Mhz Sys clock and use "#define SYSCLK_FREQ_72MHz  72000000";
   *  - stm32f4xx work on 80MHz Sys clock and use a lot of manual settings in PLLs.
   *
   * Both initialization located in "system_stm32fyxx.c", where "y" mean model f1 or f4;
   */

  SystemInit(); // init clocks and another s...
  initSysTickTimer();

  vInitRCC();
  vInitGPIO();
#if USE_FSMC
  init_FSMC();
  init_DMA_FSMC(); //TODO: make it work!!!
#else
  vInit_SPI1();
  vInit_DMA1_SPI1();
#endif

//  initRand();
  vInit_TFT(); /* initialize a ILI9341 chip */
  vInit_BacklightTFT();

  setRotation(1); // horizontal
  loadDefaultPalette();
  initRaycasterPointers();
  drawBootLogo();

  vInit_Sound();

  // Init SD and SPI_2 after all inited
  vInit_SPI2();
}

//------------------------- yep, here's how it all began... -------------------//
int main(void) // __noreturn void
{
  vInitCore();

  init_GPU();
  sync_CPU(); /* make a sync with a CPU */
  run_GPU();
}
//-----------------------------------------------------------------------------//
