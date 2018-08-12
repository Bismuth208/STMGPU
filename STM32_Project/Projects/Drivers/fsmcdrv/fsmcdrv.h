#ifndef _FSMCDRV_H
#define _FSMCDRV_H

// ------------------------- FSMC GPIO ------------------------- //

// maybe i'll add something here in future...


// ----------------------- FSMC Base func ---------------------- //

void init_FSMC(void);
void sendData8_FSMC(uint8_t data);
void sendArr8_FSMC(void *data, uint32_t size);
void sendData16_FSMC(uint16_t data);
void sendData32_FSMC(uint16_t data0, uint16_t data1);
void sendArr16_FSMC(void *data, uint32_t size);
void repeatData16_FSMC(uint16_t data, uint32_t size);


void writeCommandData_FSMC(uint16_t data);
void writeCommand_FSMC(uint16_t index);

// -------------------------- FSMC DMA ------------------------- //
// not yet ready :(

/*
 * This one prevent GPIO from changes in GFX;
 * By check this flag in while(), it provide protection for sending data;
 * Protection what all data will send to TFT correctly!
 */
// #define wait_DMA_FSMC_busy() while(READ_BIT(DMAX_ChannelY->CCR, DMA_CCR1_EN))
#define wait_DMA_FSMC_busy() while(DMA_GetCmdStatus(DMA1_Stream0) != (uint32_t)RESET)


void init_DMA_FSMC(void);
void sendData16_DMA_FSMC(void *data, uint32_t transferSize);
void sendData16_Fast_DMA_FSMC(void *data, uint16_t transferSize);
void repeatData16_DMA_FSMC(uint16_t color, uint32_t transferSize);
void setMemoryBaseAddr_DMA_FSMC(void *addr);

#endif /* _FSMCDRV_H */
