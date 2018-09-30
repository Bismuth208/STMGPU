#ifdef STM32F40XX
#include <stm32f4xx.h>
#else
#include <stm32f10x.h>
#endif /* STM32F40XX */

#include "fsmcdrv.h"

#if defined(STM32F40XX) || defined(STM32F10X_HD)
// ------------------------------------------------------- //

// DMA can`t send more than 65535 bytes
#define MAX_DMA_REQUEST 0xFFFF

__IO uint32_t data_left = 0;
__IO uint16_t dataBuffer;                   // for single storage
// ------------------------------------------------------- //

void initFSMC_GPIO(void)
{
  GPIO_InitTypeDef gpio;

  // FSMC_D4, FSMC_D5, FSMC_D6, FSMC_D7, FSMC_D8, FSMC_D9, FSMC_D10, FSMC_D11, FSMC_D12
  gpio.GPIO_Mode = GPIO_Mode_AF;
  gpio.GPIO_OType = GPIO_OType_PP;
  gpio.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpio.GPIO_Speed = GPIO_High_Speed;
  GPIO_Init(GPIOE, &gpio);

  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);

  // FSMC_D2, FSMC_D3, FSMC_NOE, FSMC_NWE, FSMC_NE1, FSMC_D13, FSMC_D14, FSMC_D15, FSMC_A18, FSMC_D0, FSMC_D1
  gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOD, &gpio);

  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

  // LCD_BL
  // gpio.GPIO_Mode = GPIO_Mode_OUT;
  // gpio.GPIO_Pin = GPIO_Pin_1;
  // GPIO_Init(GPIOB, &gpio);
}

