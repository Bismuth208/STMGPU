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

#define SD 1 // Use FatFS

#include <string.h>
#include <stdint.h>

#include <stm32f10x.h>

#include <gfx.h>
#include <gfxDMA.h>
#include <spi.h>
#include <usart.h>
#include <systicktimer.h>

#include "sdWorker.h"
#include "gpuWorker.h"
#include "gpuTiles.h"

#include "nesPalette_ext.h"

#include "stm_gpu_nes_tiles2.h"
#include "stm_gpu_nes_tilesMap.h"


//===========================================================================//

static uint32_t nextInt = 9;

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
  nextInt += 5; // yes, it real, true dice roll "random"!
}

//===========================================================================//
// draw "STM GPU" pic whith mosaic effect
// pic consist of 18 tiles wide and 6 tiles high
void drawSturtupScreen(void)
{
  tftFillScreen(COLOR_BLACK);
  
  uint8_t rndTileX, rndTileY, tileIndex;
  uint8_t tilesLesft = 108; //stmGpuTileMap[0] * stmGpuTileMap[1];     // 18*6 = 108, tiles in pic
  // bit field? no no no, what you talking about?
  uint8_t tilesDrawed[108];     // array of flags to show drawed tiles
  
  memset(tilesDrawed, 0x01, tilesLesft); // 1 - need to draw, 0 - drawed
  
  while(tilesLesft) {
    rndTileX = ( randNum() % stmGpuTileMap[0] ); // from 0 to 18
    rndTileY = ( randNum() % stmGpuTileMap[1] ); // from 0 to 6
    
    // index to draw is: x + y*wide
    
    tileIndex = rndTileX + rndTileY*stmGpuTileMap[0];
    
    if(tilesDrawed[tileIndex]) { // == 1, need to draw tile?
      
      tilesDrawed[tileIndex] = 0; // change flag to 'drawed'
      
      --tilesLesft;
      
      // get tile index from tile map
      // 2 is offet from tileswide and tileshigh
      tileIndex = stmGpuTileMap[2 + tileIndex];
      
      // reuse this vars to show where to draw;
      // 10 and 40 is base position on sreen in px
      rndTileX = ((_width/6) + ( rndTileX * TILE_BASE_SIZE ));          // old = 10
      rndTileY = (( _height/4) + ( rndTileY * TILE_BASE_SIZE ));        // old = 40
      
      // draw tile
      drawTile8x8(rndTileX, rndTileY, tileIndex);
    
      // make a little delay, overvise we don`t see mosaic effect (too fast)
      _delayMS(10);
    }
  }
  
  setCursor(_width/5, _height-15);
  setTextColorBG(pCurrentPalette[0x0A], COLOR_BLACK); // 0x0A - dark green
  print("Powered by Bismuth208");
  setCursor(0, 0);
  _delayMS(500); // he he he :[)
}

// debug
void tileSetTest(void)
{
  uint16_t rndPosX, rndPosY;
  uint8_t path[12] = "tileset.dat";
  
  // load 90 tiles from tile set  
  SDLoadTileSet8x8(path, 9, 0, 90);
  
  while(1) {
    rndPosX = randNum() % _width;
    rndPosY = randNum() % _height;
    
    drawTile8x8(rndPosX, rndPosY, randNum()%90);
  }
}

// debug
void drawRamTileSet8x8(void)
{
  int16_t posX, posY;
  uint8_t count =0;
  uint8_t path[12] = "tileset.dat";
  
  // load 90 tiles from tile set  
  SDLoadTileSet8x8(path, 9, 0, 90);
  
  for(uint8_t countY =0; countY <10; countY++) {
    for(uint8_t countX =0; countX <9; countX++) {
      
      posX = (10 + ( countX * TILE_BASE_SIZE ));
      posY = (10 + ( countY * TILE_BASE_SIZE ));
      
      drawTile8x8(posX, posY, count);
      
      ++count;
    }
  }
  
  while(1);
}

void init_GPIO_RCC(void)
{
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
  
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN); // GPIOA
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN); // GPIOB
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPCEN); // GPIOC
  
#if USE_FSMC
  //RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE);
  
  //SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPCEN); // GPIOC
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPDEN); // GPIOD
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPEEN); // GPIOE

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
  
  tftBegin();         /* initialize a ILI9341 chip */
  //initR(INITR_BLACKTAB);
  
  tftSetRotation(1); // Horizontal
  //setTextColorBG(COLOR_GREEN, COLOR_BLACK);
  //setTextColor(COLOR_GREEN);
  
  //initRand();
  
  setCurrentPalette(nesPalette_ext); // extended NES palette
  
  // 67 tiles, 17 wide, tiles array, tile size 8x8
  loadTileSet(67, 17, stm_gpu_nes_tiles2_data, 1); // startup logo

  drawSturtupScreen();
  
  // Init SD after all inited
  init_sdCard();
  
  //drawRamTileSet8x8();
  tileSetTest();
}

//------------------------- yep, here's how it all began... -------------------//
__noreturn __task void main(void)
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
