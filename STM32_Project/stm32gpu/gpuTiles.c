#include <string.h>

#include <gfx.h>
#include <gfxDMA.h>
#include <spi.h>

#include "gpuTiles.h"
#include "nesPalette_ext.h"

// -------------------------------------------------------- //

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned int *)(addr))

#define PGRW_U16(a, b) pgm_read_word(&(a[b]))

// -------------------------------------------------------- //

// RAM size is limited and share to all type of sprites!
// total: 10240
// 6144 ( if 96x64 )
uint8_t tileArr8x8[TILES_NUM_8x8][TILE_ARR_8X8_SIZE] = {0, 0};
// 2048 ( if 16x128 )
uint8_t tileArr8x16[TILES_NUM_8x16][TILE_ARR_8X16_SIZE] = {0, 0};
// 2048 ( if 8x256 )
uint8_t tileArr16x16[TILES_NUM_16x16][TILE_ARR_16X16_SIZE] = {0, 0};

// total: 896
uint16_t lastTile8x8[TILE_ARR_8X8_SIZE];       // 128
uint16_t lastTile8x16[TILE_ARR_8X16_SIZE];     // 256
uint16_t lastTile16x16[TILE_ARR_16X16_SIZE];   // 512

lastTile_t lastTileStruct8x8 = {0, 0, lastTile8x8};

// on screen tile map background
// total: 960 ( if 20x16 )
uint8_t rearBackGround[BACKGROUND_SIZE_W][BACKGROUND_SIZE_H] = {0, 0};
uint8_t middleBackGround[BACKGROUND_SIZE_W][BACKGROUND_SIZE_H] = {0, 0};
uint8_t frontBackGround[BACKGROUND_SIZE_W][BACKGROUND_SIZE_H] = {0, 0};

//static uint16_t *pLastTileArr;

const uint16_t *pCurrentPalette;

// -------------------------------------------------------- //

void setCurrentPalette(const uint16_t *pNewPalette)
{
  pCurrentPalette = pNewPalette;
}

// -------------------------------------------------------- //

// return the pointer to given tile array number
uint8_t *getArrTilePointer8x8(uint8_t tileNum)
{
  return tileArr8x8[tileNum];
}

void addTile8x8(uint8_t tileNum, uint8_t *pTile)
{
  memcpy(tileArr8x8[tileNum], pTile, TILE_ARR_8X8_SIZE);
  //memcpy32(tileArr8x8[tileNum], pTile, TILE_ARR_8X8_SIZE_32);
}

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

void drawTile8x8(int16_t posX, int16_t posY, uint8_t tileNum)
{
  // first set addr window (this is protection for DMA buffer data)
  tftSetAddrWindow(posX, posY, posX+7, posY+7);
  
  // little trick, if tile same, just redraw it
  if((lastTileStruct8x8.drawed) && (lastTileStruct8x8.lasttileNum == tileNum)) {
#if USE_DMA
    sendData16_DMA1_SPI1(lastTileStruct8x8.pLastTileArr, TILE_ARR_8X8_SIZE);
#else
    sendArr16_SPI1(lastTileStruct8x8.pLastTileArr, TILE_ARR_8X8_SIZE);
#endif
    
  } else {
    uint8_t colorTileIdx;
    
    lastTileStruct8x8.drawed = 1;
    lastTileStruct8x8.lasttileNum = tileNum;
    
    for(uint16_t count =0; count < TILE_ARR_8X8_SIZE; count++) {
      // convert colors from current palette to RGB565 color space
      colorTileIdx = tileArr8x8[tileNum][count];
      lastTileStruct8x8.pLastTileArr[count] = pCurrentPalette[colorTileIdx];
    }
#if USE_DMA  
    sendData16_DMA1_SPI1(lastTileStruct8x8.pLastTileArr, TILE_ARR_8X8_SIZE);
#else
    sendArr16_SPI1(lastTileStruct8x8.pLastTileArr, TILE_ARR_8X8_SIZE);
#endif
    
  }
}

void repeatTile8x8(int16_t posX, int16_t posY)
{
  // first set addr window (this is protection for DMA buffer data)
  tftSetAddrWindow(posX, posY, posX+7, posY+7);
  
#if USE_DMA  
  sendData16_DMA1_SPI1(lastTileStruct8x8.pLastTileArr, TILE_ARR_8X8_SIZE);
#else
  
  for(uint16_t count =0; count < TILE_ARR_8X8_SIZE; count++) {
    sendData16_SPI1(lastTileStruct8x8.pLastTileArr[count]);
  }
#endif
}

// -------------------------------------------------------- //

void loadTileSet(uint8_t tileSetSize, uint8_t tileSetW, const uint8_t *pTileSet, uint8_t tileSize)
{
  void (*pLoadTileFunc)(uint8_t, uint8_t, uint8_t *, const uint8_t *);
  
  if(tileSize == 1) {
    pLoadTileFunc = loadTile8x8;
  }/* else if(tileSize == 2) {
    pLoadTileFunc = loadTile8x16;
  } else if(tileSize == 3) {
    pLoadTileFunc = loadTile16x16;
  } */
  
  for(uint8_t count =0; count <= tileSetSize; count++) {
    pLoadTileFunc(count, tileSetW, tileArr8x8[count], pTileSet);
  }
}

void drawTileMap(int16_t x, int16_t y, const uint8_t *pTileMap, uint8_t tileSize)
{ 
  uint8_t tileX, tileY;
  uint8_t count =0;
  
  uint8_t tileNum =0, tileNumPrev=0;
  
  int16_t posX, posY;
  
  void (*pDrawTileFunc)(int16_t, int16_t, uint8_t);
  void (*pRepeatTileFunc)(int16_t, int16_t);
  
  if(tileSize == 1) {
    pDrawTileFunc = drawTile8x8;
    pRepeatTileFunc = repeatTile8x8;
  }/* else if(tileSize == 2) {
    pDrawTileFunc = drawTile8x16;
    pRepeatTileFunc = repeatTile8x16;
  } else if(tileSize == 3) {
    pDrawTileFunc = drawTile16x16;
    pRepeatTileFunc = repeatTile16x16;
  } */
  
  for(tileY=0; tileY < pTileMap[1]; tileY++) {
    for(tileX=0; tileX < pTileMap[0]; tileX++) {
      
      posX = (x + ( tileX * TILE_BASE_SIZE ));
      posY = (y + ( tileY * TILE_BASE_SIZE ));
      
      tileNum = pTileMap[2 + count];
      
      if(tileNumPrev == tileNum) {
        pRepeatTileFunc(posX, posY);
      } else {
        pDrawTileFunc(posX, posY, tileNum);
      }
      
      tileNumPrev = tileNum;
      
      ++count;
    }
  }
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
