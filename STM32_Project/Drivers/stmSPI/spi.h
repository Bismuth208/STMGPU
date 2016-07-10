#pragma once
#ifndef SPI_H
#define SPI_H

#include <stdint.h>

#if defined (__AVR__)

#define SET_SCK_LOW     PORTB &= ~ (1 << PB5);
#define SET_SCK_HI      PORTB |= (1 << PB5);

#define SET_MOSI_HI     PORTB |= (1 << PB3);
/*
 PB2 - D10; SS
 PB3 - D11; MOSI
 PB4 - D12; MISO
 PB5 - D13; SCK
 */
#else

// ------------------------- SPI_1 ------------------------- //
//sck - pa5; miso - pa6; mosi - pa7;
 
#define SCK_PIN GPIO_Pin_5      // SCK
#define MIS0_PIN GPIO_Pin_6     // DO
#define MOSI_PIN GPIO_Pin_7     // DI

// remap
//sck - pb3; miso - pb4; mosi - pb5;   
/*
#define SCK_PIN GPIO_Pin_3      // SCK
#define MIS0_PIN GPIO_Pin_4     // DO
#define MOSI_PIN GPIO_Pin_5     // DI   
*/

#define SET_SCK_HI      (GPIOA->BSRR = SCK_PIN);
#define SET_SCK_LOW     (GPIOA->BRR = SCK_PIN);

#define SET_MOSI_HI     (GPIOA->BSRR = MOSI_PIN);
   
// ждем пока данные передадутся до конца
#define WAIT_FREE_TX    while ((SPI1->SR & SPI_I2S_FLAG_TXE) == (uint16_t)RESET);
#define WAIT_FOR_BSY    while ((SPI1->SR & SPI_I2S_FLAG_BSY) != (uint16_t)RESET);

// ------------------------- SPI_2 ------------------------- //
// mosi - pb15; miso - pb14;
// sck - pb13; nss - pb12;
#define NSS_2_PIN GPIO_Pin_12
#define SCK_2_PIN GPIO_Pin_13
#define MIS0_2_PIN GPIO_Pin_14
#define MOSI_2_PIN GPIO_Pin_15


#endif // __AVR__

#ifdef __cplusplus
extern "C"{
#endif
  

// ------------------------------------------------------- //

// ------------------------- SPI_1 ------------------------- //
void init_SPI1(void);
//void beginTrSPI(uint8_t spcr, uint8_t spsr);
void sendData8_SPI1(uint8_t data);
void sendData16_SPI1(uint16_t data);
void sendData32_SPI1(uint16_t data0, uint16_t data1);
void sendArrSPI(void *buf, uint16_t count);
//void endTrSPI(void);

// ------------------------- SPI_2 ------------------------- //
void init_SPI2(void);
    
#ifdef __cplusplus
} // extern "C"
#endif

#endif /* SPI_H */
