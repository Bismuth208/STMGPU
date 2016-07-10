#ifndef _AVRSPI_H
#define _AVRSPI_H

#include <stdint.h>

#if defined (__AVR__)

#define SET_XCK_HI      PORTD |= (1 << PD4);

#define SET_SCK_LOW     PORTB &= ~ (1 << PB5);
#define SET_SCK_HI      PORTB |= (1 << PB5);

#define SET_MOSI_HI     PORTB |= (1 << PB3);

#define SET_SS_HI     PORTB |= (1 << PB2);
/*
 PB2 - D10; SS
 PB3 - D11; MOSI
 PB4 - D12; MISO
 PB5 - D13; SCK
 */
#endif

#ifdef __cplusplus
extern "C"{
#endif

void initSPI(void);
void initUSARTSPI(void);
void beginTrSPI(uint8_t spcr, uint8_t spsr);
uint8_t sendReadDataSPI(uint8_t data);
uint16_t sendReadWordDataSPI(uint16_t data);
void sendData8_SPI1(uint8_t data);
void sendData16_SPI1(uint16_t data);
void sendReadArrSPI(void *buf, uint16_t count);
void sendArrSPI(uint8_t *buf, uint16_t size);
void endTrSPI(void);
  
//void sendDataUSPI(uint8_t data);
void sendWordDataUSPI(uint16_t data);
    
#ifdef __cplusplus
} // extern "C"
#endif

#endif
