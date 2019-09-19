/*
 *
 * Created by: Antonov Alexandr (Bismuth208)
 *
 */

#include <string.h>

#ifdef STM32F40XX
#include <stm32f4xx.h>
#else
#include <stm32f10x.h>
#endif /* STM32F40XX */

#include <gfx.h>
#include <uart.h>
#include <memHelper.h>
#include <tone.h>

#include "gpuMain.h"
#include "sdLoader.h"

//===========================================================================//

// Make sync whith CPU by read from UART buffer 
// sync sequence: 0x42DD
void sync_CPU(void)
{
  print(T_WAIT_SYNC);

  bool syncEstablished = false;

#ifdef USART1_USE_PROTOCOL_V1
  fflush_buffer_USART1();
  ucSearchSyncSeq();

  syncEstablished = true;

  print(T_OK T_TFT_SIZE);

  // return screen size to CPU
  cmdBuffer.par1 = _width;
  cmdBuffer.par2 = _height;

  GPU_INTERFACE_SEND_DATA_8(SYNC_OK);// sequence right, answer to CPU
  fflush_UART1();
  GPU_INTERFACE_SEND_ARR_DATA_8(cmdBuffer.data, 4);// send resolution

  print(T_OK T_GPU_START);
#endif

#ifdef USART1_USE_PROTOCOL_V0 // old protocol
  while (!syncEstablished) {

    if (GPU_INTERFACE_GET_AVALIABLE_DATA() >= 2) {
      if (GPU_INTERFACE_GET_DATA_8() == 0x42) {
        if (GPU_INTERFACE_GET_DATA_8() == 0xDD) {

          GPU_INTERFACE_FFLUSH();
          syncEstablished = true;

          print(T_OK T_TFT_SIZE);

          // return screen size to CPU
          cmdBuffer.par1 = (uint16_t) _ulWidth;
          cmdBuffer.par2 = (uint16_t) _ulHeight;

          GPU_INTERFACE_SEND_DATA_8(SYNC_OK);  // sequence right, answer to CPU
          GPU_INTERFACE_SEND_ARR_DATA_8(cmdBuffer.data, 4); // send resolution

          print(T_OK T_GPU_START);
        }
      }
    }
  }
#endif

  // clear output
  fillScreen(COLOR_BLACK);
}

void init_GPU_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

#ifdef STM32F40XX
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Pin = GPU_BSY_PIN;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPU_BSY_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin = GPU_BSY_LED_PIN;
  GPIO_Init(GPU_BSY_LED_PORT, &GPIO_InitStruct);
  GPIO_SetBits(GPU_BSY_LED_PORT, GPU_BSY_LED_PIN);      // turn off

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
//  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Pin = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

#else
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;      // Mode: output "Push-Pull"
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;// Set speed
  GPIO_InitStruct.GPIO_Pin = GPU_BSY_PIN;
  GPIO_Init(GPU_BSY_PORT, &GPIO_InitStruct);// Apply settings

  GPIO_InitStruct.GPIO_Pin = GPU_BSY_LED_PIN;// LED BSY PIN
  GPIO_Init(GPU_BSY_LED_PORT, &GPIO_InitStruct);// Apply settings
  GPIO_SetBits(GPU_BSY_LED_PORT, GPU_BSY_LED_PIN);// turn off

  // Now init baud rate selection GPIO
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;// Mode: input "Pull-down"
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;// Set speed
  GPIO_InitStruct.GPIO_Pin = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);
  GPIO_Init(GPIOA, &GPIO_InitStruct);// Apply settings
#endif
}

void init_GPU_UART(void)
{
  print(T_SELECT_WAY T_UART_WAY);
  print(T_BAUD_SPEED);

  switch (GPIOA->IDR & 0x07)
  // get GPIO state
  {
    case 0x01: {
      init_UART1(UART_BAUD_9600);
      print(T_BAUD_9600);
    }
    break;

    case 0x02: {
      init_UART1(UART_BAUD_57600);
      print(T_BAUD_57K);
    }
    break;

    case 0x03: {
      init_UART1(UART_BAUD_115200);
      print(T_BAUD_115K);
    }
    break;

    case 0x04: {
      init_UART1(UART_BAUD_1M);
      print(T_BAUD_1M);
    }
    break;
      /*
       * params 0x05-0x07 and 0x00 : reserved
       *
       */
    default: {
      init_UART1(UART_BAUD_57600);
      print(T_DAUD_DEFAULT);
    }
    break;
  }

  /* This is REALLY important thing!
   * set pointer to cmdBuffer.
   * Almost all data for functions will
   * placed here from UART1 buffer.
   * Also reduce registers usage.
   */
  setBufferPointer_UART1(cmdBuffer.data);
}

void init_GPU(void)
{
  print(T_GPU_VERSION);
  print(T_SGPU_REGISRERS_VER "\n");

  init_sdCard();
  init_Sound();
  playNotes_Sound(startupSound);

  init_GPU_GPIO();
  init_GPU_UART();    // setup access to low level interface
  //init_DMA_memset();

  if (bsyInterfaceFlag) {
    print(T_BSY_STATE T_SOFT_BSY);
  } else {
    print(T_BSY_STATE T_HARD_BSY);
  }

  print(T_INIT T_OK);
}
