#include <stm32f10x.h>

#include <gfx.h>
#include <gfxDMA.h>
#include <spi.h>

#include "gpuTiles.h"
#include "sprites.h"

//===========================================================================//

sprite_t spriteArr[MAX_SPRITE_NUM];

uint16_t lastSprite[LAST_SPRITE_SIZE]; // 4*2*maxSprSize

//===========================================================================//

void setSpritePosition(uint8_t sprNum, uint16_t posX, uint16_t posY)
{
  spriteArr[sprNum].posX = posX;
  spriteArr[sprNum].posY = posY;
}

void setSpriteType(uint8_t sprNum, uint8_t type)
{
  spriteArr[sprNum].type = type;
}

void setSpriteVisible(uint8_t sprNum, uint8_t state)
{
  spriteArr[sprNum].visible = state;
}

void setSpriteTiles(uint8_t sprNum, uint8_t tle1, uint8_t tle2, uint8_t tle3, uint8_t tle4)
{
  spriteArr[sprNum].tle[0] = tle1;
  spriteArr[sprNum].tle[1] = tle2;
  spriteArr[sprNum].tle[2] = tle3;
  spriteArr[sprNum].tle[3] = tle4;
}

void convertSprite(uint8_t sprNum, uint8_t size)
{
  uint8_t colorTileIdx;
  uint8_t *pTileArr = 0;
  uint16_t tleOffset = 0;
  
  for(uint8_t tleCount=0; tleCount < size; tleCount++) {
    
    pTileArr = getArrTilePointer8x8(spriteArr[sprNum].tle[tleCount]);
    
    for(uint16_t count =0; count < TILE_ARR_8X8_SIZE; count++) {
      // convert colors from current palette to RGB565 color space
      colorTileIdx = pTileArr[count];
      lastSprite[count + tleOffset] = currentPaletteArr[colorTileIdx];
    }
    
    tleOffset += TILE_ARR_8X8_SIZE;
  }
}

void drawSprite(uint8_t sprNum)
{
  if(spriteArr[sprNum].visible) {
    
    uint16_t posX = spriteArr[sprNum].posX;
    uint16_t posY = spriteArr[sprNum].posY;
    
    uint16_t posXOffset =0;
    uint16_t posYOffset =0;
    
    uint32_t spriteSize =0;
    uint8_t tleNum =0;
    
    switch(spriteArr[sprNum].type)
    {
    case SPR_1X1_8: {
      tleNum = SPR_TLE_NUM_1x1;
      spriteSize = SPR_SIZE_1X1_8;
      posXOffset = posX+7;
      posYOffset = posY+7;
    } break;
    
    case SPR_1X2_8: {
      tleNum = SPR_TLE_NUM_1x2;
      spriteSize = SPR_SIZE_1X2_8;
      posXOffset = posX+15;
      posYOffset = posY+7;           
    } break;
    
    case SPR_2X1_8: {
      tleNum = SPR_TLE_NUM_1x2;
      spriteSize = SPR_SIZE_1X2_8;
      posXOffset = posX+7;
      posYOffset = posY+15;
    } break;
    
    case SPR_2X2_8: {
      tleNum = SPR_TLE_NUM_2x2;
      spriteSize = SPR_SIZE_2X2_8;
      posXOffset = posX+15;
      posYOffset = posY+15;
    } break;
    /*
    case 4: {
    //tftSetAddrWindow(posX, posY, posX+7, posY+7);
    } break;
    
    case 5: {
    //tftSetAddrWindow(posX, posY, posX+7, posY+7);
    } break;
    */
    
    }
    
    convertSprite(sprNum, tleNum);
    tftSetAddrWindow(posX, posY, posXOffset, posYOffset);
    sendData16_DMA1_SPI1(lastSprite, spriteSize);
  }
}

void setSpritesAutoRedraw(uint8_t state)
{
  
  
}

uint8_t getSpriteCollision(uint8_t sprNum1, uint8_t sprNum2)
{
  
  return 0;
}