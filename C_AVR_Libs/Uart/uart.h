#ifndef UART_H
#define UART_H
//WinAVR
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>


#define UART_TXREADY UDRE0
#define UART_RXREADY RXC0


#define SERIAL_BUFFER_SIZE 64
//#define F_CPU 16000000L
#define F_CPU 8000000L
#define UART_CALC_BAUDRATE(baudRate) ((uint32_t)((F_CPU) + ((uint32_t)baudRate * 4UL)) / ((uint32_t)(baudRate) * 8UL) - 1)

typedef struct {
	uint8_t buffer[SERIAL_BUFFER_SIZE];
  uint8_t head;
  uint8_t tail;
} ringBuffer_t;

#ifdef __cplusplus
extern "C" {
#endif

//**************UART PROTOTYPES******************************
//void uartSetup(void);
void uartSetup(uint32_t baud);
void uartSendByte(uint8_t);
void uartSendArray(uint8_t *txArray, uint8_t txArraySize);
void uartSendArrayP(uint8_t *txArray);
uint8_t serialRead(void);
uint8_t serialAvailable(void);
void serialClear(void);
  
#ifdef __cplusplus
}
#endif


#endif	//UART_H
