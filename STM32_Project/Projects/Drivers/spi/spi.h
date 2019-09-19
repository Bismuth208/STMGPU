#ifndef _SPI_H
#define _SPI_H

// ------------------------- SPI_1 ------------------------- //
//sck - pa5; miso - pa6; mosi - pa7;
#define GPIO_SPI_LCD            GPIOA
#define GPIO_Pin_SPI_LCD_SCK    GPIO_Pin_5      // SCK
#define GPIO_Pin_SPI_LCD_MISO   GPIO_Pin_6     // DO
#define GPIO_Pin_SPI_LCD_MOSI   GPIO_Pin_7     // DI
 
// wait untill all data will be sent
#define WAIT_FREE_TX    while ((SPI1->SR & SPI_I2S_FLAG_TXE) == (uint16_t)RESET);
#define WAIT_FOR_BSY    while ((SPI1->SR & SPI_I2S_FLAG_BSY) != (uint16_t)RESET);

// ------------------------------------------------------- //

// ------------------------- SPI_1 ------------------------- //
void init_SPI1(void);
void sendData8_SPI1(uint32_t ulData);
void sendArr8_SPI1(void *data, uint32_t size);
void sendData16_SPI1(uint32_t ulData);
void sendData32_SPI1(uint32_t ulData0, uint32_t ulData1);
#if 0
void sendArr16_SPI1(void *data, uint32_t size);
void repeatData16_SPI1(uint16_t data, uint32_t size);
#endif

// --------------------- DMA1_SPI_1 ------------------------ //

/*
 * This one prevent GPIO from changes in GFX;
 * By check this flag in while(), it provide protection for sending data;
 * Protection what all data will send to TFT correctly!
 */
#define wait_DMA1_SPI1_busy() while(READ_BIT(DMA1_Channel3->CCR, DMA_CCR1_EN))


void init_DMA1_SPI1(void);

void sendData16_DMA1_SPI1(void *data, uint32_t transferSize);
void sendData16_Fast_DMA1_SPI1(void *data, uint32_t transferSize);

void repeatData16_DMA1_SPI1(uint32_t color, uint32_t transferSize);
void repeatData16_Fast_DMA1_SPI1(uint16_t color, uint32_t transferSize);

void setMemoryBaseAddr_DMA1_SPI1(void *addr);


#endif /* _SPI_H */
