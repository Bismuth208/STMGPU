#include <stm32f10x.h>

#include <string.h>

#include <gfx.h>
#include <gfxDMA.h>
#include <spi.h>

#include "gpuTiles.h"
#include "nesPalette_ext.h"

// -------------------------------------------------------- //

// RAM size is limited and share to all type of sprites!
// total: 10240
// 6144 ( if 96x64 )
uint8_t tileArr8x8[TILES_NUM_8x8][TILE_ARR_8X8_SIZE] = {0, 0};
// 4096 ( if 16x256 )
uint8_t tileArr16x16[TILES_NUM_16x16][TILE_ARR_16X16_SIZE] = {0, 0};

#ifdef STM32F10X_HD
// 10240 ( if 10x1024 )
uint8_t tileArr32x32[TILES_NUM_32x32][TILE_ARR_32X32_SIZE] = {0, 0};
#endif

// total: 640
uint16_t lastTile8x8[TILE_ARR_8X8_SIZE];       // 128
uint16_t lastTile16x16[TILE_ARR_16X16_SIZE];   // 512

lastTile_t lastTileStruct8x8 = {0, 0, lastTile8x8};
lastTile_t lastTileStruct16x16 = {0, 0, lastTile16x16};

// on screen tile map background
// total: 1200 ( if 40x30 )
uint8_t rearBackGround[BACKGROUND_SIZE] = {0};
//uint8_t middleBackGround[BACKGROUND_SIZE_W][BACKGROUND_SIZE_H] = {0, 0};
//uint8_t frontBackGround[BACKGROUND_SIZE_W][BACKGROUND_SIZE_H] = {0, 0};

// total: 512
uint16_t currentPaletteArr[USER_PALETTE_SIZE]; // user can specify max 256 colors

// -------------------------------------------------------- //

void loadDefaultPalette(void)
{
  for(uint8_t count=0; count <80; count++) {
    currentPaletteArr[count] = nesPalette_ext[count]; // extended NES palette
  }   
}

#if 0
void loadLogoTileSet(uint8_t tileSetSize, uint8_t tileSetW, const uint8_t *pTileSet)
{
  uint8_t count =0;
  uint8_t tileNum=0;
  uint8_t tileYcnt, tileXcnt;
  
  uint16_t tileNumOffsetX = (tileNum % tileSetW)*TILE_BASE_SIZE;     // start position
  uint16_t tileNewLineOffsetY = tileSetW*TILE_BASE_SIZE;       // offset for new scanline
  
  uint16_t offset = ((tileNum)  / (tileSetW)) * (tileSetW * TILE_ARR_8X8_SIZE);
  
  for(; tileNum <= tileSetSize; tileNum++) {
    
    for(tileYcnt = 0; tileYcnt < TILE_BASE_SIZE; tileYcnt++) { // Y
      for(tileXcnt =0; tileXcnt < TILE_BASE_SIZE; tileXcnt++) { // X
        tileArr8x8[tileNum][count] = pTileSet[tileNumOffsetX + tileXcnt + offset]; // read single line in X pos
        ++count;
      }
      offset += tileNewLineOffsetY;
    }
    
    count =0;
    offset = ((tileNum)  / (tileSetW)) * (tileSetW * TILE_ARR_8X8_SIZE);
    tileNumOffsetX = (tileNum % tileSetW)*TILE_BASE_SIZE;     // start position
    
  }
}
#else

void loadLogoTileSet(uint8_t tileSetSize, uint8_t tileSetW, const uint8_t *pTileSet)
{
  
  for(uint8_t tileNum=0; tileNum <= tileSetSize; tileNum++) {
    loadTile8x8(tileNum, tileSetW, tileArr8x8[tileNum], pTileSet);
  }
  
}
#endif

#if 1
void loadTile8x8(uint8_t tileNum, uint8_t tileSetW, uint8_t *pTile, const uint8_t *pTileSet)
{
  uint8_t count =0;
  uint8_t tileYcnt, tileXcnt;
  
  uint16_t tileNumOffsetX = (tileNum % tileSetW)*TILE_BASE_SIZE;     // start position
  uint16_t tileNewLineOffsetY = tileSetW*TILE_BASE_SIZE;       // offset for new scanline
  
  uint16_t offset = ((tileNum)  / (tileSetW)) * (tileSetW * TILE_ARR_8X8_SIZE);

  for(tileYcnt = 0; tileYcnt < TILE_BASE_SIZE; tileYcnt++) { // Y
    for(tileXcnt =0; tileXcnt < TILE_BASE_SIZE; tileXcnt++) { // X
      pTile[count] = pTileSet[tileNumOffsetX + tileXcnt + offset]; // read single line in X pos
      ++count;
    }
    offset += tileNewLineOffsetY;
  }
}
#endif
// -------------------------------------------------------- //

// return the pointer to given tile array number
uint8_t *getArrTilePointer8x8(uint8_t tileNum)
{
  return tileArr8x8[tileNum];
}

uint8_t *getMapArrPointer(void)
{
  return rearBackGround;
}

void drawTile8x8(int16_t posX, int16_t posY, uint8_t tileNum)
{
  // little trick, if tile same, just redraw it
  if((lastTileStruct8x8.drawed == 0) || (lastTileStruct8x8.lasttileNum != tileNum)) {
    
    uint8_t colorTileIdx;
    
    lastTileStruct8x8.drawed = 1;
    lastTileStruct8x8.lasttileNum = tileNum;
    
    for(uint16_t count =0; count < TILE_ARR_8X8_SIZE; count++) {
      // convert colors from current palette to RGB565 color space
      colorTileIdx = tileArr8x8[tileNum][count];
      lastTileStruct8x8.pLastTileArr[count] = currentPaletteArr[colorTileIdx];
    }
  }
  
  tftSetAddrWindow(posX, posY, posX+7, posY+7);
  sendData16_DMA1_SPI1(lastTileStruct8x8.pLastTileArr, TILE_ARR_8X8_SIZE);
}

// -------------------------------------------------------- //

void drawBackgroundMap(void)
{
  uint8_t tileX, tileY;
  uint16_t posX, posY;
  
  uint8_t tileNum =0;
  uint16_t tileMapCount =0;
  
  for(tileY=0; tileY < BACKGROUND_SIZE_H; tileY++) {
    for(tileX=0; tileX < BACKGROUND_SIZE_W; tileX++) {
      
      posX = ( tileX * TILE_BASE_SIZE );
      posY = ( tileY * TILE_BASE_SIZE );
      
      tileNum = rearBackGround[tileMapCount];
      
      //printChar(tileNum+48);
      
      drawTile8x8(posX, posY, tileNum);
      ++tileMapCount;
    }
  }
  
  //setCursor(0,0);
}

// little help
void *memcpy32(void *dst, void const *src, size_t len)
{
  uint32_t * ldst = (uint32_t *)dst;
  uint32_t * lsrc = (uint32_t *)src;
  while (len>=4){
    *ldst++ = *lsrc++;
    len -= 4;
  }
  return (dst);
}
