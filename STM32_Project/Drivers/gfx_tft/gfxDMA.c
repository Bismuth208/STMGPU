/*
 * TODO:
 * - move all what use std periph lib in another section
 * - make a cleanup! this is terrible! so much garabage in code!
 */

#include "gfx.h"

#include <stm32f10x.h>
#include <stm32f10x_dma.h>

// DMA can`t send more than 65535 bytes
#define MAX_DMA_REQUEST 0xFFFF
#define CCR_CLEAR_Mask  ((uint32_t)0xFFFF800F)

__IO uint32_t pixels_left = 0;
__IO uint16_t dataBuffer;                   // for single storage
//__IO uint8_t DMA1_SP1_transfer_busy = 0;    // busy flag

__IO uint16_t CR1_backup = 0;
__IO uint16_t CR1_backup_16b_PS2 = 0;

//----------------------------- DMA SECTION ----------------------------------//
void init_DMA1_SPI1(void)
{
  RCC->AHBENR |= RCC_AHBPeriph_DMA1;    // enable clocks to DMA1

  uint32_t tmpreg = ( DMA_DIR_PeripheralDST |   // .DMA_DIR
            DMA_Mode_Normal |                   // .DMA_Mode
            DMA_PeripheralInc_Disable |         // .DMA_PeripheralInc
            DMA_MemoryInc_Disable |             // .DMA_MemoryInc
            DMA_PeripheralDataSize_HalfWord |   // .DMA_PeripheralDataSize
            DMA_MemoryDataSize_HalfWord |       // .DMA_MemoryDataSize
            DMA_Priority_VeryHigh |             // .DMA_Priority
            DMA_M2M_Disable |                   // .DMA_M2M
            DMA_IT_TC );                        // enable interupt
                 
  /* Clear MEM2MEM, PL, MSIZE, PSIZE, MINC, PINC, CIRC and DIR bits */
  MODIFY_REG(DMA1_Channel3->CCR, CCR_CLEAR_Mask, tmpreg); // apply new settings
  
  DMA1_Channel3->CPAR = (uint32_t)&(SPI1->DR);  // set DMA_PeripheralBaseAddr
  
  SPI1->CR2 |= SPI_I2S_DMAReq_Tx;               // enable DMA IRQ on Tx SPI1
  
  
  CR1_backup = SPI1->CR1;       // save current settings
  
  SPI1->CR1 |= SPI_DataSize_16b;        // set dataSize halfword (16 bit)
  /* DMA works propertly with higier speed,
  * that is why we can use max speed.
  * If F_CPU = 72, then SPI = 36 mbit/s
  * (according reference manual)
  */
  CLEAR_BIT(SPI1->CR1, SPI_BaudRatePrescaler_2); // set SPI_BaudRatePrescaler_2
  
  CR1_backup_16b_PS2 = SPI1->CR1; // save DMA settings
  SPI1->CR1 = CR1_backup;       // restore
  
  NVIC_InitTypeDef NVIC_InitStructure;
  //Enable DMA1 channel IRQ Channel
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void fillColor_DMA1_SPI1(uint16_t color, uint32_t transferSize)
{  
  CR1_backup = SPI1->CR1;               //  \__ only 4 asm commands
  SPI1->CR1 = CR1_backup_16b_PS2;       // /
  
  dataBuffer = color;   // store data or, you can lost it
 
  DMA1_Channel3->CMAR = (uint32_t) &dataBuffer;         // apply DMA_MemoryBaseAddr
  CLEAR_BIT(DMA1_Channel3->CCR, DMA_MemoryInc_Enable);  // disable memory increment
  
  if(transferSize > MAX_DMA_REQUEST) {
    DMA1_Channel3->CNDTR = MAX_DMA_REQUEST;        // DMA_SetCurrDataCounter
    pixels_left = (transferSize - MAX_DMA_REQUEST);
      
  } else {
    DMA1_Channel3->CNDTR = transferSize;        // set how much data to transfer
    pixels_left = 0;
  }
  
  //DMA1_SP1_transfer_busy = 1; // set flag what DMA is busy
    
  DMA1_Channel3->CCR |= DMA_CCR1_EN;          // shot DMA to transer;
}

/*
 * DO NOT SEND 'IN FUNCTION BUFFERS'!
 * Or small buffers!
 * if STM execute func to fast, then buffer created in func
 * will exist anymore, and DMA will send garbage!
 * As you know vars created in func exist while in func!
 * Be carefull!
 */
void sendData16_DMA1_SPI1(void *data, uint32_t transferSize)
{
  CR1_backup = SPI1->CR1;
  SPI1->CR1 = CR1_backup_16b_PS2;
    
  DMA1_Channel3->CMAR = (uint32_t) data;    // apply DMA_MemoryBaseAddr
  DMA1_Channel3->CCR |= (uint16_t)(DMA_MemoryInc_Enable);       // enable memory increment
  
  if(transferSize > MAX_DMA_REQUEST) {
    DMA1_Channel3->CNDTR = MAX_DMA_REQUEST;        // DMA_SetCurrDataCounter
    pixels_left = (transferSize - MAX_DMA_REQUEST);
      
  } else {
    DMA1_Channel3->CNDTR = transferSize;        // set how much data to transfer
    pixels_left = 0;
  }
  
  //DMA1_SP1_transfer_busy = 1; // set flag what DMA is busy
  
  DMA1_Channel3->CCR |= DMA_CCR1_EN;          // shot DMA to transer;
}

/*
 * This func prevent GPIO changes in GFX;
 * By check this flag in while(), it provide protection for sending data;
 * Protection what all data will send to TFT correctly!
 */
uint8_t DMA1_SPI1_busy(void)
{
  //return DMA1_SP1_transfer_busy;
  return READ_BIT(DMA1_Channel3->CCR, DMA_CCR1_EN);
}

void wait_DMA1_SPI1_busy(void)
{
  while(READ_BIT(DMA1_Channel3->CCR, DMA_CCR1_EN));
}

void DMA1_Channel3_IRQHandler(void) 
{
  if ((DMA1->ISR & DMA1_IT_TC3) ) {    // is it our IRQ? // != (uint32_t)RESET
    
    CLEAR_BIT(DMA1_Channel3->CCR, DMA_CCR1_EN);
    
    if (pixels_left > 0) {                      // all pixels transfered?
      
      if (pixels_left > MAX_DMA_REQUEST) {      // left something?
        DMA1_Channel3->CNDTR = MAX_DMA_REQUEST; // DMA_SetCurrDataCounter
        pixels_left -= MAX_DMA_REQUEST;
      } else {                                  // nope, this is last shoot
        DMA1_Channel3->CNDTR = pixels_left;     // DMA_SetCurrDataCounter
        pixels_left = 0;
      }
      
      DMA1_Channel3->CCR |= DMA_CCR1_EN;          // shot DMA to transer;
    
    } else {                                    // nope, end of transmission
      // disable memory increment
      // disable DMA1_Channel3
      CLEAR_BIT(DMA1_Channel3->CCR, (DMA_MemoryInc_Enable | DMA_CCR1_EN));
      
      //DMA1_SP1_transfer_busy = 0;               // set 'free' flag
                  
      SET_TFT_CS_HI;                            // freedom TFT CS
      SPI1->CR1 = CR1_backup; // set back dataSize byte (8 bit);
    }
    
    DMA1->IFCR = DMA1_IT_TC3;   // clear interrupt
  }
}

//----------------------------------------------------------------------------//


//---------------------------------- Code dump... ---------------------------------------------//
#if 0
DMA_InitTypeDef  DMA_InitStructure;

//----------------------------- DMA SECTION ----------------------------------//
void spi1_init_DMA(void)
{
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  DMA_InitTypeDef  DMA_InitStructure;
  
  DMA_StructInit(&DMA_InitStructure);   // apply default params
  
  //DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer;
  //DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  //DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable; //;
  //DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  //DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
     
  //tx
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(SPI1->DR);
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord  ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord ;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  
  DMA_InitStructure.DMA_BufferSize = 1; // set 1 or get assert failed
  
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);  // apply new DMA params
  
  DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
  
  SPI1->CR2 |= SPI_I2S_DMAReq_Tx;             // SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
  
  
  NVIC_InitTypeDef NVIC_InitStructure;
  //Enable DMA1 channel IRQ Channel */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void init_DMA1_SPI1(void)
{
  RCC->AHBENR |= RCC_AHBPeriph_DMA1;    // enable clocks to DMA1

  uint32_t tmpreg = DMA1_Channel3->CCR;
  /* Clear MEM2MEM, PL, MSIZE, PSIZE, MINC, PINC, CIRC and DIR bits */
  tmpreg &= CCR_CLEAR_Mask;

  tmpreg = ( DMA_DIR_PeripheralDST |             // .DMA_DIR
            DMA_Mode_Normal |                   // .DMA_Mode
            DMA_PeripheralInc_Disable |         // .DMA_PeripheralInc
            DMA_MemoryInc_Disable |             // .DMA_MemoryInc
            DMA_PeripheralDataSize_HalfWord |   // .DMA_PeripheralDataSize
            DMA_MemoryDataSize_HalfWord |       // .DMA_MemoryDataSize
            DMA_Priority_High |                 // .DMA_Priority
            DMA_M2M_Disable |                   // .DMA_M2M
            DMA_IT_TC );                          // enable interupt

  DMA1_Channel3->CCR = tmpreg;                  // apply new settings
  DMA1_Channel3->CPAR = (uint32_t)&(SPI1->DR);  // set DMA_PeripheralBaseAddr
  
  SPI1->CR2 |= SPI_I2S_DMAReq_Tx;               // enable DMA IRQ on Tx SPI1
  
  NVIC_InitTypeDef NVIC_InitStructure;
  //Enable DMA1 channel IRQ Channel */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void sendColorDMA(uint32_t transferSize, uint16_t color)
{  
  //SPI_DataSizeConfig(SPI1, SPI_DataSize_16b);
  SPI1->CR1 |= SPI_DataSize_16b;
  
  //if(transferSize < 0x10000) dma_transfer = transferSize;
  //else  dma_transfer = MAX_DMA_REQUEST;
  
  dataBuffer = color;
 
  DMA1_Channel3->CMAR = (uint32_t) &dataBuffer;    // DMA_InitStructure.DMA_MemoryBaseAddr  = (uint32_t) &color;
  //DMA1_Channel3->CNDTR = dma_transfer;        // DMA_InitStructure.DMA_BufferSize = dma_transfer;
  
  // DMA_Init(DMA1_Channel3, &DMA_InitStructure);  // apply new DMA params
  
  //SPI1->CR2 |= SPI_I2S_DMAReq_Tx;             // SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx,ENABLE);
  
  //do {
    if(transferSize > MAX_DMA_REQUEST) {
      DMA1_Channel3->CNDTR = MAX_DMA_REQUEST;        // DMA_SetCurrDataCounter(DMA1_Channel3, dma_transfer);
      pixels_left = (transferSize - MAX_DMA_REQUEST);
      //transferSize -= MAX_DMA_REQUEST;
      
    } else {
      DMA1_Channel3->CNDTR = transferSize;        // DMA_SetCurrDataCounter(DMA1_Channel3, dma_transfer);
      //transferSize = 0;
      pixels_left = 0;
    }
    
    spi_transfer = 1;
    
    //DMA1_Channel3->CCR |= DMA_IT_TC;            // DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
    DMA1_Channel3->CCR |= DMA_CCR1_EN;          // DMA_Cmd(DMA1_Channel3, ENABLE);
    
    // wait for end of transfer
    //while(((uint16_t)(DMA1_Channel3->CNDTR))!= 0);    //while(DMA_GetCurrDataCounter(DMA1_Channel3) != 0);
    
    //DMA1_Channel3->CCR &= (uint16_t)(~DMA_CCR1_EN);   // DMA_Cmd(DMA1_Channel3, DISABLE);

  //} while(transferSize > 0);
  
  //SET_TFT_CS_HI;        //RELEASE_TFT();
    
  //SPI_DataSizeConfig(SPI1, SPI_DataSize_8b);
  //SPI1->CR1 &= (uint16_t)~SPI_DataSize_16b;
  //SPI1->CR1 |= SPI_DataSize_8b;
}

void sendData16DMA(uint32_t transferSize, void *data)
{
  //SPI_DataSizeConfig(SPI1, SPI_DataSize_16b);
  SPI1->CR1 |= SPI_DataSize_16b;
  
  //p_buffer = (uint32_t*)data;
    
  DMA1_Channel3->CMAR  = (uint32_t) data;
  DMA1_Channel3->CCR |= (uint16_t)(DMA_MemoryInc_Enable);     //DMA_InitStructure.DMA_MemoryInc  = DMA_MemoryInc_Enable;
    
  //DMA_Init(DMA1_Channel3, &DMA_InitStructure);  // init the DMA
    
  // start DMA
  
  spi_transfer = 1;
  pixels_left = 0;
      
  DMA1_Channel3->CNDTR = (uint16_t)transferSize;    // DMA_SetCurrDataCounter(DMA1_Channel3, dma_transfer);
  //SPI1->CR2 |= SPI_I2S_DMAReq_Tx;       // SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx,ENABLE);
  
  //DMA1_Channel3->CCR |= DMA_IT_TC;      // DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
  DMA1_Channel3->CCR |= DMA_CCR1_EN;    // DMA_Cmd(DMA1_Channel3, ENABLE);
      
  // wait for end of transfer
  //while(((uint16_t)(DMA1_Channel3->CNDTR))!= 0);    //while(DMA_GetCurrDataCounter(DMA1_Channel3) != 0);
    
  //DMA1_Channel3->CCR &= (uint16_t)(~DMA_CCR1_EN);   // DMA_Cmd(DMA1_Channel3, DISABLE);
    
  //SET_TFT_CS_HI;
    
  //SPI_DataSizeConfig(SPI1, SPI_DataSize_8b);
  //SPI1->CR1 &= (uint16_t)~SPI_DataSize_16b;
  //SPI1->CR1 |= SPI_DataSize_8b;
    
  //DMA1_Channel3->CCR &= (uint16_t)(~DMA_MemoryInc_Enable); //DMA_InitStructure.DMA_MemoryInc  = DMA_MemoryInc_Disable;
}

uint8_t dma_busy(void)
{
  return spi_transfer;
}

//tx
void DMA1_Channel3_IRQHandler(void) 
{
  // if(DMA_GetITStatus(DMA1_IT_TC3)) {
  if ((DMA1->ISR & DMA1_IT_TC3) != (uint32_t)RESET) {    // is it our IRQ?
    
    //spi_transfer = 0;
    
    DMA1_Channel3->CCR &= (uint16_t)(~DMA_CCR1_EN);   // DMA_Cmd(DMA1_Channel3, DISABLE);
    
    if (pixels_left > 0) {
      //p_buffer += MAX_DMA_REQUEST;
      //spi1_init_dma(p_buffer, pixels_left);
      //pixels_left = pixels_left - MAX_DMA_REQUEST;
      
      //DMA1_Channel3->CCR &= (uint16_t)(~DMA_CCR1_EN);   // DMA_Cmd(DMA1_Channel3, DISABLE);
      
      if (pixels_left > MAX_DMA_REQUEST) {
        DMA1_Channel3->CNDTR = MAX_DMA_REQUEST;        // DMA_SetCurrDataCounter(DMA1_Channel3, dma_transfer);
        pixels_left -= MAX_DMA_REQUEST;
      } else {
        DMA1_Channel3->CNDTR = pixels_left;        // DMA_SetCurrDataCounter(DMA1_Channel3, dma_transfer);
        pixels_left = 0;
      }
      
      //spi_transfer = 1;
      DMA1_Channel3->CCR |= DMA_CCR1_EN;          // DMA_Cmd(DMA1_Channel3, ENABLE);
    
    } else {
      spi_transfer = 0;
      
      SET_TFT_CS_HI;
      //DMA1_Channel3->CCR &= (uint16_t)(~DMA_CCR1_EN);   // DMA_Cmd(DMA1_Channel3, DISABLE);
      //DMA1_Channel3->CCR &= ~DMA_IT_TC;                    // DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, DISABLE);
      
      DMA1_Channel3->CCR &= (uint16_t)(~DMA_MemoryInc_Enable); // DMA_InitStructure.DMA_MemoryInc  = DMA_MemoryInc_Disable;
            
      //SPI_DataSizeConfig(SPI1, SPI_DataSize_8b);
      SPI1->CR1 &= (uint16_t)~SPI_DataSize_16b;
      //SPI1->CR1 |= SPI_DataSize_8b;
    }
    
    /* Clear the selected DMAy interrupt pending bits */
    DMA1->IFCR = DMA1_IT_GL3;   // DMA_ClearITPendingBit(DMA1_IT_GL3);
  }
}
#endif
