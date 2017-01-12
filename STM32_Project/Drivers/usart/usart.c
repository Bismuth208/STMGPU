
#include <stm32f10x.h>

#include <string.h>
#include <stdlib.h>

#include <memHelper.h>
#include "usart.h"

// ------------------------------------------------------------ //

#define RX_AVALIABLE ( rx_buffer.head - rx_buffer.tail)

// ------------------------------------------------------------ //

ring_buffer_t rx_buffer = { 0, 0 };
uint8_t rxBuffer[SERIAL_BUFFER_SIZE] = { 0 };

// ------------------------------------------------------------ //

uint16_t dataAvailable_USART1(void)
{
  return RX_AVALIABLE;
}

uint8_t readData8_USART1(void)
{
  return rxBuffer[rx_buffer.tail++];
}

inline void cutData(uint8_t *pDest, uint16_t size)
{
  while(size) {
    while(!RX_AVALIABLE); // wait for something in buf
    
    *pDest = rxBuffer[rx_buffer.tail];
    ++rx_buffer.tail;
    ++pDest;
    --size;
  }
}

uint8_t waitCutByte_USART1(void)
{
#if 0
  while(!RX_AVALIABLE); // wait for something in buf
  
  uint8_t byteData = rxBuffer[rx_buffer.tail];
  ++rx_buffer.tail;
  
  return byteData;
#endif
  
  uint8_t byteData =0;
  
  cutData(&byteData, 1);
  
  return byteData;
}

uint16_t waitCutWord_USART1(void)
{
  uint16_t wordData =0;
  
  cutData((uint8_t*)&wordData, 2);
  
  return wordData;
}

void waitCutBuf_USART1(void *dest, uint16_t size)
{
  uint8_t *pDest = (uint8_t*)dest;
  uint16_t dataToRead = rx_buffer.tail + size; // check for overflow
  
  // cross zero point (overflow)?
  if(SERIAL_BUFFER_SIZE < dataToRead ) {
    if(size > 4) { // best case for memcpy32 is 4 byte align
      // have enough data?
      if(RX_AVALIABLE >= size) {
        // copy it
        memcpy32(pDest, &rxBuffer[rx_buffer.tail], size);
        rx_buffer.tail += size;
        return;
      }
    }
  } 
  
  // nope, nope, nope, no one condition fit to us
  cutData(pDest, size);
}

void fflush_USART1(void)
{
  rx_buffer.head = rx_buffer.tail = 0;
}

inline void sendData8_USART1(uint8_t data)
{
  while(!(USART1->SR & USART_SR_TC));
  USART1->DR = data;
}

inline void sendArrData8_USART1(void *src, uint32_t size)
{
  for(uint32_t count =0; count < size; count++) {
    sendData8_USART1(((uint8_t*)src)[count]);
  }
}
  
void init_UART1(uint32_t baud)
{
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_USART1);
  
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  
  // set PA10 as input UART (RxD)
  GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPD;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  // set PA9 as output UART (TxD)
  GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  
  USART_InitStruct.USART_BaudRate            = baud;
  USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits            = USART_StopBits_1;
  USART_InitStruct.USART_Parity              = USART_Parity_No ;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode                = (USART_Mode_Rx | USART_Mode_Tx);
  
  USART_Init(USART1, &USART_InitStruct);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART1, ENABLE);
  
  NVIC_InitTypeDef NVIC_InitStructure;
  //Enable USART IRQ Channel
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void USART1_IRQHandler(void)
{
  if(USART1->SR & USART_SR_RXNE ) {
    rxBuffer[rx_buffer.head++] = USART1->DR;
  }
}
