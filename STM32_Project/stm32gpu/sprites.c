#include <stm32f10x.h>
#include <string.h>

#include <gfx.h>
#include <gfxDMA.h>
#include <spi.h>
#include <memhelper.h>

#include "gpuTiles.h"
#include "sprites.h"

//===========================================================================//

sprite_t spriteArr[MAX_SPRITE_NUM];

uint8_t lastSpriteNum = 0xFF;
uint16_t lastSprite[LAST_SPRITE_SIZE]; // 4*2*maxSprSize

uint8_t spriteAutoUpdFlag = 0;

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

void setSpriteTiles(void *src)
{
  uint8_t *pData = (uint8_t*)src;
  uint8_t sprNum = *pData++;
  
  memcpy(spriteArr[sprNum].tle, pData, 0x4);
}

void convertSprite(uint8_t sprNum, uint8_t size)
{
  uint8_t *pTileArr = 0;
  uint16_t tleOffset = 0;
  
  for(uint8_t tleCount=0; tleCount < size; tleCount++) {
    
    pTileArr = getArrTilePointer8x8(spriteArr[sprNum].tle[tleCount]);
    
    for(uint16_t count =0; count < TILE_ARR_8X8_SIZE; count++) {
      // convert colors from current palette to RGB565 color space
      lastSprite[count + tleOffset] = currentPaletteArr[*pTileArr++];
    }
    
    tleOffset += TILE_ARR_8X8_SIZE;
  }
}

void getSpriteData(uint8_t sprNum, spriteData_t *pSprData)
{
  object_t sprBase;
  
  sprBase.posX = spriteArr[sprNum].posX;
  sprBase.posY = spriteArr[sprNum].posY;
  
  switch(spriteArr[sprNum].type)
  {
  case SPR_1X1_8: {
    pSprData->tleNum = SPR_TLE_NUM_1x1;
    pSprData->spriteSize = SPR_SIZE_1X1_8;
    pSprData->posOffsetX = sprBase.posX+7;
    pSprData->posOffsetY = sprBase.posY+7;
    sprBase.width = 7;
    sprBase.height = 7;
  } break;
  
  case SPR_1X2_8: {
    pSprData->tleNum = SPR_TLE_NUM_1x2;
    pSprData->spriteSize = SPR_SIZE_1X2_8;
    pSprData->posOffsetX = sprBase.posX+15;
    pSprData->posOffsetY = sprBase.posY+7;
    sprBase.width = 15;
    sprBase.height = 7;           
  } break;
  
  case SPR_2X1_8: {
    pSprData->tleNum = SPR_TLE_NUM_1x2;
    pSprData->spriteSize = SPR_SIZE_1X2_8;
    pSprData->posOffsetX = sprBase.posX+7;
    pSprData->posOffsetY = sprBase.posY+15;
    sprBase.width = 7;
    sprBase.height = 15;
  } break;
  
  case SPR_2X2_8: {
    pSprData->tleNum = SPR_TLE_NUM_2x2;
    pSprData->spriteSize = SPR_SIZE_2X2_8;
    pSprData->posOffsetX = sprBase.posX+15;
    pSprData->posOffsetY = sprBase.posY+15;
    sprBase.width = 15;
    sprBase.height = 15;
  } break;
  
  default: return;
  }
  
  memcpy32(&pSprData->sprBase, &sprBase, sizeof(object_t));
}

void drawSprite(uint8_t sprNum)
{
  if(spriteArr[sprNum].visible) {
    /*
    if(sprNum < SPR_1X1_16) {
      pConvSprite = convertSprite8x8;
      pGetSpriteData = getSpriteData8x8;
      
    } else if(() && ()) {
      pConvSprite = convertSprite16x16;
      pGetSpriteData = getSpriteData16x16;
    } else {
      pConvSprite = convertSprite32x32;
      pGetSpriteData = getSpriteData32x32;
    }
    */
    spriteData_t spriteData;
    getSpriteData(sprNum, &spriteData);
    
    if(lastSpriteNum != sprNum) {
      convertSprite(sprNum, spriteData.tleNum);
    }
    
    setAddrWindow(spriteData.sprBase.posX, spriteData.sprBase.posY, 
                  spriteData.posOffsetX, spriteData.posOffsetY);
    sendData16_Fast_DMA1_SPI1(lastSprite, spriteData.spriteSize);
  }
}

void setSpritesAutoRedraw(uint8_t state)
{
  spriteAutoUpdFlag = state;
}


bool checkCollisionExP(object_t *objOne, object_t *objTwo)
{
  /* ----------- Check X position ----------- */
  uint16_t objOnePosEndX = (objOne->posX + objOne->width);

  if (objOnePosEndX >= objTwo->posX) {
    uint16_t objTwoPosEndX = (objTwo->posX + objTwo->width);
    
    if (objOne->posX >= objTwoPosEndX) {
      return false; // nope, different X positions
    }
    // ok, objects on same X lines; Go next...
  } else {
    return false; // nope, absolutelly different X positions
  }
  
  /* ---------------------------------------- */
  /* ----------- Check Y position ----------- */
  uint16_t objOnePosEndY = (objOne->posY + objOne->height);
  
  if (objOnePosEndY >= objTwo->posY) {
    uint16_t objTwoPosEndY = (objTwo->posY + objTwo->height);
    
    if (objOne->posY <= objTwoPosEndY) {
      // ok, objects on same Y lines; Go next...
      
      // yep, if we are here
      // then, part of one object collide wthith another object
      return true;
    } else {
      return false; // nope, different Y positions
    }
  } else {
    return false; // nope, absolutelly different Y positions
  }
}

uint8_t getSpriteCollision(uint8_t sprNum1, uint8_t sprNum2)
{
  if((spriteArr[sprNum1].visible) && (spriteArr[sprNum2].visible)) {
    
    spriteData_t sprDat1;
    getSpriteData(sprNum1, &sprDat1);
    
    spriteData_t sprDat2;
    getSpriteData(sprNum2, &sprDat2);
    
    return checkCollisionExP(&sprDat1.sprBase, &sprDat2.sprBase);
    
  } else {
    return false;
  }
}
