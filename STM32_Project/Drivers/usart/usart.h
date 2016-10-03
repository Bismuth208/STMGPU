#ifndef _USART_H
#define _USART_H

// --------------------------------------------------//

#define SERIAL_BUFFER_SIZE 4096     //3072 2048

// --------------------------------------------------//
/*
typedef struct {
  uint8_t buffer[SERIAL_BUFFER_SIZE];
  uint16_t head;
  uint16_t tail;
} ring_buffer_t;
*/

// #pragma pack(push, 1)
typedef struct {
  // just overflow, no check and additional code need
  uint16_t head         :12;
  uint16_t align1       :4;     // not used
  uint16_t tail         :12;
  uint16_t align2       :4;     // not used
} ring_buffer_t;
// #pragma pack(pop)

/*
typedef struct {
  uint16_t head;
  uint16_t tail;
} ring_buffer_t;
*/

typedef enum {
  USART_BAUD_9600 = 9600,
  USART_BAUD_57600 = 57600,
  USART_BAUD_115200 = 115200,
  USART_BAUD_1M = 1000000 
} baudSpeed_t;

// --------------------------------------------------//

uint8_t init_UART1(uint32_t baud);
void sendData8_USART1(uint8_t data);
void fflush_USART1(void);
uint8_t readData8_USART1(void);
void sendArrData8_USART1(void *src, uint32_t size);
uint16_t dataAvailable_USART1(void);

void waitForData_USART1(uint16_t size);
void cutFromBuf_USART1(void *dest, uint16_t size);
void waitCutBuf_USART1(void *dest, uint16_t size);

// --------------------------------------------------//

#endif // _USART_H