#ifndef _UART_H
#define _UART_H

// --------------------------------------------------//

#ifdef STM32F10X_MD // if STM32F103C8T6 20k RAM
#define SERIAL_BUFFER_SIZE 4096
#define SERIAL_BUFFER_BITS   12
#endif

#ifdef STM32F10X_HD // if STM32F103VET6 64k RAM
#define SERIAL_BUFFER_SIZE 4096
#define SERIAL_BUFFER_BITS   12
#endif

#ifdef STM32F40XX // if STM32F407VET6 128k RAM
#define SERIAL_BUFFER_SIZE 8192
#define SERIAL_BUFFER_BITS   13
#endif

#define SERIAL_BUFFER_SIZE_MASK (SERIAL_BUFFER_SIZE-1)
// --------------------------------------------------//

#define USART1_USE_PROTOCOL_V0
//#define USART1_USE_PROTOCOL_V1 // DMA Rx feature was added

// --------------------------------------------------//

#pragma pack(push, 1)
typedef struct
{
  uint32_t head;
  uint32_t tail;
} ring_buffer_t;
#pragma pack(pop)

typedef enum
{
  UART_BAUD_9600 = 9600,
  UART_BAUD_57600 = 57600,
  UART_BAUD_115200 = 115200,
  UART_BAUD_1M = 1000000
} baudSpeed_t;

// --------------------------------------------------//

#if USART1_USE_PROTOCOL_V1
uint8_t ucSearchSyncSeq(void);
void fflush_buffer_USART1(void);

void vCallbackFrom_USART1_IRQ(void);
#endif

// --------------------------------------------------//

void init_UART1(uint32_t baud);
void setBufferPointer_UART1(void *pBuf);
void sendData8_UART1(uint8_t data);
void fflush_UART1(void);
uint8_t readData8_UART1(void);
void sendArrData8_UART1(void *src, uint32_t size);
uint32_t dataAvailable_UART1(void);

uint8_t waitCutByte_UART1(void);
uint32_t waitCutWord_UART1(void);
void waitCutpBuf_UART1(uint32_t size);
void waitCutBuf_UART1(void *dest, uint32_t size);

//void waitCutPtrBuf_UART1(void *dest, uint16_t size);
void *waitCutPtrBuf_UART1(uint32_t size);
// --------------------------------------------------//

#endif // _UART_H
