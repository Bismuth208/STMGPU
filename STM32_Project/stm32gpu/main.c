/*
*
* STM32_GPU Project
* Creation start: 10.04.2016 20:21 (UTC+4)
*
* Created by: Antonov Alexandr (Bismuth208)
*
* For addition info look read.me
*
*/

#include <string.h>
#include <stdint.h>

#include <stm32f10x.h>

#include <gfx.h>
#include <gfxDMA.h>
#include <spi.h>
#include <usart.h>
#include <systicktimer.h>

#include "sdLoader.h"
#include "gpuMain.h"
#include "gpuTiles.h"

#include "stm_gpu_nes_tiles2.h"
#include "stm_gpu_nes_tilesMap.h"


//===========================================================================//

static uint32_t nextInt = 9;  // yes, it real, true dice roll "random"!

//===========================================================================//
uint32_t randNum(void)
{
  nextInt ^= nextInt >> 6;
  nextInt ^= nextInt << 11;
  nextInt ^= nextInt >> 15;
  return ( nextInt * 3538123 );
}

void initRand(void)
{
  RCC->APB2ENR |= RCC_APB2Periph_ADC1; // Enable ADC1 Clock
  RCC->CFGR |= (0x2<<14);     //set ADC Prescaler '6'
  
  ADC1->CR2 |= (1<<4);         //Input Channel 16
  ADC1->SMPR1 |= (0x4<<18);  //41.5 cycles sample time
  ADC1->CR2 |= (1<<23);        //Enable Temperature Sensor & Vref
  ADC1->CR2 |= (1<<0);         //Enable ADC and start conversion
  
  for (uint8_t i = 0; i < 8; i++) {
    ADC1->CR2 |= (1<<0);         //Enable ADC and start conversion
    while(!(ADC1->SR & (1<<1))); //Wait until end of conversion
    
    nextInt += ADC1->DR;
  }
  
  //disable ADC1 to save power
  ADC1->CR2 &= ~(1<<0);
  RCC->APB2ENR &= ~RCC_APB2Periph_ADC1;
}

//===========================================================================//
// draw "STM GPU" pic whith mosaic effect
// pic consist of 18 tiles wide and 6 tiles high
void drawSturtupScreen(void)
{
  fillScreen(COLOR_BLACK);
  
  uint8_t tilesLesft = 108; //stmGpuTileMap[0] * stmGpuTileMap[1];     // 18*6 = 108, tiles in pic
  // bit field? no no no, what you talking about?
  uint8_t tilesDrawed[108];     // array of flags to show drawed tiles
  
  struct tile_t {
    uint16_t rndTileX;
    uint16_t rndTileY;
    uint8_t tileIndex;
  } tile;
  
  memset(tilesDrawed, 0x01, tilesLesft); // 1 - need to draw, 0 - drawed
  
  while(tilesLesft) {
    tile.rndTileX = ( randNum() % stmGpuTileMap[0] ); // from 0 to 18
    tile.rndTileY = ( randNum() % stmGpuTileMap[1] ); // from 0 to 6
    
    // index to draw is: x + y*wide
    
    tile.tileIndex = tile.rndTileX + tile.rndTileY*stmGpuTileMap[0];
    
    if(tilesDrawed[tile.tileIndex]) { // == 1, need to draw tile?
      
      tilesDrawed[tile.tileIndex] = 0; // change flag to 'drawed'
      
      --tilesLesft;
      
      // get tile index from tile map
      // 2 is offet from tileswide and tileshigh
      tile.tileIndex = stmGpuTileMap[2 + tile.tileIndex];
      
      // reuse rndTileX and rndTileY vars to show where to draw;
      // trying to draw in centre of screen
      tile.rndTileX = ((_width/6) + ( tile.rndTileX * TILE_BASE_SIZE ));
      tile.rndTileY = (( _height/4) + ( tile.rndTileY * TILE_BASE_SIZE ));
      
      // draw tile
      drawTile8x8(&tile);
      
      // make a little delay, overvise we don`t see mosaic effect (too fast)
      _delayMS(1);
    }
  }
  
  setCursor(_width/5, _height-15);
  setTextColorBG(currentPaletteArr[0x0A], COLOR_BLACK); // 0x0A - dark green
  print("Powered by Bismuth208");
  setCursor(0, 0);
  _delayMS(50); // he he he :[)
}

void init_GPIO_RCC(void)
{
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOA); // GPIOA
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOB); // GPIOB
  
#ifdef STM32F10X_MD
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOC); // GPIOC
#endif

#ifdef STM32F10X_HD
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOE); // GPIOE
  
 #if USE_FSMC
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOC); // GPIOC
  SET_BIT(RCC->APB2ENR, RCC_APB2Periph_GPIOD); // GPIOD
 #endif  
#endif
}

void startupInit(void)
{
  /*
  * NOTE!
  * STM32 confugired for 72Mhz Sys clock!
  * to change that go to "system_stm32f10x.c"
  * and find "#define SYSCLK_FREQ_72MHz  72000000"
  * select what you need
  */
  SystemInit(); // init clocks and another s...
  initSysTickTimer();
  
  init_GPIO_RCC();
  init_SPI1();
  initRand();
  
  initLCD();         /* initialize a ILI9341 chip */
  setRotation(1); // Horizontal
  setCurrentFont(1);
  
  // 67 tiles, 17 wide, tiles array, tile size 8x8
  loadLogoTileSet(67, 17, stm_gpu_nes_tiles2_data);      // startup logo
  loadDefaultPalette();
  
  drawSturtupScreen();
  
  // Init SD and SPI_2 after all inited
  init_sdCard();
}

//------------------------- yep, here's how it all began... -------------------//
__noreturn void main(void)
{
  startupInit();
  
  init_GPU();
  sync_CPU();   /* make a sync whith a CPU */
  run_GPU();
}
//-----------------------------------------------------------------------------//

#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif
