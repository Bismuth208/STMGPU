#pragma once

#ifndef GFXDMA_H
#define GFXDMA_H

#ifdef __cplusplus
extern "C"{
#endif
  
void init_DMA1_SPI1(void);

uint8_t DMA1_SPI1_busy(void);
void wait_DMA1_SPI1_busy(void);

void fillColor_DMA1_SPI1(uint16_t color, uint32_t transferSize);
void sendData16_DMA1_SPI1(void *data, uint32_t transferSize);


#ifdef __cplusplus
} // extern "C"
#endif

#endif /* GFXDMA_H */