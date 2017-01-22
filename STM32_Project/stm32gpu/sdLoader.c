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
#include <gfxDMA.h>
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
#define MAX_SD_INIT_ATTEMPTS 30 // yep, sometimes need som much attempts...

//===========================================================================//

//uint8_t buffer[2 * BUFFPIXELCOUNT]; // pixel buffer (contains already formatted data for ILI9341 display)
uint8_t  sdbuffer[3 * BUFFPIXELCOUNT]; // pixel in buffer (R+G+B per pixel)
uint16_t lcdbuffer[BUFFPIXELCOUNT];  // pixel out buffer (16-bit per pixel)


const char bmpExtension[] = ".bmp"; // 0x00
const char palExtension[] = ".pal"; // 0x01
const char tleExtension[] = ".tle"; // 0x02
const char mapExtension[] = ".map"; // 0x03

const char *extensions[] = {bmpExtension, palExtension, 
                            tleExtension, mapExtension};

//===========================================================================//

FATFS SDfs;				/* File system object for each logical drive */
FIL File;				/* File objects */
FRESULT result;

//uint8_t buffer[50];

//#define BUFFPIXELCOUNT 320	// size of the buffer in pixels
//===========================================================================//

void init_sdCard(void)
{
  sd_spi_init(); // init SPI_2 for SD card
  
  print( T_MOUNT_SD );
  
  int8_t attempts = MAX_SD_INIT_ATTEMPTS;
  
  do {
    // mount drive immedetly
    result = f_mount(&SDfs, "0", 1);
    
    --attempts;
    if(attempts%3) { // make a little less dots..
      print(".");
    }
    
  } while((result != FR_OK) && (attempts));
  
  if(result == FR_OK) {
    print(T_OK);
    
  } else {
    print(T_FAIL);
  }
}

FRESULT openTleFile(void *pFileName, uint8_t extension)
{
  strcat( (char*)pFileName, extensions[extension] );
  result = f_open(&File, (char*)pFileName, FA_OPEN_EXISTING | FA_READ);
  
  return result;
}

void SDLoadTile8x8(void *fileName, uint8_t *pData)
{
  /*
  * This function load single Tile from Tileset located in SD card
  * and store loaded data to RAM
  *
  * fileName        - name of tile set array on SD card
  * tileSetW        - width in NUMBER of tiles in tile set
  * ramTileNum      - tile number in ram, where to store loaded tile
  * tileNum         - number of tile in tile set which must me loaded from SD card
  */
  
  UINT cnt;
  
  uint8_t count =0;
  uint8_t tileYcnt;
  
  uint16_t tileNumOffsetX = 0;
  uint16_t tileNewLineOffsetY = 0;
  uint16_t offset = 0;
  
  strcat( (char*)fileName, tleExtension );
  result = f_open(&File, (char*)fileName, FA_OPEN_EXISTING | FA_READ);
  
  if(result == FR_OK) {
    
    uint8_t tileSetW = *pData++;
    uint8_t ramTileNum = *pData++;
    uint8_t tileNum = *pData++;
    
    uint8_t *pTileArr = getArrTilePointer8x8(ramTileNum);
    
    tileNumOffsetX = (tileNum % tileSetW)*TILE_BASE_SIZE;     // start position
    tileNewLineOffsetY = tileSetW*TILE_BASE_SIZE;       // offset for new scanline
    
    offset = ((tileNum)  / (tileSetW)) * (tileSetW * TILE_ARR_8X8_SIZE);
    
    f_lseek (&File, offset+tileNumOffsetX);
    
    for(tileYcnt = 0; tileYcnt < TILE_BASE_SIZE; tileYcnt++) { // Y
      
      f_read(&File, &pTileArr[count], TILE_BASE_SIZE, &cnt);
      
      count += TILE_BASE_SIZE;
      offset += tileNewLineOffsetY;
      f_lseek (&File, offset+tileNumOffsetX);
    }
    
  } else {
    // TODO: add some error code when return
    return;
  }
  
  f_close(&File);
}

