#include <string.h>
#include <stm32f10x.h>

#include <gfx.h>
#include <systicktimer.h>

#include <memHelper.h>

#include "gpuMain.h"
#include "gpuTiles.h"

#include "stm_gpu_nes_tiles2.h"
#include "stm_gpu_nes_tilesMap.h"

#include "landscapeTileset.h"
#include "landscapeTileMap.h"

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
void drawSTMsGPU(void)
{
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
      tile.rndTileX = (80 + ( tile.rndTileX * TILE_8_BASE_SIZE ));
      tile.rndTileY = (88 + ( tile.rndTileY * TILE_8_BASE_SIZE ));
      
      // draw tile
      drawTile8x8(&tile);
      
      // make a little delay, overvise we don`t see mosaic effect (too fast)
      _delayMS(1);
    }
  }
}


void drawBootLogo(void)
{
  // 18 tiles, 8 wide, tiles array, tile size 8x8
  loadInternalTileSet(18, 6, titleScreenTiles_data);
  memcpy32(getMapArrPointer(), lanscapeTileMap, BACKGROUND_SIZE);
  drawBackgroundMap();
  
  // 67 tiles, 17 wide, tiles array, tile size 8x8
  loadInternalTileSet(67, 17, stm_gpu_nes_tiles2_data);
  drawSTMsGPU();
  
  
  setCursor(190, 230); // magic numbers! ohohoho!!!
  print(T_AUTHOR_ME);
  setCursor(0, 168); // magic numbers! ahahhaa!!!
  _delayMS(50); // he he he :[)
}