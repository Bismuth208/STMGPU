/*
*
* Created by: Antonov Alexandr (Bismuth208)
*
*/

#include <string.h>
#include <stm32f10x.h>

#include <gfx.h>
#include <usart.h>
#include <memHelper.h>

#include "gpuMain.h"

//===========================================================================//

// Make sync whith CPU by read from USART buffer 
// sync sequence: 0x42DD
void sync_CPU(void)
{
  print(T_WAIT_SYNC);
  
  bool syncEstablished = false;
  
  while(!syncEstablished) {
    if(dataAvailable_USART1() >= 2) {
      if(readData8_USART1() == 0x42) {
        if(readData8_USART1() == 0xDD) {
          
          fflush_USART1();
          syncEstablished = true;
          sendData8_USART1(SYNC_OK);  // sequence right, answer to CPU
          
          print(T_OK T_TFT_SIZE);
          
          // return screen size to CPU
          cmdBuffer.par1 = width();
          cmdBuffer.par2 = height();
          
          sendArrData8_USART1(cmdBuffer.data, 4);
          
          print(T_OK T_GPU_START);
        }
      }
    }
  }
  
  // clear output
  fillScreen(COLOR_BLACK);
}

void init_GPU_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;         // Mode: output "Push-Pull"
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;        // Set speed
  GPIO_InitStruct.GPIO_Pin = GPU_BSY_PIN;
  GPIO_Init(GPU_BSY_PORT, &GPIO_InitStruct);            // Apply settings
  
  
  GPIO_InitStruct.GPIO_Pin = GPU_BSY_LED_PIN;           // LED BSY PIN
  GPIO_Init(GPU_BSY_LED_PORT, &GPIO_InitStruct);        // Apply settings
  
  GPIO_SET_PIN(GPU_BSY_LED_PORT, GPU_BSY_LED_PIN);      // turn off
  
  // Now init baud rate selection GPIO
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;         // Mode: input "Pull-down"
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;        // Set speed
  GPIO_InitStruct.GPIO_Pin = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2);
  GPIO_Init(GPIOA, &GPIO_InitStruct);            // Apply settings
}

void init_GPU_USART(void)
{
  print(T_SELECT_WAY T_USART_WAY);
  print(T_BAUD_SPEED);

  switch(GPIOA->IDR & 0x07) // get GPIO state
  {
  case 0x01: {
    init_UART1(USART_BAUD_9600);
    print(T_BAUD_9600);
  } break;
  
  case 0x02: {
    init_UART1(USART_BAUD_57600);
    print(T_BAUD_57K);
  } break;
  
  case 0x03: {
    init_UART1(USART_BAUD_115200);
    print(T_BAUD_115K);
  } break;
  
  case 0x04: {
    init_UART1(USART_BAUD_1M);
    print(T_BAUD_1M);
  } break;
  /* 
  * params 0x05-0x07 : reserved
  *
  */
  default: {
    init_UART1(USART_BAUD_57600);
    print(T_DAUD_DEFAULT);
  } break;
  }
}

void init_GPU(void)
{
  print(T_GPU_VERSION);
  
  init_GPU_GPIO();
  init_GPU_USART();    // setup access to low level interface
  //init_DMA_memset();
  
  if(bsyState){
    print(T_BSY_STATE T_SOFT_BSY);
  } else {
    print(T_BSY_STATE T_HARD_BSY);
  }
 
  print(T_INIT T_OK);
}
