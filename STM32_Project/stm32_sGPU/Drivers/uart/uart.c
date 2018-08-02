
#include <stm32f10x.h>

#include <string.h>
#include <stdlib.h>

#include <memHelper.h>
#include "uart.h"

// ------------------------------------------------------------ //

#define RX_AVALIABLE ( rx_buffer.head - rx_buffer.tail)

#define MIN_CUT_DATA_SIZE   6

// ------------------------------------------------------------ //

ring_buffer_t rx_buffer = { 0, 0 };
uint8_t rxBuffer[SERIAL_BUFFER_SIZE] = { 0 };

uint8_t *pDestBuf =0;

// ------------------------------------------------------------ //
// apply pointer to end buffer
void setBufferPointer_UART1(void *pBuf)
{
  pDestBuf = (uint8_t*)pBuf;  
}

uint16_t dataAvailable_UART1(void)
{
  return RX_AVALIABLE;
}

uint8_t readData8_UART1(void)
{
  return rxBuffer[rx_buffer.tail++];
}

void cutData(uint8_t *pDest, uint16_t size)
{
  while(size--) {
    while(!RX_AVALIABLE); // wait for something in buf
    *pDest++ = rxBuffer[rx_buffer.tail++];
  }
}

uint8_t waitCutByte_UART1(void)
{
  while(!RX_AVALIABLE); // wait for something in buf
  return rxBuffer[rx_buffer.tail++];
}

uint16_t waitCutWord_UART1(void)
{
  uint16_t wordData =0;
  cutData((uint8_t*)&wordData, 2);
  return wordData;
}

void waitCutBuf_UART1(void *dest, uint16_t size)
{
  if((RX_AVALIABLE >= size) && (size >= MIN_CUT_DATA_SIZE)) {
    if(rx_buffer.tail < ((SERIAL_BUFFER_SIZE-1) - size)) {
      memcpy32(dest, &rxBuffer[rx_buffer.tail], size);
      rx_buffer.tail += size;
      return;
    }
  }
  cutData(dest, size);
}

// this one use preinited pointer to end data buffer
void waitCutpBuf_UART1(uint16_t size)
{
  if((RX_AVALIABLE >= size) && (size >= MIN_CUT_DATA_SIZE)) {
    if(rx_buffer.tail < ((SERIAL_BUFFER_SIZE-1) - size)) {
      memcpy32(pDestBuf, &rxBuffer[rx_buffer.tail], size);
      rx_buffer.tail += size;
      return;
    }
  }
  cutData(pDestBuf, size);
}

// in theory returning pointer is faster than memcpy32
//void waitCutPtrBuf_UART1(void *dest, uint16_t size)
//{
//  if((RX_AVALIABLE >= size) && (size >= MIN_CUT_DATA_SIZE)) {
//      if(rx_buffer.tail < ((SERIAL_BUFFER_SIZE-1) - size)) {
//	  uint16_t pos = rx_buffer.tail;
//	  rx_buffer.tail += size;
//	  dest = &rxBuffer[pos];
//      }
//  }
//  // not enough data or near to end of buffer
//  cutData(dest, size);
//}

void fflush_UART1(void)
{
  rx_buffer.head = rx_buffer.tail = 0;
}

inline void sendData8_UART1(uint8_t data)
{
  while(!(USART1->SR & USART_SR_TC));
  USART1->DR = data;
}

inline void sendArrData8_UART1(void *src, uint32_t size)
{
  for(uint32_t count =0; count < size; count++) {
    sendData8_UART1(((uint8_t*)src)[count]);
  }
}
  
void init_UART1(uint32_t baud)
{
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_USART1);
  
  GPIO_InitTypeDef GPIO_InitStruct;
  
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
  
  // setup UART1
  USART_InitTypeDef USART_InitStruct;
  USART_InitStruct.USART_BaudRate            = baud;
  USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits            = USART_StopBits_1;
  USART_InitStruct.USART_Parity              = USART_Parity_No ;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode                = (USART_Mode_Rx | USART_Mode_Tx);
  
  USART_Init(USART1, &USART_InitStruct);        // apply settings
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);// eneble IRQ
  USART_Cmd(USART1, ENABLE);                    // no comment
  
  //setup NVIC for USART IRQ Channel
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

// get data as fast as possible!
void USART1_IRQHandler(void)
{
  //if(USART1->SR & USART_SR_RXNE ) {
    rxBuffer[rx_buffer.head++] = USART1->DR;
//  }
}
