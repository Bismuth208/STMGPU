#ifndef _UART_H
#define _UART_H

// --------------------------------------------------//

#define SERIAL_BUFFER_SIZE 4096

// --------------------------------------------------//

#pragma pack(push, 1)
typedef struct {
  // just overflow, no check and additional code need
  uint16_t head         :12;
  uint16_t align1       :4;     // not used
  uint16_t tail         :12;
  uint16_t align2       :4;     // not used
} ring_buffer_t;
#pragma pack(pop)


typedef enum {
  UART_BAUD_9600 = 9600,
  UART_BAUD_57600 = 57600,
  UART_BAUD_115200 = 115200,
  UART_BAUD_1M = 1000000 
} baudSpeed_t;

// --------------------------------------------------//



void init_UART1(uint32_t baud);
void setBufferPointer_UART1(void *pBuf);
void sendData8_UART1(uint8_t data);
void fflush_UART1(void);
uint8_t readData8_UART1(void);
void sendArrData8_UART1(void *src, uint32_t size);
uint16_t dataAvailable_UART1(void);

uint8_t waitCutByte_UART1(void);
uint16_t waitCutWord_UART1(void);
void waitCutpBuf_UART1(uint16_t size);
void waitCutBuf_UART1(void *dest, uint16_t size);

//void waitCutPtrBuf_UART1(void *dest, uint16_t size);
// --------------------------------------------------//

#endif // _UART_H
