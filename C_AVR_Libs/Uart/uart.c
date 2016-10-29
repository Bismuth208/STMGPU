#include <avr/pgmspace.h>
#include <string.h> //for memset

#include "uart.h"

static volatile ringBuffer_t rx_buffer  =  { { 0 }, 0, 0 };
//ring_buffer tx_buffer  =  { { 0 }, 0, 0 };

void uartSetup(uint32_t baud)
{
  //cli();  //denie global interupt
  
  UBRR0L = (UART_CALC_BAUDRATE(baud) & 0xFF);
  UBRR0H = (UART_CALC_BAUDRATE(baud) >>8) & 0xFF;
  
  UCSR0A = 1<<U2X0;
  UCSR0B = 1<<RXCIE0 | 1<<RXEN0 | 1<<TXEN0;  //разр. прерыв при приеме, разр приема, разр передачи.
  UCSR0C = 1<<UCSZ01 | 1<<UCSZ00;  //размер слова 8 разрядов
  
  sei();  //enable global interupt
}
  
uint8_t serialAvailable(void)  //return size of avalible bytes in uart buffer
{
  return (SERIAL_BUFFER_SIZE + rx_buffer.head - rx_buffer.tail) % SERIAL_BUFFER_SIZE;
}

uint8_t serialRead(void)
{
  if (rx_buffer.head == rx_buffer.tail) {
    return -1;
  } else {
    uint8_t rx_tmpByte = rx_buffer.buffer[rx_buffer.tail];
    rx_buffer.tail = (rx_buffer.tail + 1) % SERIAL_BUFFER_SIZE;
    return rx_tmpByte;
  }
}

void serialClear(void)
{
  //memset(rx_buffer.buffer, 0x00, SERIAL_BUFFER_SIZE);
  rx_buffer.head = 0;
  rx_buffer.tail = 0;
}
#if 0
void byteStore(uint8_t tmpByte, ringBuffer_t *buffer)
{
  uint8_t i = (buffer->head + 1) % SERIAL_BUFFER_SIZE;

  if (i != buffer->tail) {
    buffer->buffer[buffer->head] = tmpByte;
    buffer->head = i;
  }
}
#endif

ISR(USART_RX_vect)
{
  uint8_t rxByte;
  if (bit_is_clear(UCSR0A, UPE0)) {
    rxByte = UDR0;
    
    uint8_t i = (rx_buffer.head + 1) % SERIAL_BUFFER_SIZE;
    
    if (i != rx_buffer.tail) {
      rx_buffer.buffer[rx_buffer.head] = rxByte;
      rx_buffer.head = i;
    }
    
  } else rxByte = UDR0;
}

void uartSendByte(uint8_t txByte)
{
  while (!(UCSR0A & (1<<UART_TXREADY)));
  UDR0 = txByte;
}

void uartSendArrayP(uint8_t *txArray)
{
  while (pgm_read_byte(txArray)!='\0')
  {
    uartSendByte(pgm_read_byte(txArray));
    ++txArray;
  }
}

void uartSendArray(uint8_t *txArray, uint8_t txArraySize)
{
  for(uint8_t count = 0; count < txArraySize; ++count){
    while (!(UCSR0A & (1<<UART_TXREADY)));
    UDR0 = txArray[count];
  }
}
