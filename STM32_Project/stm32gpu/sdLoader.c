/*
*
* STM32_GPU Project
* Creation start: 15.07.2016
*
* Created by: Antonov Alexandr (Bismuth208)
*
*/

#include <string.h>

#include <gfx.h>
#if USE_FSMC
 #include <fsmcdrv.h>
#else
 #include <spi.h>
#endif

#include <ff.h>
#include <diskio.h>
#include <sdcard_spi.h>

#include "sdLoader.h"
#include "gpuTiles.h"

#define BUFFPIXELCOUNT 80
#define MAX_SD_INIT_ATTEMPTS 30 // yep, sometimes need so much attempts...

//===========================================================================//

uint8_t  sdbuffer[3 * BUFFPIXELCOUNT]; // pixel in buffer (R+G+B per pixel)
uint16_t lcdbuffer[BUFFPIXELCOUNT];  // pixel out buffer (16-bit per pixel)

//uint8_t buffer[50]; // for loaded text

//===========================================================================//

FATFS SDfs;				/* File system object for each logical drive */
FIL File;				/* File objects */
FRESULT result;

//===========================================================================//

void init_sdCard(void)
{
  print( T_MOUNT_SD );
  
  int8_t attempts = MAX_SD_INIT_ATTEMPTS; // how many times try to mount drive
  
  do {
    // mount drive immedetly
    result = f_mount(&SDfs, "0", 1);
    
    --attempts;
    if(attempts%3) { // make a little less dots..
      print("."); // show what init is in progress
    }
    
  } while((result != FR_OK) && (attempts)); // have attempts and not mounted
  
  if(result == FR_OK) {
    print(T_OK);
    
  } else {
    print(T_FAIL);
  }
}


FRESULT openSDFile(void *pFileName, const char *extension)
{
  strcat( (char*)pFileName, extension );
  result = f_open(&File, (char*)pFileName, FA_OPEN_EXISTING | FA_READ);
  
  return result;
}


// ---------------------- Tiles 8,16,32 -------------------------------- //
/*
 * is it possible to remove a half of that?
 * make loading tiles faster and smaller?
 * maybe make simpler math of pointing single tile?
 *
 * code below something like mind blow...
 * and it looks like a f@@@ing magic!
 * how i make it in the past!?
 */


pfGetFunc_t getFuncArrTilePointer(uint16_t tleBaseSize)
{
  switch(tleBaseSize)
  {
  case TILE_8_BASE_SIZE: {
    return getArrTilePointer8x8;
  } break;
  
  case TILE_16_BASE_SIZE: {
    return getArrTilePointer16x16;
  } break;
  
#ifdef STM32F10X_HD
  case TILE_32_BASE_SIZE: {
    return getArrTilePointer32x32;
  } break;
#endif
  }
  
  return 0;
}

void SDLoadTile(void *fileName, uint8_t *pData, uint16_t tleBaseSize, uint16_t tlesNumSize)
{
  /*
  * This function load single Tile from Tileset located in SD card
  * and store loaded data to RAM
  *
  * fileName        - name of tile set array on SD card
  * tileSetW        - width in NUMBER of tiles in tile set
  * ramTileBase     - tile number in ram, where to store loaded tile
  * tileMin         - number of tile in tileset which must me loaded from SD card
  */
  
  UINT cnt;
  uint16_t tleOffsetX = 0;
  uint16_t tleNewLineOffsetX = 0;
  
  tileParam_t *tilePar = (tileParam_t*)pData;
  
  // little RAM protection
  if(tilePar->ramTileBase > tlesNumSize) return;
  
  pfGetFunc_t pfGetArrTilePointer = getFuncArrTilePointer(tleBaseSize);
  
  uint8_t *pTileArr = pfGetArrTilePointer(tilePar->ramTileBase);
  
  if(openSDFile(fileName, T_TLE_SET_EXT_NAME) == FR_OK) {
    
    tleNewLineOffsetX = tilePar->tileSetW * tleBaseSize - tleBaseSize;
    
    // calculate x offset for start of scanline
    tleOffsetX = tilePar->tileMin * tleBaseSize;
    // set start of scanline; 2 - because tile is square
    tilePar->tileMin > tilePar->tileSetW ? f_lseek(&File, tleOffsetX*2) : f_lseek(&File, tleOffsetX);
    
    for(uint8_t tileYcnt=0; tileYcnt < tleBaseSize; tileYcnt++) {
      
      f_read(&File, pTileArr, tleBaseSize, &cnt); // read single scanline
      f_lseek(&File, f_tell(&File) + tleNewLineOffsetX); // make new scanline
      pTileArr += tleBaseSize;
    }
  } 
  f_close(&File);
  // TODO: add some error code when return
}

