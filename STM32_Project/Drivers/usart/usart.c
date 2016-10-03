
#include <stm32f10x.h>

#include <string.h>
#include <stdlib.h>

#include "usart.h"

#pragma diag_suppress=Pa082 // disable volatile warnings

// ------------------------------------------------------------ //

#define RX_AVALIABLE ( rx_buffer.head - rx_buffer.tail)
//#define RX_AVALIABLE ((SERIAL_BUFFER_SIZE + rx_buffer.head - rx_buffer.tail) % SERIAL_BUFFER_SIZE)

// ------------------------------------------------------------ //

static volatile ring_buffer_t rx_buffer = { 0, 0 };
//static volatile uint8_t rxBuffer[SERIAL_BUFFER_SIZE] = { 0 };
static volatile uint8_t *rxBuffer;

// ------------------------------------------------------------ //

uint16_t dataAvailable_USART1(void)  //return size of avalible bytes in buffer
{
  return RX_AVALIABLE;
}

void waitForData_USART1(uint16_t size)
{
  while(RX_AVALIABLE <= size);
}

uint8_t readData8_USART1(void)
{
  if (rx_buffer.head != rx_buffer.tail) {
    uint8_t rx_tmpByte = rxBuffer[rx_buffer.tail];
    ++rx_buffer.tail;
    
    //rx_buffer.buffer[rx_buffer.tail] = 0x00;  // optional
    //rx_buffer.tail = (rx_buffer.tail + 1) % SERIAL_BUFFER_SIZE;
    return rx_tmpByte;
  } else return 0;
}

void cutFromBuf_USART1(void *dest, uint16_t size)
{
  for(uint8_t count=0; count < size; count++) {
    ((uint8_t*)dest)[count] = rxBuffer[rx_buffer.tail];
    //rx_buffer.tail = (rx_buffer.tail + 1) % SERIAL_BUFFER_SIZE;
    ++rx_buffer.tail;
  }
}

void waitCutBuf_USART1(void *dest, uint16_t size)
{
  uint8_t count=0;
  
  while(size) {
    
    while(!RX_AVALIABLE); // wait for something in buf
    
    ((uint8_t*)dest)[count] = rxBuffer[rx_buffer.tail];
    
    //rxBuffer[rx_buffer.tail] = 0x00;  // optional
    //rx_buffer.tail = (rx_buffer.tail + 1) % SERIAL_BUFFER_SIZE;
    ++rx_buffer.tail;
    
    --size;
    ++count;
  }
}

void fflush_USART1(void)
{
  //memset(rx_buffer.buffer, 0x00, SERIAL_BUFFER_SIZE);
  rx_buffer.head = 0;
  rx_buffer.tail = 0;
}

void sendData8_USART1(uint8_t data)
{
  while(!(USART1->SR & USART_SR_TC));
  USART1->DR = data;
}

void sendArrData8_USART1(void *src, uint32_t size)
{
  for(uint32_t count =0; count < size; count++) {
    sendData8_USART1(((uint8_t*)src)[count]);
  }
}
  
uint8_t init_UART1(uint32_t baud)
{
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);
  
  GPIO_InitTypeDef GPIO_InitStruct;
  
  // set PA10 as input UART (RxD)
  GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPD;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  // set PA9 as output UART (TxD)
  GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  uint8_t initStatus =0;
  rxBuffer = malloc(SERIAL_BUFFER_SIZE * sizeof(uint8_t));
  
  if(rxBuffer == NULL) {
    return 0;
  } else {
    initStatus = 1;
  }
  
  USART_InitTypeDef USART_InitStruct;
  
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
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  return initStatus;
}

void USART1_IRQHandler(void)
{
  if(USART1->SR & USART_SR_RXNE ) { // (USART_GetITStatus(USART1, USART_IT_RXNE)

    //uint16_t tmp = (rx_buffer.head + 1) % SERIAL_BUFFER_SIZE;
    
    //if (tmp != rx_buffer.tail) {
      //rx_buffer.buffer[rx_buffer.head] = USART1->DR;
      rxBuffer[rx_buffer.head] = USART1->DR;
      //rx_buffer.head = (rx_buffer.head + 1) % SERIAL_BUFFER_SIZE;
      ++rx_buffer.head;
    //}
      
    //USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  }
}

#pragma diag_default=Pa082
