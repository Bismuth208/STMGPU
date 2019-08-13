#ifndef _FSMCDRV_H
#define _FSMCDRV_H

// ------------------------- FSMC GPIO ------------------------- //

// maybe i'll add something here in future...

// ------------------------------------------------------- //

#define FSMC_REG_ADDR  0x60000000
#define FSMC_DATA_ADDR 0x60080000  // FSMC_A18; PD13

#define LCD_REG      (*(__IO uint16_t*)(FSMC_REG_ADDR))
#define LCD_DATA     (*(__IO uint16_t*)(FSMC_DATA_ADDR))

// ------------------------------------------------------- //
#define DMA_FSMC_STREAM     DMA2_Stream7
#define DMA_FSMC_CH         DMA_Channel_0
#define DMA_FSMC_IQR        DMA2_Stream7_IRQn
#define DMA_FSMC_IRQ_HANLER DMA2_Stream7_IRQHandler
#define DMA_FSMC_RCC        RCC_AHB1Periph_DMA2

// ----------------------- FSMC Base func ---------------------- //

void init_FSMC(void);

void writeCommand_FSMC(uint16_t index);
void writeCommandData_FSMC(uint16_t data);
void writeCommandData32_FSMC(uint16_t data0, uint16_t data1);

//void sendData8_FSMC(uint8_t data);
//void sendData16_FSMC(uint16_t data);
void sendData32_FSMC(uint16_t data0, uint16_t data1);
void sendArr8_FSMC(void *data, uint32_t size);
void sendArr16_FSMC(void *data, uint32_t size);
void repeatData16_FSMC(uint16_t data, uint32_t size);

#define sendData8_FSMC(data) (LCD_DATA = (uint8_t)data)
#define sendData16_FSMC(data) (LCD_DATA = (uint16_t)data)

// -------------------------- FSMC DMA ------------------------- //
// not yet ready :(

/*
 * This one prevent GPIO from changes in GFX;
 * By check this flag in while(), it provide protection for sending data;
 * Protection what all data will send to TFT correctly!
 */
// #define wait_DMA_FSMC_busy() while(DMA2_Stream0->CR & (uint32_t)DMA_SxCR_EN)
//#define wait_DMA_FSMC_busy() while(DMA_GetCmdStatus(DMA2_Stream0) == ENABLE)
void wait_DMA_FSMC_busy(void);

void init_DMA_FSMC(void);
void sendData16_DMA_FSMC(void *data, uint32_t transferSize);
void sendData16_Fast_DMA_FSMC(void *data, uint16_t transferSize);
void repeatData16_DMA_FSMC(uint16_t color, uint32_t transferSize);
void setMemoryBaseAddr_DMA_FSMC(void *addr);

#endif /* _FSMCDRV_H */