void SDLoadTileSet(void *fileName, uint8_t *pData, uint16_t tleBaseSize, uint16_t tlesNumSize)
{
  /*
  * This function load selected region of tiles from Tileset located in SD card.
  * Tiles load from tileMin position, to tileMax position;
  * Store loaded data to RAM from ramTileBase to (ramTileBase + tileMax);
  *
  * fileName        - name of tileset array on SD card
  * tileSetW        - width in NUMBER of tiles in tile set
  * ramTileBase     - base tile number in ram, where to start store loaded tiles
  * tileMin         - base tile number in tileset where loading start
  * tileMax         - number of tiles in tileset which must be loaded from SD card
  */
  
  UINT cnt;
  uint8_t tileYcnt;
  uint16_t tleOffsetX = 0;
  uint16_t tleNewLineOffsetX = 0;
  
  uint8_t *pTileArr =0;
  tileParam_t *tilePar = (tileParam_t*)pData;
  
  // little RAM protection
  if((tilePar->ramTileBase + tilePar->tileMax) > tlesNumSize) return;
  
  pfGetFunc_t pfGetArrTilePointer = getFuncArrTilePointer(tleBaseSize);
  
  if(openSDFile(fileName, T_TLE_SET_EXT_NAME) == FR_OK) {
    
    tleNewLineOffsetX = tilePar->tileSetW * tleBaseSize - tleBaseSize;

    for(uint8_t tileNum=tilePar->tileMin; tileNum < tilePar->tileMax; tileNum++) {
      
      // calculate x offset for start of scanline
      tleOffsetX = tileNum * tleBaseSize;
      // set start of scanline; 2 - because tile is square
      tileNum > tilePar->tileSetW ? f_lseek(&File, tleOffsetX*2) : f_lseek(&File, tleOffsetX);
      
      // get pointer to new tile in RAM
      pTileArr = pfGetArrTilePointer(tilePar->ramTileBase + tileNum);

      // load single tile to RAM
      for(tileYcnt = 0; tileYcnt < tleBaseSize; tileYcnt++) { // Y
        
        f_read(&File, pTileArr, tleBaseSize, &cnt); // read single scanline
        f_lseek(&File, f_tell(&File) + tleNewLineOffsetX); // make new scanline
        pTileArr += tleBaseSize;
      }
    }
  }
  f_close(&File);
  // TODO: add some error code when return
}
// --------------------------------------------------------------------- //


// --------------------- Tile Map -------------------------- //
void SDLoadTileMap(void *fileName)
{
  if(openSDFile(fileName, T_MAP_SET_EXT_NAME) == FR_OK) {
    
    UINT cnt;
    uint8_t *pTileMapArr = getMapArrPointer();
    
    for(uint8_t countH=0; countH < BACKGROUND_SIZE_H; countH++) {
      f_read(&File, pTileMapArr, BACKGROUND_SIZE_W, &cnt);
      pTileMapArr += BACKGROUND_SIZE_W;
    }
  } else {
    // TODO: add some error code when return
    return;
  }
  
  f_close(&File);
}
// --------------------------------------------------------- //


// --------------------- Palette --------------------------- //
void SDLoadPalette(void *fileName)
{
  if(openSDFile(fileName, T_PAL_SET_EXT_NAME) == FR_OK) {
    UINT cnt;
    uint8_t  r, g, b;
    uint16_t colors = f_size(&File) /3;
    
    if(colors <= 255) {
      
      f_read(&File, sdbuffer, f_size(&File), &cnt);
      
      for(uint16_t count =0; count<colors; count++) {
        r = sdbuffer[3 * count];
        g = sdbuffer[3 * count + 1];
        b = sdbuffer[3 * count + 2];
        currentPaletteArr[count] = convRGBto565(r, g, b);
      } 
    }
    
    palChanged = 1; // force 
    
  }
  f_close(&File);
  // TODO: add some error code when return
}
// --------------------------------------------------------- //



