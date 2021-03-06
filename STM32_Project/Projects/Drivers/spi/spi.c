#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef STM32F40XX
#include <stm32f4xx.h>
#else
#include <stm32f10x.h>
#endif /* STM32F40XX */

#include "spi.h"

//---------------------------------------------------------------------------------------------//

#define USE_CALC_REG_SPI1 1


#define CR1_CLEAR_Mask       ((uint16_t)0x3040)
#define SPI_Mode_Select      ((uint16_t)0xF7FF)
#define CR1_SPE_Set          ((uint16_t)0x0040)


// DMA can`t send more than 65535 bytes
#define MAX_DMA_REQUEST 0xFFFF
#define CCR_CLEAR_Mask  ((uint32_t)0xFFFF800F)

__IO uint32_t ulDMADataLeft = 0;
__IO uint32_t ulDataBuffer;                   // for single storage

#if USE_CALC_REG_SPI1
#define CR1_backup_16b_PS0      0xcb44
#define CR1_backup_8b_PS2       0xc344

#define DMA1_Channel3_CCR_BASE        0x3512
#define DMA1_Channel3_CCR_NO_INC_EN   0x3513 // disable increment and shoot DMA
#define DMA1_Channel3_CCR_EN_INC_EN   0x3593 // enable increment and shoot DMA

#else
__IO uint16_t CR1_backup_16b_PS0 = 0;
__IO uint16_t CR1_backup_8b_PS2 = 0;
#endif

