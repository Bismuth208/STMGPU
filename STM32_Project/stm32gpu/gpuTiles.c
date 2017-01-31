#include <string.h>
#include <stm32f10x.h>

#include <gfx.h>
#include <gfxDMA.h>

#if USE_FSMC
 #include <fsmcdrv.h>
#else
 #include <spi.h>
#endif
#include <memHelper.h>

#include "gpuTiles.h"
#include "nesPalette_ext.h"

// -------------------------------------------------------- //

// RAM size is limited and share to all type of sprites!
uint8_t tileArr8x8[TILES_NUM_8x8][TILE_ARR_8X8_SIZE] = {0, 0};
uint8_t tileArr16x16[TILES_NUM_16x16][TILE_ARR_16X16_SIZE] = {0, 0};

// total: 640
uint16_t lastTile8x8[TILE_ARR_8X8_SIZE];       // 128
uint16_t lastTile16x16[TILE_ARR_16X16_SIZE];   // 512

// last tile id in tileArrYxY
uint8_t lastTileNum8x8 = 0xFF;
uint8_t lastTileNum16x16 = 0xFF;

#ifdef STM32F10X_HD
uint8_t tileArr32x32[TILES_NUM_32x32][TILE_ARR_32X32_SIZE] = {0, 0};
uint16_t lastTile32x32[TILE_ARR_32X32_SIZE];   // 2048
uint8_t lastTileNum32x32 = 0xFF;
#endif  /* STM32F10X_HD */


// on screen tile map background
// total: 1200 ( if 40x30 )
uint8_t mainBackGround[BACKGROUND_SIZE] = {0};

// total: 512
uint16_t currentPaletteArr[USER_PALETTE_SIZE]; // user can specify max 256 colors

uint8_t palChanged =0; // status flag; force to redraw tiles

// -------------------------------------------------------- //

// load to RAM built-in palette
void loadDefaultPalette(void)
{
  // 80 colors and each 2 byte in size
  memcpy32(currentPaletteArr, nesPalette_ext, 160);
}

#if 0
void loadInternalTileSet(uint8_t tileSetSize, uint8_t tileSetW, const uint8_t *pTileSet)
{
  uint8_t count =0;
  uint8_t tileNum=0;
  uint8_t tileYcnt, tileXcnt;
  
  uint16_t tileNumOffsetX = (tileNum % tileSetW)*TILE_8_BASE_SIZE;     // start position
  uint16_t tileNewLineOffsetY = tileSetW*TILE_8_BASE_SIZE;       // offset for new scanline
  
  uint16_t offset = ((tileNum)  / (tileSetW)) * (tileSetW * TILE_ARR_8X8_SIZE);
  
  for(; tileNum <= tileSetSize; tileNum++) {
    
    for(tileYcnt = 0; tileYcnt < TILE_8_BASE_SIZE; tileYcnt++) { // Y
      for(tileXcnt =0; tileXcnt < TILE_8_BASE_SIZE; tileXcnt++) { // X
        tileArr8x8[tileNum][count] = pTileSet[tileNumOffsetX + tileXcnt + offset]; // read single line in X pos
        ++count;
      }
      offset += tileNewLineOffsetY;
    }
    
    count =0;
    offset = ((tileNum)  / (tileSetW)) * (tileSetW * TILE_ARR_8X8_SIZE);
    tileNumOffsetX = (tileNum % tileSetW)*TILE_8_BASE_SIZE;     // start position
    
  }
}
#else