// ---------------- *.BMP pictures ------------------------- //
// These read 16- and 32-bit types from the SD card file.
uint16_t read16()
{
  UINT cnt;
  uint16_t result16 =0;
  uint8_t result8[2];
  
  f_read(&File, (char*)result8, 2, &cnt);
  
  result16 = (uint16_t)((result8[1] <<8) | result8[0]);
  return result16;
}

uint32_t read32()
{
  UINT cnt;
  union uResult_t {
    uint8_t resultArr[4];
    uint32_t result32;
  } uResult;
  
  f_read(&File, (char*)uResult.resultArr, 4, &cnt);
  
  return uResult.result32;
}

void drawBMP24(uint16_t w, uint16_t h, uint32_t bmpImageoffset)
{
  UINT cnt; // how much bytes really read
  uint8_t  r, g, b;
  
  f_lseek(&File, bmpImageoffset);

  for(uint16_t row = 0; row < h; row++) { // For each scanline...
    for(uint16_t col = 0; col < w; col += BUFFPIXELCOUNT) {
      // read pixels into the buffer
      f_read(&File, sdbuffer, 3 * BUFFPIXELCOUNT, &cnt);
      
      // convert color
      for(uint16_t p = 0; p < BUFFPIXELCOUNT; p++) {
        b = sdbuffer[3 * p];
        g = sdbuffer[3 * p + 1];
        r = sdbuffer[3 * p + 2];
        lcdbuffer[p] = convRGBto565(r, g, b);
      }
      // push buffer to TFT
      SEND_ARR16_FAST(lcdbuffer, BUFFPIXELCOUNT);
    }
  }
}

void drawBMP16(uint16_t w, uint16_t h, uint32_t bmpImageoffset)
{
  UINT cnt;
  
  f_lseek(&File, bmpImageoffset); // skip header
  
  for(uint16_t row = 0; row < h; row++) { // For each scanline...
    for(uint16_t col = 0; col < w; col += BUFFPIXELCOUNT) {
      // read pixels into the buffer
      f_read(&File, lcdbuffer, BUFFPIXELCOUNT, &cnt);
      
      // push buffer to TFT
      SEND_ARR16_FAST(lcdbuffer, BUFFPIXELCOUNT);
    }
  }
}

void SDPrintBMP(uint16_t x, uint16_t y, void *fileName)
{
  int32_t  bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint8_t  headerSize;
  uint32_t bmpImageoffset;        // Start of image data in file
  uint16_t w, h;
  
  if((x >= width()) || (y >= height())) return;
  
  if(openSDFile(fileName, T_BMP_SET_EXT_NAME) == FR_OK) {
    // Parse BMP header
    if(read16() == 0x4D42) { // BMP signature; 0x00
      
      (void)read32(); // files size; 0x02
      (void)read32(); // Read & ignore creator bytes; 0x06, 0x08
      
      bmpImageoffset = read32(); // Start of image data; 0x0A
      
      // Read bcSize ( biSize, bV4Size, bV5Size )
      headerSize = read32();
      
      if(headerSize > 0xC) { // 3, 4, and 5 version
        bmpWidth = read32();   // 0x12
        bmpHeight = read32();  // 0x16
      } else { // CORE version
        bmpWidth = read16();   // 0x12
        bmpHeight = read16();  // 0x14
      }
      
      if(read16() == 1) { // must be '1'; 0x16 or 0x1A
        bmpDepth = read16(); // bits per pixel; 0x18 or 0x1C
        
        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
        }
        
        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if ((x + w - 1) >= width())  w = width() - x;
        if ((y + h - 1) >= height()) h = height() - y;
        
        // Set TFT address window to clipped image bounds
        setAddrWindow(x, y, x + w - 1, y + h - 1);
        
        if(read32() == 0) { // 0 = uncompressed
          if(bmpDepth == 24) {	// standard 24bit bmp
            drawBMP24(w, h, bmpImageoffset);
          } else {
            //print("Unsupported Bit Depth.");
          }
        } else { // 1 = comressed
          if(bmpDepth == 16) {	// 565 format
            drawBMP16(w, h, bmpImageoffset);
          }
        }
      }
    }
  }
  f_close(&File);
}
// --------------------------------------------------------- //
