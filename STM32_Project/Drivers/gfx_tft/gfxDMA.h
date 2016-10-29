#ifndef GFXDMA_H
#define GFXDMA_H

#define wait_DMA1_SPI1_busy() while(READ_BIT(DMA1_Channel3->CCR, DMA_CCR1_EN))

#ifdef __cplusplus
extern "C"{
#endif
  
  void init_DMA1_SPI1(void);
  
  uint8_t DMA1_SPI1_busy(void);
  //void wait_DMA1_SPI1_busy(void);
  
  void fillColor_DMA1_SPI1(uint16_t color, uint32_t transferSize);
  void sendData16_DMA1_SPI1(void *data, uint32_t transferSize);
  
  
#ifdef __cplusplus
} // extern "C"
#endif

#endif /* GFXDMA_H */