void SDLoadTileSet8x8(void *fileName, uint8_t *pData)
{
  /*
  * This function load selected region of tiles from Tileset located in SD card.
  * Tiles load from tileMin position, to tileMax position;
  * Store loaded data to RAM from ramTileBase to (ramTileBase + tileMax);
  *
  * fileName        - name of tile set array on SD card
  * tileSetW        - width in NUMBER of tiles in tile set
  * ramTileBase     - base tile number in ram, where to start store loaded tiles
  * tileMin         - base tile number in tile set where loading start
  * tileMax         - number of tiles in tile set which must be loaded from SD card
  */
  
  UINT cnt;
  
  uint8_t tileDataOffset =0;
  uint8_t tileYcnt;
  
  uint16_t tileNumOffsetX = 0;
  uint16_t tileNewLineOffsetY = 0;
  uint16_t offset = 0;
  
  strcat( (char*)fileName, tleExtension );
  result = f_open(&File, (char*)fileName, FA_OPEN_EXISTING | FA_READ);
  
  if(result == FR_OK) {
    
    uint8_t *pTileArr =0;
    uint8_t tileSetW = *pData++;
    uint8_t ramTileBase = *pData++;
    uint8_t tileMin = *pData++;
    uint8_t tileMax = *pData++;
    
    tileNewLineOffsetY = tileSetW*TILE_BASE_SIZE;       // offset for new scanline
    
    for(uint8_t tileCount=tileMin; tileCount < tileMax; tileCount++) {
      
      pTileArr = getArrTilePointer8x8(ramTileBase + tileCount);
      
      tileNumOffsetX = (tileCount % tileSetW)*TILE_BASE_SIZE;     // start position
      offset = ((tileCount)  / (tileSetW)) * (tileSetW * TILE_ARR_8X8_SIZE);
      
      f_lseek (&File, offset+tileNumOffsetX);
      
      for(tileYcnt = 0; tileYcnt < TILE_BASE_SIZE; tileYcnt++) { // Y
        
        f_read(&File, &pTileArr[tileDataOffset], TILE_BASE_SIZE, &cnt);
        
        tileDataOffset += TILE_BASE_SIZE;
        offset += tileNewLineOffsetY;
        
        f_lseek (&File, offset+tileNumOffsetX);
      }
      tileDataOffset =0;
    }
  } else {
    // TODO: add some error code when return
    return;
  }
  
  f_close(&File);
}


void SDLoadTileMap(void *fileName)
{
  strcat( (char*)fileName, mapExtension );
  result = f_open(&File, (char*)fileName, FA_OPEN_EXISTING | FA_READ);
    
  if(result == FR_OK) {
    
    UINT cnt;
    
    uint8_t *pTileMapArr = getMapArrPointer();
    uint16_t tileCount=0;
    
    for(uint8_t countH=0; countH < BACKGROUND_SIZE_H; countH++) {
      f_read(&File, &pTileMapArr[tileCount], BACKGROUND_SIZE_W, &cnt);
      tileCount += BACKGROUND_SIZE_W;
      
      f_lseek (&File, tileCount);
    }
  } else {
    // TODO: add some error code when return
    return;
  }
  
  f_close(&File);
}


void SDLoadPalette(void *fileName)
{
  UINT cnt;
  
  strcat( (char*)fileName, palExtension );
  result = f_open(&File, (char*)fileName, FA_OPEN_EXISTING | FA_READ);
  
  if(result == FR_OK) {
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
    
  } else {
    // TODO: add some error code when return
    return;
  }
  
  f_close(&File);
}


// These read 16- and 32-bit types from the SD card file.
uint16_t read16(FIL *f)
{
  UINT cnt;
  uint16_t result16 =0;
  uint8_t result8[2];
  
  f_read (f, (char*)result8, 2, &cnt);
  
  result16 = (uint16_t)((result8[1] <<8) | result8[0]);
  return result16;
}

uint32_t read32(FIL *f)
{
  UINT cnt;
  union uResult_t {
    uint8_t resultArr[4];
    uint32_t result32;
  } uResult;
  
  f_read (f, (char*)uResult.resultArr, 4, &cnt);
  
  return uResult.result32;
}

