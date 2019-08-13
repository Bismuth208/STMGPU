#ifdef STM32F40XX
#include <stm32f4xx.h>
#else
#include <stm32f10x.h>
#endif /* STM32F40XX */

#include <string.h>

#include "memHelper.h"

#define CCR_CLEAR_Mask  ((uint32_t)0xFFFF800F)

uint32_t dmaDataBuf;                   // for single storage

inline void memcpy8(void *dst, void const *src, size_t len)
{
  uint8_t *pDst = (uint8_t *) dst;
  uint8_t const *pSrc = (uint8_t const *) src;

  while (len--) {
    *pDst++ = *pSrc++;
  }
}

// The modified-GNU algorithm
void memcpy32(void *dst, void const *src, size_t len)
{
  uint32_t *plDst = (uint32_t *) dst;
  uint32_t const *plSrc = (uint32_t const *) src;

  while (len >= 4) {
    *plDst++ = *plSrc++;
    len -= 4;
  }

  uint8_t *pcDst = (uint8_t *) plDst;
  uint8_t const *pcSrc = (uint8_t const *) plSrc;

  while (len--) {
    *pcDst++ = *pcSrc++;
  }
}

// set by block in 4 bytes only!
void memset32(void *dst, uint32_t src, size_t len)
{
  uint32_t *plDst = (uint32_t *) dst;

  while (len--) {
    *plDst++ = src;
  }
}

#if defined(STM32F10X_MD) || defined(STM32F10X_HD)
void init_DMA_memset(void)
{
  uint32_t tmpreg = ( DMA_DIR_PeripheralDST |  // .DMA_DIR
      DMA_Mode_Normal |// .DMA_Mode
      DMA_PeripheralInc_Enable |// .DMA_PeripheralInc
      DMA_MemoryInc_Disable |// .DMA_MemoryInc
      DMA_PeripheralDataSize_Byte |// .DMA_PeripheralDataSize
      DMA_MemoryDataSize_Byte |// .DMA_MemoryDataSize
      DMA_Priority_VeryHigh |// .DMA_Priority
      DMA_M2M_Enable |// .DMA_M2M
      DMA_IT_TC );// enable interupt

  /* Clear MEM2MEM, PL, MSIZE, PSIZE, MINC, PINC, CIRC and DIR bits */
  MODIFY_REG(DMA1_Channel1->CCR, CCR_CLEAR_Mask, tmpreg); // apply new settings

  NVIC_InitTypeDef NVIC_InitStructure;
  //Enable DMA1 channel IRQ Channel
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

// NOTE: len must be less than 65535!
// it is max DMA transfer size
void memset_DMA1(void *dst, void const *src, size_t len)
{
  dmaDataBuf = (*(uint32_t*)src);   // store data or, you can lost it

  DMA1_Channel1->CPAR = (uint32_t) dst;// set DMA_PeripheralBaseAddr
  DMA1_Channel1->CMAR = (uint32_t) &dmaDataBuf;// apply DMA_MemoryBaseAddr
  DMA1_Channel1->CNDTR = len;// set how much data to transfer
  DMA1_Channel1->CCR |= DMA_CCR1_EN;// shot DMA to transer;
}

void DMA1_Channel1_IRQHandler(void)
{
  if((DMA1->ISR & DMA1_IT_TC1)) {    // is it our IRQ? // != (uint32_t)RESET

    CLEAR_BIT(DMA1_Channel1->CCR, DMA_CCR1_EN);

    DMA1->IFCR = DMA1_IT_TC1;// clear interrupt
  }
}
#endif
