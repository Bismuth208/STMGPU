#include <stm32f10x.h>

#include <string.h>

#include <gfx.h>
#include <gfxDMA.h>
#include <spi.h>

#include "gpuTiles.h"
#include "nesPalette_ext.h"

// -------------------------------------------------------- //

// RAM size is limited and share to all type of sprites!
uint8_t tileArr8x8[TILES_NUM_8x8][TILE_ARR_8X8_SIZE] = {0, 0};
uint8_t tileArr16x16[TILES_NUM_16x16][TILE_ARR_16X16_SIZE] = {0, 0};

#ifdef STM32F10X_HD
uint8_t tileArr32x32[TILES_NUM_32x32][TILE_ARR_32X32_SIZE] = {0, 0};
#endif

// total: 640
uint16_t lastTile8x8[TILE_ARR_8X8_SIZE];       // 128
uint16_t lastTile16x16[TILE_ARR_16X16_SIZE];   // 512
#ifdef STM32F10X_HD
uint16_t lastTile32x32[TILE_ARR_32X32_SIZE];   // 2048
#endif

uint8_t  lastTileNum8x8 = 0xFF;          // last tile id in tileArrYxY

lastTile_t lastTileStruct16x16 = {0, 0};

// on screen tile map background
// total: 1200 ( if 40x30 )
uint8_t mainBackGround[BACKGROUND_SIZE] = {0};
//uint8_t middleBackGround[BACKGROUND_SIZE_W][BACKGROUND_SIZE_H] = {0, 0};
//uint8_t frontBackGround[BACKGROUND_SIZE_W][BACKGROUND_SIZE_H] = {0, 0};

// total: 512
uint16_t currentPaletteArr[USER_PALETTE_SIZE]; // user can specify max 256 colors

uint8_t palChanged =0; // status flag, need to redraw tiles

// -------------------------------------------------------- //

void loadDefaultPalette(void)
{
  for(uint8_t count=0; count <80; count++) { // 80 colors
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

uint8_t *getArrTilePointer16x16(uint8_t tileNum)
{
  return tileArr16x16[tileNum];
}

uint8_t *getMapArrPointer(void)
{
  return mainBackGround;
}

void drawTile8x8(void *tile)
{
  uint16_t *pTile = (uint16_t *)tile;
  
  int16_t posX = *pTile++;
  int16_t posY = *pTile++;
  uint8_t tileNum = *pTile++;
  
  // little trick, if tile same, just redraw it
  if(lastTileNum8x8 != tileNum) {
    
    uint8_t *pTileArr = &tileArr8x8[tileNum][0];
    lastTileNum8x8 = tileNum;
    
    for(uint16_t count =0; count < TILE_ARR_8X8_SIZE; count++) {
      // convert colors from current palette to RGB565 color space
      lastTile8x8[count] = currentPaletteArr[*pTileArr++];
    }
  }
  
  setSqAddrWindow(posX, posY, 7); // on oscilloscope this one remove few uS
  sendData16_Fast_DMA1_SPI1(lastTile8x8, TILE_ARR_8X8_SIZE);
}

void drawTile16x16(int16_t posX, int16_t posY, uint8_t tileNum)
{
  // little trick, if tile same, just redraw it
  if((lastTileStruct16x16.drawed == 0) || (lastTileStruct16x16.lasttileNum != tileNum)) {
    
    uint8_t colorTileIdx;
    
    lastTileStruct16x16.drawed = 1;
    lastTileStruct16x16.lasttileNum = tileNum;
    
    for(uint16_t count =0; count < TILE_ARR_16X16_SIZE; count++) {
      // convert colors from current palette to RGB565 color space
      colorTileIdx = tileArr16x16[tileNum][count];
      lastTile16x16[count] = currentPaletteArr[colorTileIdx];
    }
  }
  
  setSqAddrWindow(posX, posY, 15);
  sendData16_DMA1_SPI1(lastTile16x16, TILE_ARR_16X16_SIZE);
}


#ifdef STM32F10X_HD
void drawTile32x32(int16_t posX, int16_t posY, uint8_t tileNum)
{
  // little trick, if tile same, just redraw it
  if((lastTileStruct32x32.drawed == 0) || (lastTileStruct32x32.lasttileNum != tileNum)) {
    
    uint8_t colorTileIdx;
    
    lastTileStruct32x32.drawed = 1;
    lastTileStruct32x32.lasttileNum = tileNum;
    
    for(uint16_t count =0; count < TILE_ARR_32X32_SIZE; count++) {
      // convert colors from current palette to RGB565 color space
      colorTileIdx = tileArr32x32[tileNum][count];
      lastTileStruct32x32.pLastTileArr[count] = currentPaletteArr[colorTileIdx];
    }
  }
  
  setSqAddrWindow(posX, posY, 31);
  sendData16_DMA1_SPI1(lastTileStruct32x32.pLastTileArr, TILE_ARR_16X16_SIZE);
}
#endif

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
      
      tile.posX = ( tileX * TILE_BASE_SIZE );
      tile.posY = ( tileY * TILE_BASE_SIZE );
      
      tile.tileNum = mainBackGround[tileMapCount];
      
      //printChar(tileNum+48);
      
      drawTile8x8(&tile);
      ++tileMapCount;
    }
  }
  
  //setCursor(0,0);
}
