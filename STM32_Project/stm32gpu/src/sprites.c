#include <string.h>

#include <stm32f10x.h>

#include <gfx.h>
#if USE_FSMC
 #include <fsmcdrv.h>
#else
 #include <spi.h>
#endif
#include <memhelper.h>

#include "tiles.h"
#include "sprites.h"

//===========================================================================//

sprite_t spriteArr[MAX_SPRITE_NUM];

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

void getSpriteData(uint8_t sprNum, spriteData_t *pSprData)
{
  object_t sprBase;
  
  sprBase.posX = spriteArr[sprNum].posX;
  sprBase.posY = spriteArr[sprNum].posY;
  
  switch(spriteArr[sprNum].type)
  {
  case SPR_1X1_8: {
    //pSprData->tleNum = SPR_TLE_NUM_1x1;
    //pSprData->spriteSize = SPR_SIZE_1X1_8;
    //pSprData->posOffsetX = sprBase.posX+7;
    //pSprData->posOffsetY = sprBase.posY+7;
    sprBase.width = 7;
    sprBase.height = 7;
  } break;
  
  case SPR_1X2_8: {
    //pSprData->tleNum = SPR_TLE_NUM_1x2;
    //pSprData->spriteSize = SPR_SIZE_1X2_8;
    //pSprData->posOffsetX = sprBase.posX+15;
    //pSprData->posOffsetY = sprBase.posY+7;
    sprBase.width = 15;
    sprBase.height = 7;           
  } break;
  
  case SPR_2X1_8: {
    //pSprData->tleNum = SPR_TLE_NUM_1x2;
    //pSprData->spriteSize = SPR_SIZE_1X2_8;
    //pSprData->posOffsetX = sprBase.posX+7;
    //pSprData->posOffsetY = sprBase.posY+15;
    sprBase.width = 7;
    sprBase.height = 15;
  } break;
  
  case SPR_2X2_8: {
    //pSprData->tleNum = SPR_TLE_NUM_2x2;
    //pSprData->spriteSize = SPR_SIZE_2X2_8;
    //pSprData->posOffsetX = sprBase.posX+15;
    //pSprData->posOffsetY = sprBase.posY+15;
    sprBase.width = 15;
    sprBase.height = 15;
  } break;
  
  default: return;
  }
  
  memcpy32(&pSprData->sprBase, &sprBase, sizeof(object_t));
}

uint8_t getTileOffsetSize(uint8_t sprNum)
{
  uint8_t value =0;
    
  switch(spriteArr[sprNum].type)
  {
  case SPR_1X1_8: case SPR_1X2_8:
  case SPR_2X1_8: case SPR_2X2_8: {
    value = TILE_8_BASE_SIZE;
  } break;
  
  case SPR_1X1_16: case SPR_1X2_16:
  case SPR_2X1_16: case SPR_2X2_16: {
    value = TILE_16_BASE_SIZE;
  } break;

  case SPR_1X1_32: case SPR_1X2_32:
  case SPR_2X1_32: case SPR_2X2_32: {
    value = TILE_32_BASE_SIZE;
  } break;
  }
  
  return value;
}

uint8_t getTleNumberCount(uint8_t sprNum)
{
  uint8_t value =0;
  
  switch(spriteArr[sprNum].type)
  {
  case SPR_1X1_8: value = SPR_TLE_NUM_1x1; break;
  case SPR_1X2_8: value = SPR_TLE_NUM_1x2; break;  //  \ __ not mistake
  case SPR_2X1_8: value = SPR_TLE_NUM_1x2; break;  //  /    just same sizes
  case SPR_2X2_8: value = SPR_TLE_NUM_2x2; break;
  
  case SPR_1X1_16: value = SPR_TLE_NUM_1x1; break;
  case SPR_1X2_16: value = SPR_TLE_NUM_1x2; break;
  case SPR_2X1_16: value = SPR_TLE_NUM_1x2; break;
  case SPR_2X2_16: value = SPR_TLE_NUM_2x2; break;

  case SPR_1X1_32: value = SPR_TLE_NUM_1x1; break;
  case SPR_1X2_32: value = SPR_TLE_NUM_1x2; break;
  case SPR_2X1_32: value = SPR_TLE_NUM_1x2; break;
  case SPR_2X2_32: value = SPR_TLE_NUM_2x2; break;
  }
  
  return value;
}

void drawSprite(uint8_t sprNum)
{
  if(spriteArr[sprNum].visible) {
    
    struct tile_t {
      uint16_t posX;
      uint16_t posY;
      uint8_t tileNum;
    } tile;
    
    uint8_t offset = getTileOffsetSize(sprNum);
    uint8_t numberOfTiles = getTleNumberCount(sprNum);
    
    for(uint8_t tleCount=0; tleCount < numberOfTiles; tleCount++) {
      
      tile.tileNum = spriteArr[sprNum].tle[tleCount];
      
      switch(tleCount)
      {
      case 0: {
        tile.posX = spriteArr[sprNum].posX;
        tile.posY = spriteArr[sprNum].posY;
      } break;
      
      case 1: {
        tile.posX = spriteArr[sprNum].posX + offset;
        tile.posY = spriteArr[sprNum].posY;
      } break;
      
      case 2: {
        tile.posX = spriteArr[sprNum].posX;
        tile.posY = spriteArr[sprNum].posY + offset;
      } break;
      
      case 3: {
        tile.posX = spriteArr[sprNum].posX + offset;
        tile.posY = spriteArr[sprNum].posY + offset;
      } break;
      }
      
      drawTile8x8(&tile);
    }
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
  bool state = false;
  
  if((spriteArr[sprNum1].visible) && (spriteArr[sprNum2].visible)) {
    
    spriteData_t sprDat1;
    getSpriteData(sprNum1, &sprDat1);
    
    spriteData_t sprDat2;
    getSpriteData(sprNum2, &sprDat2);
    
    state = checkCollisionExP(&sprDat1.sprBase, &sprDat2.sprBase);
  } 
  
  return state;
}