void loadInternalTileSet(uint8_t tileSetSize, uint8_t tileSetW, const uint8_t *pTileSet)
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
  
  uint16_t tileNumOffsetX = (tileNum % tileSetW)*TILE_8_BASE_SIZE;     // start position
  uint16_t tileNewLineOffsetY = tileSetW*TILE_8_BASE_SIZE;       // offset for new scanline
  
  uint16_t offset = ((tileNum)  / (tileSetW)) * (tileSetW * TILE_ARR_8X8_SIZE);

  for(tileYcnt = 0; tileYcnt < TILE_8_BASE_SIZE; tileYcnt++) { // Y
    for(tileXcnt =0; tileXcnt < TILE_8_BASE_SIZE; tileXcnt++) { // X
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

uint8_t *getArrTilePointer16x16(uint8_t tileNum)
{
  return tileArr16x16[tileNum];
}
#ifdef STM32F10X_HD
uint8_t *getArrTilePointer32x32(uint8_t tileNum)
{
  return tileArr32x32[tileNum];
}
#endif /* STM32F10X_HD */

uint8_t *getMapArrPointer(void)
{
  return mainBackGround;
}

void drawTile8x8(void *tile)
{
  uint16_t *pTile = (uint16_t *)tile;
  
  int16_t posX = *pTile++;
  int16_t posY = *pTile++;
  uint8_t tileNum = *pTile;
  
  // little trick, if tile same, just redraw it
  if(lastTileNum8x8 != tileNum) { // same tile number?
    
    uint8_t *pTileArr = &tileArr8x8[tileNum][0]; // get pointer by *pTile tile number
    lastTileNum8x8 = tileNum; // apply last tile number
    
    // convert colors from indexes in current palette to RGB565 color space
    for(uint16_t count =0; count < TILE_ARR_8X8_SIZE; count++) {
      lastTile8x8[count] = currentPaletteArr[*pTileArr++];
    }
  }
  
  // on oscilloscope this one reduce few uS
  setSqAddrWindow(posX, posY, TILE_8x8_WINDOW_SIZE);
#if USE_FSMC
  sendData16_Arr_FSMC(lastTile8x8, TILE_ARR_8X8_SIZE);
#else
  sendData16_Fast_DMA1_SPI1(lastTile8x8, TILE_ARR_8X8_SIZE);
#endif
}

void drawTile16x16(void *tile)
{
  uint16_t *pTile = (uint16_t *)tile;
  
  int16_t posX = *pTile++;
  int16_t posY = *pTile++;
  uint8_t tileNum = *pTile++;
  
  // little trick, if tile same, just redraw it
  if(lastTileNum16x16 != tileNum) {
    
    uint8_t *pTileArr = &tileArr16x16[tileNum][0];
    lastTileNum16x16 = tileNum;
    
    // convert colors from indexes in current palette to RGB565 color space
    for(uint16_t count =0; count < TILE_ARR_16X16_SIZE; count++) {
      lastTile16x16[count] = currentPaletteArr[*pTileArr++];
    }
  }
  
  // on oscilloscope this one reduce few uS
  setSqAddrWindow(posX, posY, TILE_16x16_WINDOW_SIZE);
#if USE_FSMC
  sendData16_Arr_FSMC(lastTile16x16, TILE_ARR_16X16_SIZE);
#else
  sendData16_Fast_DMA1_SPI1(lastTile16x16, TILE_ARR_16X16_SIZE);
#endif
}


#ifdef STM32F10X_HD
void drawTile32x32(void *tile)
{
  uint16_t *pTile = (uint16_t *)tile;
  
  int16_t posX = *pTile++;
  int16_t posY = *pTile++;
  uint8_t tileNum = *pTile++;
  
  // little trick, if tile same, just redraw it
  if(lastTileNum32x32 != tileNum) {
    
    uint8_t *pTileArr = &tileArr32x32[tileNum][0];
    lastTileNum32x32 = tileNum;
    
    // convert colors from indexes in current palette to RGB565 color space
    for(uint16_t count =0; count < TILE_ARR_32X32_SIZE; count++) {
      lastTile32x32[count] = currentPaletteArr[*pTileArr++];
    }
  }
  
  // on oscilloscope this one reduce few uS
  setSqAddrWindow(posX, posY, TILE_32x32_WINDOW_SIZE);
#if USE_FSMC
  sendData16_Arr_FSMC(lastTile32x32, TILE_ARR_32X32_SIZE);
#else
  sendData16_Fast_DMA1_SPI1(lastTile32x32, TILE_ARR_32X32_SIZE);
#endif
}
#endif /* STM32F10X_HD */

// -------------------------------------------------------- //

void drawBackgroundMap(void)
{
  uint8_t tileX, tileY;
  uint16_t tileMapCount =0;
  
  struct tile_t {
    uint16_t posX;
    uint16_t posY;
    uint8_t tileNum;
  } tile;
  
  for(tileY=0; tileY < BACKGROUND_SIZE_H; tileY++) {
    for(tileX=0; tileX < BACKGROUND_SIZE_W; tileX++) {
      
      tile.posX = ( tileX * TILE_8_BASE_SIZE );
      tile.posY = ( tileY * TILE_8_BASE_SIZE );
      tile.tileNum = mainBackGround[tileMapCount];
      
      drawTile8x8(&tile);
      //printChar(tile.tileNum+48);
      ++tileMapCount;
    }
  }
  //setCursor(0,0);
}