//--------------------------------------- SPI_1 ------------------------------------------------//
void vInit_SPI1(void)
{
#if !USE_CALC_REG_SPI1
  uint16_t tmpreg = (uint16_t)((uint32_t)SPI_Direction_1Line_Tx |       // .SPI_Direction
                        SPI_Mode_Master |                       // .SPI_Mode
                        SPI_DataSize_8b |                       // .SPI_DataSize
                        SPI_CPOL_Low |                          // .SPI_CPOL
                        SPI_CPHA_1Edge |                        // .SPI_CPHA
                        SPI_NSS_Soft |                          // .SPI_NSS
                        SPI_BaudRatePrescaler_2 |               // .SPI_BaudRatePrescaler
                        SPI_FirstBit_MSB |                      // .SPI_FirstBit
                        CR1_SPE_Set |                           // Enable SPI
                        SPI_NSSInternalSoft_Set  );             // 
  /*
  * Only Tx
  * Master mode
  * transfer size 8 bit
  * Polarity
  * Clock phase of signal
  * NSS by controled by soft
  * SCK devider = 2
  * First bit sended MSB
  */
  
  /* Clear BIDIMode, BIDIOE, RxONLY, SSM, SSI, LSBFirst, BR, MSTR, CPOL and CPHA bits */
  /* Write to SPIx CR1 */
  MODIFY_REG(SPI1->CR1, CR1_CLEAR_Mask, tmpreg);
#else
  SPI1->CR1 = CR1_backup_8b_PS2; // Use precalculated Value
#endif
  
  /* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
  SPI1->I2SCFGR &= SPI_Mode_Select;
}

__attribute__((optimize("O2"))) void sendData8_SPI1(uint32_t ulData)
{
  SPI1->DR = (uint8_t) ulData;
  WAIT_FOR_BSY;
}

__attribute__((optimize("O2"))) void sendData16_SPI1(uint32_t ulData)
{
  // this faster, than change data size...
  SPI1->DR = (((uint16_t) ulData)>>8);
  WAIT_FREE_TX;
  SPI1->DR = ((uint16_t) ulData);
  WAIT_FOR_BSY;
}

__attribute__((optimize("O2"))) void sendData32_SPI1(uint32_t ulData0, uint32_t ulData1)
{
  SPI1->CR1 = CR1_backup_16b_PS0;
  SPI1->DR = (uint16_t) ulData0;
  WAIT_FREE_TX;
  SPI1->DR = (uint16_t) ulData1;

  WAIT_FOR_BSY;
  SPI1->CR1 = CR1_backup_8b_PS2;
}

#if 0
void sendArr8_SPI1(void *data, uint32_t size)
{
  for(uint32_t count =0; count < size; count++) {
    WAIT_FREE_TX;
    SPI1->DR = ((uint8_t*)data)[count];
  }
  
  WAIT_FOR_BSY;
}

void sendArr16_SPI1(void *data, uint32_t size)
{
  SET_BIT(SPI1->CR1, SPI_DataSize_16b);
  
  for(uint32_t count =0; count < size; count++) {
    WAIT_FREE_TX;
    SPI1->DR = ((uint16_t*)data)[count];
  }
  
  WAIT_FOR_BSY;
  CLEAR_BIT(SPI1->CR1, SPI_DataSize_16b);       // set 8bit size
}

void repeatData16_SPI1(uint16_t data, uint32_t size)
{
  SET_BIT(SPI1->CR1, SPI_DataSize_16b);
  
  while(size) {
    WAIT_FREE_TX;
    SPI1->DR = data;
    --size;
  }

  WAIT_FOR_BSY;
  CLEAR_BIT(SPI1->CR1, SPI_DataSize_16b);       // set 8bit size
}
#endif

//----------------------------- DMA SECTION ----------------------------------//
void vInit_DMA1_SPI1(void)
{
#if !USE_CALC_REG_SPI1
  uint32_t tmpreg = ( DMA_DIR_PeripheralDST |   // .DMA_DIR
            DMA_Mode_Normal |                   // .DMA_Mode
            DMA_PeripheralInc_Disable |         // .DMA_PeripheralInc
            DMA_MemoryInc_Disable |             // .DMA_MemoryInc
            DMA_PeripheralDataSize_HalfWord |   // .DMA_PeripheralDataSize
            DMA_MemoryDataSize_HalfWord |       // .DMA_MemoryDataSize
            DMA_Priority_VeryHigh |             // .DMA_Priority
            DMA_M2M_Disable |                   // .DMA_M2M
            DMA_IT_TC );                        // enable interrupt
                 
  /* Clear MEM2MEM, PL, MSIZE, PSIZE, MINC, PINC, CIRC and DIR bits */
  MODIFY_REG(DMA1_Channel3->CCR, CCR_CLEAR_Mask, tmpreg); // apply new settings
#else
  DMA1_Channel3->CCR = DMA1_Channel3_CCR_BASE;
#endif
  
  DMA1_Channel3->CPAR = (uint32_t)&(SPI1->DR);  // set DMA_PeripheralBaseAddr
  
  SPI1->CR2 |= SPI_I2S_DMAReq_Tx;               // enable DMA IRQ on Tx SPI1
#if !USE_CALC_REG_SPI1
  CR1_backup_8b_PS2 = SPI1->CR1;       // save current settings
  SPI1->CR1 |= SPI_DataSize_16b;        // set dataSize halfword (16 bit)
  /* DMA works properly with higher speed,
  * that is why we can use max speed.
  * If F_CPU = 72, then SPI = 36 Mbit/s
  * (according reference manual)
  */
  CLEAR_BIT(SPI1->CR1, SPI_BaudRatePrescaler_2); // set SPI_BaudRatePrescaler_2
  
  CR1_backup_16b_PS0 = SPI1->CR1; // save DMA settings
  SPI1->CR1 = CR1_backup_8b_PS2;  // restore
#endif
  
  // Enable DMA1 channel IRQ Channel
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void setMemoryBaseAddr_DMA1_SPI1(void *addr)
{
  DMA1_Channel3->CMAR = (uint32_t) addr;    // apply DMA_MemoryBaseAddr
}

__attribute__((optimize("O2"))) void repeatData16_DMA1_SPI1(uint32_t color, uint32_t transferSize)
{
  SPI1->CR1 = CR1_backup_16b_PS0;

  ulDataBuffer = color;   // store data or, you can lost it

  DMA1_Channel3->CMAR = (uint32_t) &(ulDataBuffer);         // apply DMA_MemoryBaseAddr
#if !USE_CALC_REG_SPI1
  CLEAR_BIT(DMA1_Channel3->CCR, DMA_MemoryInc_Enable);  // disable memory increment
#endif

  if(transferSize > MAX_DMA_REQUEST) {
    DMA1_Channel3->CNDTR = MAX_DMA_REQUEST;        // DMA_SetCurrDataCounter
    ulDMADataLeft = (transferSize - MAX_DMA_REQUEST);
  } else {
    DMA1_Channel3->CNDTR = transferSize;        // set how much data to transfer
  }

#if !USE_CALC_REG_SPI1
  DMA1_Channel3->CCR |= DMA_CCR1_EN;          // shot DMA to transfer;
#else
  DMA1_Channel3->CCR = DMA1_Channel3_CCR_NO_INC_EN;
#endif
}

#if 0
/*
 * No check for transfer data size
 */
__attribute__((optimize("O2"))) void repeatData16_Fast_DMA1_SPI1(uint16_t color, uint32_t transferSize)
{
  SPI1->CR1 = CR1_backup_16b_PS0;

  ulDataBuffer = (uint32_t) color;   // store data or, you can lost it
  DMA1_Channel3->CMAR = (uint32_t) &(ulDataBuffer);         // apply DMA_MemoryBaseAddr
  DMA1_Channel3->CNDTR = transferSize;        // set how much data to transfer
  DMA1_Channel3->CCR = DMA1_Channel3_CCR_NO_INC_EN;
}
#endif

/*
 * DO NOT SEND 'IN FUNCTION BUFFERS'!
 * Or small buffers!
 * if STM execute func to fast, then buffer created in func
 * will exist anymore, and DMA will send garbage!
 * As you know vars created in func exist while in func!
 * Be carefull!
 */
__attribute__((optimize("O2"))) void sendData16_DMA1_SPI1(void *data, uint32_t transferSize)
{
  SPI1->CR1 = CR1_backup_16b_PS0;
    
  DMA1_Channel3->CMAR = (uint32_t) data;    // apply DMA_MemoryBaseAddr
  
  if(transferSize > MAX_DMA_REQUEST) {
    DMA1_Channel3->CNDTR = MAX_DMA_REQUEST;        // DMA_SetCurrDataCounter
    ulDMADataLeft = (transferSize - MAX_DMA_REQUEST);
      
  } else {
    DMA1_Channel3->CNDTR = transferSize;        // set how much data to transfer
    //data_left = 0;
  }
  
  // shot DMA to transfer; enable memory increment
#if !USE_CALC_REG_SPI1
  DMA1_Channel3->CCR |= (uint16_t)(DMA_CCR1_EN | DMA_MemoryInc_Enable);
#else
  DMA1_Channel3->CCR = DMA1_Channel3_CCR_EN_INC_EN;
#endif
}

/*
 * No check for transfer data size
 */
__attribute__((optimize("O2"))) void sendData16_Fast_DMA1_SPI1(void *data, uint32_t transferSize)
{
  SPI1->CR1 = CR1_backup_16b_PS0;
    
  DMA1_Channel3->CMAR = (uint32_t) data;    // apply DMA_MemoryBaseAddr
  DMA1_Channel3->CNDTR = transferSize;      // set how much data to transfer

  // shot DMA to transfer; enable memory increment
#if !USE_CALC_REG_SPI1
  DMA1_Channel3->CCR |= (uint16_t)(DMA_CCR1_EN | DMA_MemoryInc_Enable);
#else
  DMA1_Channel3->CCR = DMA1_Channel3_CCR_EN_INC_EN;
#endif
}

__attribute__((optimize("O2"))) void DMA1_Channel3_IRQHandler(void)
{
  if((DMA1->ISR & DMA1_IT_TC3)) { // is it our IRQ? // != (uint32_t)RESET
    DMA1->IFCR = DMA1_IT_TC3;   // clear interrupt
    
    if(ulDMADataLeft) {               // all pixels transfered?
      CLEAR_BIT(DMA1_Channel3->CCR, DMA_CCR1_EN);
      
#if 0 // this is useless condition in most cases
      if(data_left > MAX_DMA_REQUEST) {         // left something?
        DMA1_Channel3->CNDTR = MAX_DMA_REQUEST; // DMA_SetCurrDataCounter
        data_left -= MAX_DMA_REQUEST;
      } else {                                 // nope, this is last shoot
        DMA1_Channel3->CNDTR = data_left;       // DMA_SetCurrDataCounter
        data_left = 0;
      }
#else
      DMA1_Channel3->CNDTR = ulDMADataLeft;       // DMA_SetCurrDataCounter
      ulDMADataLeft = 0;
#endif
      DMA1_Channel3->CCR |= DMA_CCR1_EN;        // shot DMA to transfer;
    
    } else {                                   // nope, end of transmission
      // disable: DMA1_Channel3, memory increment
      CLEAR_BIT(DMA1_Channel3->CCR, (DMA_CCR1_EN | DMA_MemoryInc_Enable));
      SPI1->CR1 = CR1_backup_8b_PS2; // set back dataSize byte (8 bit);
    }
  }
}
//----------------------------------------------------------------------------//
