#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <systicktimer.h>

#include <stm32f10x.h>

#include "spi.h"

//---------------------------------------------------------------------------------------------//

#define CR1_CLEAR_Mask       ((uint16_t)0x3040)
#define SPI_Mode_Select      ((uint16_t)0xF7FF)
#define CR1_SPE_Set          ((uint16_t)0x0040)

//---------------------------------------------------------------------------------------------//


//--------------------------------------- SPI_1 ------------------------------------------------//
void init_SPI1(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  // Enable clock
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN);
  
  //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //выключаем JTAG, SW отстается включенным
  //GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
  
  //Заполняем поля структуры нашими параметрами
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;      // Режим альтернативной функции
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;     // Скорость
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_SPI_LCD_MOSI | GPIO_Pin_SPI_LCD_SCK ;
  GPIO_Init(GPIO_SPI_LCD, &GPIO_InitStruct);        // Применяем настроки на порт A
  
  uint16_t tmpreg = (uint16_t)((uint32_t)SPI_Direction_1Line_Tx |       // .SPI_Direction
                        SPI_Mode_Master |                       // .SPI_Mode
                        SPI_DataSize_8b |                       // .SPI_DataSize
                        SPI_CPOL_Low |                          // .SPI_CPOL
                        SPI_CPHA_1Edge |                        // .SPI_CPHA
                        SPI_NSS_Soft |                          // .SPI_NSS
                        SPI_BaudRatePrescaler_2 |               // .SPI_BaudRatePrescaler
                        SPI_FirstBit_MSB |                      // .SPI_FirstBit
                        CR1_SPE_Set |                           // Enable SPI
                        SPI_NSSInternalSoft_Set  );             // *see below PS
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
  
  /* *PS:
  * Поскольку сигнал NSS контролируется программно, установим его в единицу
  * Если сбросить его в ноль, то наш SPI модуль подумает, что
  * у нас мультимастерная топология и его лишили полномочий мастера.
  */ 
}

void sendData8_SPI1(uint8_t data)
{
  //CLEAR_BIT(SPI1->CR1, SPI_DataSize_16b);       // set 8bit size
  //WAIT_FREE_TX;         // wait for empty SPI_DR
  SPI1->DR = data;      // add data to Tx queue
  
  WAIT_FREE_TX;
  WAIT_FOR_BSY;
  //SET_BIT(SPI1->CR1, SPI_DataSize_16b);
}

void sendData16_SPI1(uint16_t data)
{
#if 0
  //WAIT_FREE_TX;
  SET_BIT(SPI1->CR1, SPI_DataSize_16b);
  
  SPI1->DR = data;
  
  WAIT_FREE_TX;
  WAIT_FOR_BSY;
  CLEAR_BIT(SPI1->CR1, SPI_DataSize_16b);       // set 8bit size
  
#else // i don't know how, but this faster...
  
  SPI1->DR = (data>>8);
  
  WAIT_FREE_TX;
  SPI1->DR = (data);
  
  WAIT_FREE_TX;
  WAIT_FOR_BSY;
#endif
}

void sendData32_SPI1(uint16_t data0, uint16_t data1)
{
#if 0
 //WAIT_FREE_TX;
  SET_BIT(SPI1->CR1, SPI_DataSize_16b);
  
  SPI1->DR = data0;
  
  WAIT_FREE_TX;
  SPI1->DR = data1;
  
  WAIT_FREE_TX;
  WAIT_FOR_BSY;
  CLEAR_BIT(SPI1->CR1, SPI_DataSize_16b);       // set 8bit size
  
#else // i don't know how, but this faster...
  
  SPI1->DR = (data0>>8);
  
  WAIT_FREE_TX;
  SPI1->DR = (data0);
  
  WAIT_FREE_TX;
  SPI1->DR = (data1>>8);
  
  WAIT_FREE_TX;
  SPI1->DR = (data1);
  
  WAIT_FREE_TX;
  WAIT_FOR_BSY;
#endif
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

//---------------------------------- Code dump... ---------------------------------------------//
#if 0
void init_SPI1(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  SPI_InitTypeDef SPI_InitStruct;
  
  // Подаем такты
  RCC_APB2PeriphClockCmd((RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1), ENABLE);
  
  //Заполняем поля структуры нашими параметрами
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;      // Режим альтернативной функции
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;     // Скорость
  GPIO_InitStruct.GPIO_Pin = MIS0_PIN | MOSI_PIN | SCK_PIN ;
  GPIO_Init(GPIOA, &GPIO_InitStruct);        // Применяем настроки на порт A

  SPI_StructInit(&SPI_InitStruct);

  //Заполняем структуру с параметрами SPI модуля
  SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx;             // указываем, что используем мы только передачу данных
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;                     // передаем по 8 бит
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;                            // Полярность и
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;                          // фаза тактового сигнала
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;                             // Управлять состоянием сигнала NSS программно
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;   // Предделитель SCK
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;                    // Первым отправляется старший бит
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;                         // Режим - мастер
  
  SPI_Init(SPI1, &SPI_InitStruct); //Настраиваем SPI1
  SPI_Cmd(SPI1, ENABLE); // Включаем модуль SPI1...
  
  // Поскольку сигнал NSS контролируется программно, установим его в единицу
  // Если сбросить его в ноль, то наш SPI модуль подумает, что
  // у нас мультимастерная топология и его лишили полномочий мастера.
  SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
}

uint16_t readData8_SPI1(uint8_t data)
{
  sendDataSPI(data); // отправим байт данных в очередь на отправку
  while ((SPI1->SR & SPI_I2S_FLAG_RXNE) == RESET); // ждём, приема в SPI_DR
    
  return SPI1->DR;
}
#endif
