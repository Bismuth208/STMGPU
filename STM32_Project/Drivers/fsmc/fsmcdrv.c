#include <stm32f10x.h>
#include "fsmcdrv.h"


// 8 bit 8080 pins
#define FSMC_PIN_D0     GPIO_Pin_14     // PD14
#define FSMC_PIN_D1     GPIO_Pin_15     // PD15
#define FSMC_PIN_D2     GPIO_Pin_0      // PD0
#define FSMC_PIN_D3     GPIO_Pin_1      // PD1
#define FSMC_PIN_D4     GPIO_Pin_7      // PE7
#define FSMC_PIN_D5     GPIO_Pin_8      // PE8
#define FSMC_PIN_D6     GPIO_Pin_9      // PE9
#define FSMC_PIN_D7     GPIO_Pin_10     // PE10
#if 1 // for 16 bit 8080 pins
#define FSMC_PIN_D8     GPIO_Pin_11     // PE11
#define FSMC_PIN_D9     GPIO_Pin_12     // PE12
#define FSMC_PIN_D10    GPIO_Pin_13     // PE13
#define FSMC_PIN_D11    GPIO_Pin_14     // PE14
#define FSMC_PIN_D12    GPIO_Pin_15     // PE15
#define FSMC_PIN_D13    GPIO_Pin_8      // PD8
#define FSMC_PIN_D14    GPIO_Pin_9      // PD9
#define FSMC_PIN_D15    GPIO_Pin_10     // PD10
#endif

// RD - read; WR - write
#define FSMC_PIN_RD    GPIO_Pin_4      // PD4 (FSMC_PIN_NOE)
#define FSMC_PIN_WR    GPIO_Pin_5      // PD5 (FSMC_PIN_NWE)

// CS - chip select; RS - register select (D/C line)
#define FSMC_PIN_CS    GPIO_Pin_7      // PD7 (FSMC_PIN_NE1)
#define FSMC_PIN_RS    GPIO_Pin_11     // PD11 (FSMC_PIN_A16)


void sendRegData(uint16_t reg, uint16_t data)
{
  LCD_WRITE_REGISTER(reg, data);
}

void sendCMD8_FSMC(uint16_t c)
{
  FSMC_SEND_CMD(c);
}

void sendData8_FSMC(uint16_t c)
{
  FSMC_SEND_DATA(c);
}

void sendData8_Arr_FSMC(uint8_t *arr, uint16_t size)
{
  for(uint16_t count =0; count < size; count++) {
    FSMC_SEND_DATA((uint16_t)((0x00<<8)|arr[count]));
  }
}

void sendData16_FSMC(uint16_t data)
{
  FSMC_SEND_DATA(data);
}

void sendData16_Arr_FSMC(uint16_t *arr, uint16_t size)
{
  uint16_t tmp =0;
  
  for(uint16_t count =0; count < size; count++) {
    tmp = arr[count];
    FSMC_SEND_DATA(tmp);
  }
}

void repeatData16_Arr_FSMC(uint16_t data, uint32_t size)
{
  for(uint16_t count =0; count < size; count++) {
    FSMC_SEND_DATA(data);
  }
}

void sendData32_FSMC(uint16_t data1, uint16_t data2)
{
  FSMC_SEND_DATA(data1);
  FSMC_SEND_DATA(data2);
}


void initFSMC_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  // Init GPIO for FSMC
  GPIO_InitStruct.GPIO_Pin = FSMC_PIN_D0 | FSMC_PIN_D1 | FSMC_PIN_D2 | FSMC_PIN_D3 | FSMC_PIN_RD | FSMC_PIN_WR | FSMC_PIN_CS | FSMC_PIN_RS;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStruct);
  
  
  GPIO_InitStruct.GPIO_Pin = FSMC_PIN_D4 | FSMC_PIN_D5 | FSMC_PIN_D6 | FSMC_PIN_D7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStruct);
  
  
  /////////////////////////////////
  // CS -> 0
  // RD -> 1
  // WR -> 1
  
  GPIO_ResetBits(GPIOD, FSMC_PIN_CS);
  GPIO_SetBits(GPIOD, FSMC_PIN_RD);
  GPIO_SetBits(GPIOD, FSMC_PIN_WR);
}

void initFSMC(void)
{
  initFSMC_GPIO();
  
  FSMC_NORSRAMInitTypeDef fsmc;
  FSMC_NORSRAMTimingInitTypeDef fsmcTiming;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
  
  // setup FSMC
  fsmcTiming.FSMC_AddressSetupTime = 1;
  fsmcTiming.FSMC_AddressHoldTime = 0;
  fsmcTiming.FSMC_DataSetupTime = 5;
  fsmcTiming.FSMC_BusTurnAroundDuration = 0;
  fsmcTiming.FSMC_CLKDivision = 0;
  fsmcTiming.FSMC_DataLatency = 0;
  fsmcTiming.FSMC_AccessMode = FSMC_AccessMode_B;
  
  
  fsmc.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  fsmc.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  fsmc.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  fsmc.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  fsmc.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  fsmc.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  fsmc.FSMC_WrapMode = FSMC_WrapMode_Disable;
  fsmc.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  fsmc.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  fsmc.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  fsmc.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  fsmc.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  fsmc.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable; // need or not?
  fsmc.FSMC_ReadWriteTimingStruct = &fsmcTiming;
  fsmc.FSMC_WriteTimingStruct = &fsmcTiming;
  
  FSMC_NORSRAMInit(&fsmc);
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}