// NOR_PSRAM_Mode
void initFSMC_Module(void)
{
  FSMC_NORSRAMTimingInitTypeDef fsmcTimings;
  fsmcTimings.FSMC_AccessMode = FSMC_AccessMode_A;
  fsmcTimings.FSMC_AddressHoldTime = 0x01;  // n/a for SRAM mode A
  fsmcTimings.FSMC_AddressSetupTime = 0x00;
  fsmcTimings.FSMC_BusTurnAroundDuration = 0x00;
  fsmcTimings.FSMC_CLKDivision = 0x02;  // n/a for SRAM mode A
  fsmcTimings.FSMC_DataLatency = 0x02;  // n/a for SRAM mode A
  fsmcTimings.FSMC_DataSetupTime = 0x01;

  FSMC_NORSRAMInitTypeDef fsmcInitType;
  fsmcInitType.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  fsmcInitType.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  fsmcInitType.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Enable;
  fsmcInitType.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  fsmcInitType.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  fsmcInitType.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  fsmcInitType.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  fsmcInitType.FSMC_ReadWriteTimingStruct = &fsmcTimings;
  fsmcInitType.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  fsmcInitType.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  fsmcInitType.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  fsmcInitType.FSMC_WrapMode = FSMC_WrapMode_Disable;
  fsmcInitType.FSMC_WriteBurst = FSMC_WriteBurst_Enable;
  fsmcInitType.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  fsmcInitType.FSMC_WriteTimingStruct = &fsmcTimings;

  FSMC_NORSRAMInit(&fsmcInitType);
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

void init_FSMC(void)
{
  initFSMC_GPIO();
  initFSMC_Module();
}

// ------------------------------------------------------- //

__inline void writeCommand_FSMC(uint16_t index)
{
  LCD_REG = index;
}

__inline void writeCommandData_FSMC(uint16_t data)
{
  LCD_DATA = data>>8;
  LCD_DATA = data&0xff;
}

//__inline void sendData8_FSMC(uint8_t data)
//{
//  LCD_DATA = data;
//}

//__inline void sendData16_FSMC(uint16_t data)
//{
//  LCD_DATA = data;
//}

__inline void sendData32_FSMC(uint16_t data0, uint16_t data1)
{
	LCD_DATA = data0>>8;
    LCD_DATA = data0&0xff;
    LCD_DATA = data1>>8;
	LCD_DATA = data1&0xff;
}

void sendArr8_FSMC(void *data, uint32_t size)
{
  uint8_t *pData = (uint8_t*)data;
  while (size--) {
    LCD_DATA = *pData++;
  }
}


void sendArr16_FSMC(void *data, uint32_t size)
{
  uint16_t *pData = (uint16_t*)data;
  while (size--) {
    LCD_DATA = *pData++;
  }
}

void repeatData16_FSMC(uint16_t data, uint32_t size)
{
  while (size--) {
    LCD_DATA = data;
  }
}


// ------------------------------------------------------- //
void init_DMA_FSMC(void)
{
  RCC_AHB1PeriphResetCmd(DMA_FSMC_RCC, ENABLE);

  DMA_InitTypeDef DMA_FSMC_settings;
  DMA_FSMC_settings.DMA_BufferSize = 0;
  DMA_FSMC_settings.DMA_Channel = DMA_FSMC_CH;
  DMA_FSMC_settings.DMA_DIR = DMA_DIR_MemoryToPeripheral; //DMA_DIR_MemoryToMemory;
  DMA_FSMC_settings.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_FSMC_settings.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_FSMC_settings.DMA_Memory0BaseAddr = 0;
  DMA_FSMC_settings.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_FSMC_settings.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_FSMC_settings.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_FSMC_settings.DMA_Mode = DMA_Mode_Normal;
  DMA_FSMC_settings.DMA_PeripheralBaseAddr = (uint32_t)&LCD_DATA;
  DMA_FSMC_settings.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_FSMC_settings.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_FSMC_settings.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_FSMC_settings.DMA_Priority = DMA_Priority_High;

  DMA_ITConfig(DMA_FSMC_STREAM, DMA_IT_TC, ENABLE);
  DMA_Init(DMA_FSMC_STREAM, &DMA_FSMC_settings);

  // Enable DMA1 channel IRQ Channel
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = DMA_FSMC_IQR;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_EnableIRQ(DMA_FSMC_IQR);
}

// There is no way to change that flag fast, except slow DMA_Init()
// So, i add new one function with syntax like SPL API
void DMA_MemoryIncrementConfig(DMA_Stream_TypeDef* DMAy_Streamx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_DMA_ALL_PERIPH(DMAy_Streamx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState == ENABLE) {
      DMAy_Streamx->CR |= (uint32_t)DMA_SxCR_MINC;  //DMA_MemoryInc_Enable;
  } else {
      DMAy_Streamx->CR &= ~(uint32_t)DMA_SxCR_MINC; //DMA_MemoryInc_Disable;
  }
}

void setMemoryBaseAddr_DMA_FSMC(void *addr)
{
  DMA_MemoryTargetConfig(DMA_FSMC_STREAM, (uint32_t) addr, DMA_Memory_0);
}

void repeatData16_DMA_FSMC(uint16_t color, uint32_t transferSize)
{
  dataBuffer = color;   // store data or, you can lost it

  if(transferSize > MAX_DMA_REQUEST) {
      DMA_SetCurrDataCounter(DMA_FSMC_STREAM, MAX_DMA_REQUEST); // DMA_SetCurrDataCounter
      data_left = (transferSize - MAX_DMA_REQUEST);
  } else {
      DMA_SetCurrDataCounter(DMA_FSMC_STREAM, transferSize);   // set how much data to transfer
  }

  // apply DMA_MemoryBaseAddr
  setMemoryBaseAddr_DMA_FSMC((void*)&dataBuffer);
  DMA_MemoryIncrementConfig(DMA_FSMC_STREAM, DISABLE);
  DMA_Cmd(DMA_FSMC_STREAM, ENABLE); // shot DMA to transfer;
}

/*
 * DO NOT SEND 'IN FUNCTION BUFFERS'!
 * Or small buffers!
 * if STM execute func to fast, then buffer created in func
 * will exist anymore, and DMA will send garbage!
 * As you know vars created in func exist while in func!
 * Be carefull!
 */
void sendData16_DMA_FSMC(void *data, uint32_t transferSize)
{
  if(transferSize > MAX_DMA_REQUEST) {
      DMA_SetCurrDataCounter(DMA_FSMC_STREAM, MAX_DMA_REQUEST);  // DMA_SetCurrDataCounter
      data_left = (transferSize - MAX_DMA_REQUEST);
  } else {
      DMA_SetCurrDataCounter(DMA_FSMC_STREAM, transferSize);     // set how much data to transfer
//      data_left = 0;
  }

  //apply DMA_MemoryBaseAddr; enable memory increment; shot DMA to transfer;
  setMemoryBaseAddr_DMA_FSMC(data);
  DMA_MemoryIncrementConfig(DMA_FSMC_STREAM, ENABLE);
  DMA_Cmd(DMA_FSMC_STREAM, ENABLE);
}

/*
 * No check for transfer data size
 */
void sendData16_Fast_DMA_FSMC(void *data, uint16_t transferSize)
{
  // set how much data to transfer
  DMA_SetCurrDataCounter(DMA_FSMC_STREAM, transferSize);

  //apply DMA_MemoryBaseAddr; enable memory increment; shot DMA to transfer;
  setMemoryBaseAddr_DMA_FSMC(data);
  DMA_MemoryIncrementConfig(DMA_FSMC_STREAM, ENABLE);
  DMA_Cmd(DMA_FSMC_STREAM, ENABLE);
}

void wait_DMA_FSMC_busy(void)
{
//	while(DMA_GetFlagStatus(DMA_FSMC_STREAM, DMA_FLAG_TCIF0) != SET);
//	DMA_ClearFlag(DMA_FSMC_STREAM, DMA_FLAG_TCIF0);

	while(DMA_GetCmdStatus(DMA_FSMC_STREAM) != DISABLE);
}

void DMA_FSMC_IRQ_HANLER(void)
{
  if(DMA_GetITStatus(DMA_FSMC_STREAM, DMA_IT_TCIF0)) {
    DMA_ClearITPendingBit(DMA_FSMC_STREAM, DMA_IT_TCIF0);   // clear interrupt

    if(data_left) {  // all pixels transfered?
      DMA_Cmd(DMA_FSMC_STREAM, DISABLE);

      if(data_left > MAX_DMA_REQUEST) {         // left something?
	    DMA_SetCurrDataCounter(DMA_FSMC_STREAM, MAX_DMA_REQUEST); // DMA_SetCurrDataCounter
	    data_left -= MAX_DMA_REQUEST;
      } else {                                 // nope, this is last shoot
	    DMA_SetCurrDataCounter(DMA_FSMC_STREAM, data_left);      // DMA_SetCurrDataCounter
	    data_left = 0;
      }
      DMA_Cmd(DMA_FSMC_STREAM, ENABLE);  // shot DMA to transfer;
    } else {   // nope, end of transmission
      DMA_MemoryIncrementConfig(DMA_FSMC_STREAM, DISABLE);
      DMA_Cmd(DMA_FSMC_STREAM, DISABLE);
    }
  }
}

#endif /* defined(STM32F40XX) || defined(STM32F10X_HD) */
