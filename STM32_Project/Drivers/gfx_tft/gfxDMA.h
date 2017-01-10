#ifndef GFXDMA_H
#define GFXDMA_H

/*
 * This one prevent GPIO from changes in GFX;
 * By check this flag in while(), it provide protection for sending data;
 * Protection what all data will send to TFT correctly!
 */
#define wait_DMA1_SPI1_busy() while(READ_BIT(DMA1_Channel3->CCR, DMA_CCR1_EN))


void init_DMA1_SPI1(void);

void fillColor_DMA1_SPI1(uint16_t color, uint32_t transferSize);
void sendData16_DMA1_SPI1(void *data, uint32_t transferSize);
void sendData16_Fast_DMA1_SPI1(void *data, uint16_t transferSize);

void setMemoryBaseAddr_DMA1_SPI1(void *addr);

#endif /* GFXDMA_H */