void drawBMP24(uint16_t w, uint16_t h, uint32_t bmpImageoffset)
{
  UINT cnt;
  uint8_t  r, g, b;
  
  f_lseek (&File, bmpImageoffset);

  for (uint16_t row = 0; row < h; row++) { // For each scanline...
    
    for (uint16_t col = 0; col < w; col += BUFFPIXELCOUNT) {
      // read pixels into the buffer
      f_read(&File, sdbuffer, 3 * BUFFPIXELCOUNT, &cnt);
      
      // convert color
      for (uint16_t p = 0; p < BUFFPIXELCOUNT; p++) {
        b = sdbuffer[3 * p];
        g = sdbuffer[3 * p + 1];
        r = sdbuffer[3 * p + 2];
        lcdbuffer[p] = convRGBto565(r, g, b);
      }
      // push buffer to TFT
#if USE_FSMC
      sendData16_Arr_FSMC(lcdbuffer, BUFFPIXELCOUNT);
#else
      sendData16_Fast_DMA1_SPI1(lcdbuffer, BUFFPIXELCOUNT);
#endif
    }
  }
}

#if 0
void drawBMP16()
{
  
  f_lseek (&File, headerSize+1); // skip header
  
  uint32_t totalPixels = bmpWidth*bmpHeight;
  uint16_t numFullBufferRuns = totalPixels / BUFFPIXELCOUNT;
  for (uint32_t p = 0; p < numFullBufferRuns; p++) {
    // read pixels into the buffer
    f_read(&File, buffer, 2*BUFFPIXELCOUNT, &cnt);
    
    //for(uint16_t pxCount=0; pxCount <BUFFPIXELCOUNT-1; pxCount++) {
    //lcdbuffer[pxCount] = ( buffer[pxCount]<<8 | (buffer[pxCount+1]));
    //}
    
    // push them to the diplay
    sendData16_Fast_DMA1_SPI1(buffer, BUFFPIXELCOUNT);
  }
  
  // render any remaining pixels that did not fully fit the buffer
  uint32_t remainingPixels = totalPixels % BUFFPIXELCOUNT;
  if (remainingPixels > 0)
  {
    f_read(&File, buffer, 2*remainingPixels, &cnt);
    
    //for(uint16_t pxCount=0; pxCount <BUFFPIXELCOUNT-1; pxCount++) {
    // lcdbuffer[pxCount] = ( buffer[pxCount]<<8 | (buffer[pxCount+1]));
    //}
    
    sendData16_Fast_DMA1_SPI1(buffer, remainingPixels);
  }
}
#endif

void SDPrintBMP(uint16_t x, uint16_t y, void *fileName)
{
  int32_t bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint8_t  headerSize;
  uint32_t bmpImageoffset;        // Start of image data in file
  uint16_t w, h;
  
  if ((x >= width()) || (y >= height())) return;
  
  strcat( (char*)fileName, bmpExtension );
  result = f_open(&File, (char*)fileName, FA_OPEN_EXISTING | FA_READ);
  
  if(result == FR_OK) {
    // Parse BMP header
    if (read16(&File) == 0x4D42) { // BMP signature; 0x00
      
      (void)read32(&File); // files size; 0x02
      (void)read32(&File); // Read & ignore creator bytes; 0x06, 0x08
      
      bmpImageoffset = read32(&File); // Start of image data; 0x0A
      
      // Read bcSize ( biSize, bV4Size, bV5Size )
      headerSize = read32(&File);
      
      if(headerSize > 0xC) { // 3, 4, and 5 version
        bmpWidth = read32(&File);   // 0x12
        bmpHeight = read32(&File);  // 0x16
      } else { // CORE version
        bmpWidth = read16(&File);   // 0x12
        bmpHeight = read16(&File);  // 0x14
      }
      
      if (read16(&File) == 1) { // must be '1'; 0x16 or 0x1A
        bmpDepth = read16(&File); // bits per pixel; 0x18 or 0x1C
        
        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
        }
        
        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if ((x + w - 1) >= width())  w = width() - x;
        if ((y + h - 1) >= height()) h = height() - y;
        
        // Set TFT address window to clipped image bounds
        setAddrWindow(x, y, x + w - 1, y + h - 1);
        
        if (read32(&File) == 0) { // 0 = uncompressed
          if (bmpDepth == 24) {	// standard 24bit bmp
            drawBMP24(w, h, bmpImageoffset);
          } else {
            //print("Unsupported Bit Depth.");
          }
        } else { // 1 = comressed
#if 0
          if (bmpDepth == 16) {	// 565 format
            drawBMP16();
          }
#endif 
        }
      }
    }
  } 
  
  f_close(&File);
}
