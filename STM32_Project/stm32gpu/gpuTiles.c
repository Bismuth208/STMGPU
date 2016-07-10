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
static uint8_t tileArr8x8[TILES_NUM_8x8][TILE_ARR_8X8_SIZE] = {0, 0};
// 2048 ( if 16x128 )
static uint8_t tileArr8x16[TILES_NUM_8x16][TILE_ARR_8X16_SIZE] = {0, 0};
// 2048 ( if 8x256 )
static uint8_t tileArr16x16[TILES_NUM_16x16][TILE_ARR_16X16_SIZE] = {0, 0};

// total: 896
static uint16_t lastTile8x8[TILE_ARR_8X8_SIZE];       // 128
static uint16_t lastTile8x16[TILE_ARR_8X16_SIZE];     // 256
static uint16_t lastTile16x16[TILE_ARR_16X16_SIZE];   // 512

// on screen tile map background
// total: 960 ( if 20x16 )
static uint8_t rearBackGround[BACKGROUND_SIZE_W][BACKGROUND_SIZE_H] = {0, 0};
static uint8_t middleBackGround[BACKGROUND_SIZE_W][BACKGROUND_SIZE_H] = {0, 0};
static uint8_t frontBackGround[BACKGROUND_SIZE_W][BACKGROUND_SIZE_H] = {0, 0};


static uint16_t *pLastTileArr;

const uint16_t *pCurrentPalette;

// -------------------------------------------------------- //

void setCurrentPalette(const uint16_t *pNewPalette)
{
  pCurrentPalette = pNewPalette;
}

/*
void drawBMP_RLE_PGR(int16_t x, int16_t y, uint8_t w, uint8_t h,
  const uint8_t *colorInd, const uint16_t *colorTable, uint16_t sizePic) 
{
  // Same as drawBMP_BG_PGR(), but, this is used
  // when need maximum pic compression,
  // and you can waste some CPU resources for it;
  // It use very simple RLE compression;
  // Also draw background color;

  uint16_t count = 0;
  uint16_t repeatColor;
  uint8_t tmpInd, repeatTimes;

  tftSetAddrWindow(x, y, x+w-1, y+h-1);

  while(count < sizePic ) {  // compressed pic size!
    // get color index or repeat times
    tmpInd = colorInd[count];
    
    if(~tmpInd & 0x80) { // is it color index?
      repeatTimes = 1;
    } else {   // nope, just repeat color
      repeatTimes = tmpInd - 0x80;
      // get previous color index to repeat
      tmpInd = colorInd[count - 1];
    }

    // get color from colorTable by tmpInd color index
    repeatColor = colorTable[tmpInd];
    
    do {
      --repeatTimes;
      sendData16_SPI1(repeatColor);
    } while (repeatTimes);

    ++count;
  }
  DISABLE_DATA(); // release Chip Select
}
*/

void draw_PIC_RLE(int16_t x, int16_t y, uint8_t w, uint8_t h,
                     const uint8_t *colorInd, uint16_t sizePic) 
{
  // Same as drawBMP_BG_PGR(), but, this is used
  // when need maximum pic compression,
  // and you can waste some CPU resources for it;
  // It use very simple RLE compression;
  // Also draw background color;

  uint16_t count = 0;
  uint16_t repeatColor;
  uint8_t tmpInd, repeatTimes;

  tftSetAddrWindow(x, y, x+w-1, y+h-1);

  while(count < sizePic ) {  // compressed pic size!
    // get color index or repeat times
    tmpInd = colorInd[count];
    
    if(~tmpInd & 0x80) { // is it color index?
      repeatTimes = 1;
    } else {   // nope, just repeat color
      repeatTimes = tmpInd - 0x80;
      // get previous color index to repeat
      tmpInd = colorInd[count - 1];
    }

    // get color from colorTable by tmpInd color index
    repeatColor = pCurrentPalette[tmpInd];
    /*
    if(repeatTimes > 10) {
      while(DMA1_SPI1_busy());
      SET_TFT_CS_LOW;
      fillColor_DMA1_SPI1(repeatTimes, repeatColor);
      repeatTimes =0;
    } else {
*/
      do {
        --repeatTimes;
#if USE_FSMC
        *(uint16_t *) (LCD_FSMC_DATA) = repeatColor;
#else
        sendData16_SPI1(repeatColor);
#endif
      } while (repeatTimes);

    //}

    ++count;
  }
  DISABLE_DATA(); // release Chip Select
}

// -------------------------------------------------------- //

void addTile8x8(uint8_t tileNum, uint8_t *pTile)
{
  for(uint16_t count =0; count < TILE_ARR_8X8_SIZE; count++) {
    tileArr8x8[tileNum][count] = pTile[count];
  }
  //memcpy(tileArr8x8[tileNum], pTile, TILE_ARR_8X8_SIZE);
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
  uint8_t colorTileIdx;
  
  // first set addr window (this is protection for DMA buffer data)
  tftSetAddrWindow(posX, posY, posX+7, posY+7);
  
  //pLastTileArr = lastTile8x8;
  
#if USE_DMA
  for(uint16_t count =0; count < TILE_ARR_8X8_SIZE; count++) {
    // convert colors from current palette to RGB565 color space
    colorTileIdx = tileArr8x8[tileNum][count];
    lastTile8x8[count] = pCurrentPalette[colorTileIdx];
  }
  
  sendData16_DMA1_SPI1(TILE_ARR_8X8_SIZE, lastTile8x8);
#else
  
  for(uint16_t count =0; count < TILE_ARR_8X8_SIZE; count++) {
    // convert colors from current palette to RGB565 color space
    colorTileIdx = tileArr8x8[tileNum][count];
    //lastTile8x8[count] = pCurrentPalette[colorTileIdx];
    sendData16_SPI1(pCurrentPalette[colorTileIdx]);
  }
#endif
}

void repeatTile8x8(int16_t posX, int16_t posY)
{
  // first set addr window (this is protection for DMA buffer data)
  tftSetAddrWindow(posX, posY, posX+7, posY+7);
  
#if USE_DMA  
  sendData16_DMA1_SPI1(TILE_ARR_8X8_SIZE, lastTile8x8);
#else
  
  for(uint16_t count =0; count < TILE_ARR_8X8_SIZE; count++) {
    sendData16_SPI1(lastTile8x8[count]);
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
        pDrawTileFunc(posX, posY, tileNum);
      } else {
        pRepeatTileFunc(posX, posY);
      }
      
      tileNumPrev = tileNum;
      
      ++count;
    }
  }
}
