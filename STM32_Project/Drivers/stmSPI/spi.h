#ifndef SPI_H
#define SPI_H

#include <stdint.h>

// ------------------------- SPI_1 ------------------------- //
//sck - pa5; miso - pa6; mosi - pa7;
#define GPIO_SPI_LCD            GPIOA
#define GPIO_Pin_SPI_LCD_SCK    GPIO_Pin_5      // SCK
#define GPIO_Pin_SPI_LCD_MISO   GPIO_Pin_6     // DO
#define GPIO_Pin_SPI_LCD_MOSI   GPIO_Pin_7     // DI
 
// wait untill all data will be sent
#define WAIT_FREE_TX    while ((SPI1->SR & SPI_I2S_FLAG_TXE) == (uint16_t)RESET);
#define WAIT_FOR_BSY    while ((SPI1->SR & SPI_I2S_FLAG_BSY) != (uint16_t)RESET);

//#define WAIT_FOR_END    while((SPI1->SR & (SPI_I2S_FLAG_BSY | SPI_I2S_FLAG_TXE )) == 0x80)
// ------------------------------------------------------- //

// ------------------------- SPI_1 ------------------------- //
void init_SPI1(void);
void sendData8_SPI1(uint8_t data);
void sendArr8_SPI1(void *data, uint32_t size);
void sendData16_SPI1(uint16_t data);
void sendData32_SPI1(uint16_t data0, uint16_t data1);
void sendArr16_SPI1(void *data, uint32_t size);
void repeatData16_SPI1(uint16_t data, uint32_t size);

// ------------------------- SPI_2 ------------------------- //
void init_SPI2(void);

#endif /* SPI_H */
