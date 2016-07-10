#pragma once

#ifndef GFXDMA_H
#define GFXDMA_H

#ifdef __cplusplus
extern "C"{
#endif
  
void init_DMA1_SPI1(void);

uint8_t DMA1_SPI1_busy(void);
void wait_DMA1_SPI1_busy(void);

void fillColor_DMA1_SPI1(uint32_t transferSize, uint16_t color);
void sendData16_DMA1_SPI1(uint32_t transferSize, void *data);


#ifdef __cplusplus
} // extern "C"
#endif

#endif /* GFXDMA_H */