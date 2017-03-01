#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <stm32f10x.h>

#include "spi.h"

//---------------------------------------------------------------------------------------------//

#define CR1_CLEAR_Mask       ((uint16_t)0x3040)
#define SPI_Mode_Select      ((uint16_t)0xF7FF)
#define CR1_SPE_Set          ((uint16_t)0x0040)


// DMA can`t send more than 65535 bytes
#define MAX_DMA_REQUEST 0xFFFF
#define CCR_CLEAR_Mask  ((uint32_t)0xFFFF800F)

__IO uint32_t data_left = 0;
__IO uint16_t dataBuffer;                   // for single storage

__IO uint16_t CR1_backup = 0;
__IO uint16_t CR1_backup_16b_PS2 = 0;

//---------------------------------------------------------------------------------------------//

void init_GPIO_SPI1(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //disable JTAG, SW left enabled
  //GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;      // Alternate fun mode
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_SPI_LCD_MOSI | GPIO_Pin_SPI_LCD_SCK ;
  GPIO_Init(GPIO_SPI_LCD, &GPIO_InitStruct);        // apply settings
}

//--------------------------------------- SPI_1 ------------------------------------------------//
void init_SPI1(void)
{
  // Enable clock for SPI1
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN);
  
  init_GPIO_SPI1();
  
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
  
  /* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
  SPI1->I2SCFGR &= SPI_Mode_Select;
}

inline void sendData8_SPI1(uint8_t data)
{
  SPI1->DR = data;
  
  WAIT_FREE_TX;
  WAIT_FOR_BSY;
  //WAIT_FOR_END;
}

inline void sendData16_SPI1(uint16_t data)
{
  // this faster, than change data size...
  SPI1->DR = (data>>8);
  
  WAIT_FREE_TX;
  SPI1->DR = (data);
  
  WAIT_FREE_TX;
  WAIT_FOR_BSY;
  //WAIT_FOR_END;
}

inline void sendData32_SPI1(uint16_t data0, uint16_t data1)
{
  SET_BIT(SPI1->CR1, SPI_DataSize_16b);
  SPI1->DR = data0;
  
  WAIT_FREE_TX;
  SPI1->DR = data1;

  WAIT_FREE_TX;
  WAIT_FOR_BSY;
  //WAIT_FOR_END;
  CLEAR_BIT(SPI1->CR1, SPI_DataSize_16b);
}

void sendArr8_SPI1(void *data, uint32_t size)
{
  for(uint32_t count =0; count < size; count++) {
    WAIT_FREE_TX;
    SPI1->DR = ((uint8_t*)data)[count];
  }
  
  WAIT_FREE_TX;
  WAIT_FOR_BSY;
}

void sendArr16_SPI1(void *data, uint32_t size)
{
  SET_BIT(SPI1->CR1, SPI_DataSize_16b);
  
  for(uint32_t count =0; count < size; count++) {
    WAIT_FREE_TX;
    SPI1->DR = ((uint16_t*)data)[count];
  }
  
  WAIT_FREE_TX;
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
  
  WAIT_FREE_TX;
  WAIT_FOR_BSY;
  CLEAR_BIT(SPI1->CR1, SPI_DataSize_16b);       // set 8bit size
}


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
  
  // Enable DMA1 channel IRQ Channel
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void setMemoryBaseAddr_DMA1_SPI1(void *addr)
{
  DMA1_Channel3->CMAR = (uint32_t) addr;    // apply DMA_MemoryBaseAddr
}

void repeatData16_DMA1_SPI1(uint16_t color, uint32_t transferSize)
{  
  CR1_backup = SPI1->CR1;               //  \__ only 4 asm commands
  SPI1->CR1 = CR1_backup_16b_PS2;       // /
  
  dataBuffer = color;   // store data or, you can lost it
 
  DMA1_Channel3->CMAR = (uint32_t) &dataBuffer;         // apply DMA_MemoryBaseAddr
  CLEAR_BIT(DMA1_Channel3->CCR, DMA_MemoryInc_Enable);  // disable memory increment
  
  if(transferSize > MAX_DMA_REQUEST) {
    DMA1_Channel3->CNDTR = MAX_DMA_REQUEST;        // DMA_SetCurrDataCounter
    data_left = (transferSize - MAX_DMA_REQUEST);
  } else {
    DMA1_Channel3->CNDTR = transferSize;        // set how much data to transfer
  }
    
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
  
  if(transferSize > MAX_DMA_REQUEST) {
    DMA1_Channel3->CNDTR = MAX_DMA_REQUEST;        // DMA_SetCurrDataCounter
    data_left = (transferSize - MAX_DMA_REQUEST);
      
  } else {
    DMA1_Channel3->CNDTR = transferSize;        // set how much data to transfer
    //data_left = 0;
  }
  
  // shot DMA to transer; enable memory increment
  DMA1_Channel3->CCR |= (uint16_t)(DMA_CCR1_EN | DMA_MemoryInc_Enable);
}

/*
 * No check for transfer data size
 */
#if 1
void sendData16_Fast_DMA1_SPI1(void *data, uint16_t transferSize)
{
  CR1_backup = SPI1->CR1;
  SPI1->CR1 = CR1_backup_16b_PS2;
    
  DMA1_Channel3->CMAR = (uint32_t) data;    // apply DMA_MemoryBaseAddr
  DMA1_Channel3->CNDTR = transferSize;      // set how much data to transfer

  // shot DMA to transer; enable memory increment
  DMA1_Channel3->CCR |= (uint16_t)(DMA_CCR1_EN | DMA_MemoryInc_Enable);
}
#endif

void DMA1_Channel3_IRQHandler(void) 
{
  if((DMA1->ISR & DMA1_IT_TC3)) { // is it our IRQ? // != (uint32_t)RESET
    if(data_left) {               // all pixels transfered?
      
      CLEAR_BIT(DMA1_Channel3->CCR, DMA_CCR1_EN);
      
      if(data_left > MAX_DMA_REQUEST) {         // left something?
        DMA1_Channel3->CNDTR = MAX_DMA_REQUEST; // DMA_SetCurrDataCounter
        data_left -= MAX_DMA_REQUEST;
      } else {                                 // nope, this is last shoot
        DMA1_Channel3->CNDTR = data_left;       // DMA_SetCurrDataCounter
        data_left = 0;
      }
      DMA1_Channel3->CCR |= DMA_CCR1_EN;        // shot DMA to transer;
    
    } else {                                   // nope, end of transmission
      // disable: DMA1_Channel3, memory increment
      CLEAR_BIT(DMA1_Channel3->CCR, (DMA_CCR1_EN | DMA_MemoryInc_Enable));
      SPI1->CR1 = CR1_backup; // set back dataSize byte (8 bit);
    }
    DMA1->IFCR = DMA1_IT_TC3;   // clear interrupt
  }
}
//----------------------------------------------------------------------------//
