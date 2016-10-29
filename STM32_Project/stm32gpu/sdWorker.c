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

#include <ff.h>
#include <diskio.h>
#include <sdcard_spi.h>

#include "sdWorker.h"
#include "gpuTiles.h"


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
  
  // mount drive immedetly
  result = f_mount(&SDfs, "0", 1);
  
  if(result == FR_OK) {
    print(T_OK);

  } else {
    print(T_FAIL);
  }
}

// tileSetArrName       - name of tile set array on SD card
// tileSetW             - width in NUMBER of tiles in tile set
// ramTileNum           - tile number in ram, where to store loaded tile
// tileNum              - number of tile in tile set which must me loaded from SD card
void SDLoadTileFromSet8x8(uint8_t *tileSetArrName, uint8_t tileSetW, uint8_t ramTileNum, uint8_t tileNum)
//void SDLoadTileFromSet8x8(uint8_t *tileSetArrName, tileParam_t *params)
{
 /*
  * This function load single Tile from Tileset located in SD card
  * and store loaded data to RAM
  */
  
  UINT cnt;
  
  uint8_t count =0;
  uint8_t tileYcnt;
  
  uint16_t tileNumOffsetX = 0;
  uint16_t tileNewLineOffsetY = 0;
  uint16_t offset = 0;
  
  result = f_open(&File, (char*)tileSetArrName, FA_OPEN_EXISTING | FA_READ);
  
  if(result == FR_OK) {
    
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

// tileSetArrName       - name of tile set array on SD card
// tileSetW             - width in NUMBER of tiles in tile set
// ramTileBase          - base tile number in ram, where to start store loaded tiles 
// tileMax              - number of tiles in tile set which must me loaded from SD card
void SDLoadTileSet8x8(uint8_t *tileSetArrName, uint8_t tileSetW, uint8_t ramTileBase, uint8_t tileMax)
//void SDLoadTileSet8x8(uint8_t *tileSetArrName, tileParam_t *params)
{
 /*
  * This function load tileMax tiles from Tileset at 0 position,
  * and located in SD card;
  * Store loaded data to RAM from ramTileBase to (ramTileBase + tileMax);
  */
  
  UINT cnt;
  
  uint8_t tileDataOffset =0;
  uint8_t tileYcnt;
  
  uint16_t tileNumOffsetX = 0;
  uint16_t tileNewLineOffsetY = 0;
  uint16_t offset = 0;
  
  result = f_open(&File, (char*)tileSetArrName, FA_OPEN_EXISTING | FA_READ);
  
  if(result == FR_OK) {
    
    uint8_t *pTileArr =0;
    
    tileNewLineOffsetY = tileSetW*TILE_BASE_SIZE;       // offset for new scanline
    
    for(uint8_t tileCount=0; tileCount < tileMax; tileCount++) {
      
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

// tileSetArrName       - name of tile set array on SD card
// tileSetW             - width in NUMBER of tiles in tile set
// ramTileBase          - base tile number in ram, where to start store loaded tiles
// tileMin              - base tile number in tile set whee loadindg start
// tileMax              - number of tiles in tile set which must be loaded from SD card
void SDLoadRegionOfTileSet8x8(uint8_t *tileSetArrName, uint8_t tileSetW, uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax)
//void SDLoadRegionOfTileSet8x8(uint8_t *tileSetArrName, tileParam_t *params)
{
 /*
  * This function load selected region of tiles from Tileset located in SD card.
  * Tiles load from tileMin position, to tileMax position;
  * Store loaded data to RAM from ramTileBase to (ramTileBase + tileMax);
  */
  
  UINT cnt;
  
  uint8_t tileDataOffset =0;
  uint8_t tileYcnt;
  
  uint16_t tileNumOffsetX = 0;
  uint16_t tileNewLineOffsetY = 0;
  uint16_t offset = 0;
  
  result = f_open(&File, (char*)tileSetArrName, FA_OPEN_EXISTING | FA_READ);
  
  if(result == FR_OK) {
    
    uint8_t *pTileArr =0;
    
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


void SDLoadTileMap(uint8_t *tileMapArrName)
{
  result = f_open(&File, (char*)tileMapArrName, FA_OPEN_EXISTING | FA_READ);
  
  if(result == FR_OK) {
    
    UINT cnt;
    
    uint8_t *pTileMapArr = getMapArrPointer();
    uint16_t tileCount=0;
    
#if 1
    for(uint8_t countH=0; countH < BACKGROUND_SIZE_H; countH++) {
      f_read(&File, &pTileMapArr[tileCount], BACKGROUND_SIZE_W, &cnt);
      tileCount += BACKGROUND_SIZE_W;
      
      f_lseek (&File, tileCount);
    }
#endif
  } else {
    // TODO: add some error code when return
    return;
  }
  
  f_close(&File);
}

#if 0
// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(FIL *f)
{
  UINT cnt;
  uint16_t result =0;
  
  f_read (f, (uint8_t*)result, 2, &cnt);
  
  return result;
}

uint32_t read32(FIL *f)
{
  UINT cnt;
  uint32_t result =0;
  
  f_read (f, (uint8_t*)result, 4, &cnt);
  
  return result;
}

void printSdBMP(uint16_t x, uint16_t y, const char* name)
{
  FIL   bmpFile;
  UINT cnt;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint8_t	 headerSize;
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;     // Not always = bmpWidth; may have padding
  uint32_t fileSize;
  bool  goodBmp = false;       // Set to true on valid header parse
  bool  flip = true;        // BMP is stored bottom-to-top
  uint16_t w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime;
  
  if ((x >= width()) || (y >= height())) return;
  
  
  print("open file... ");
  result = f_open(&bmpFile, (char*)name, FA_OPEN_EXISTING | FA_READ);
  
  if(result == FR_OK) {
    print("ok.\n");

  } else {
    print("fail.\n");
    return;
  }
  
  // Parse BMP header
  if (read16(&bmpFile) == 0x4D42) { // BMP signature
    fileSize = read32(&bmpFile);

    (void)read32(&bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(&bmpFile); // Start of image data

    // Read DIB header
    headerSize = read32(&bmpFile);

    bmpWidth = read32(&bmpFile);
    bmpHeight = read32(&bmpFile);
    if (read16(&bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(&bmpFile); // bits per pixel

      if (read32(&bmpFile) == 0) { // 0 = uncompressed
      
        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip = false;
        }
        
        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if ((x + w - 1) >= width())  w = width() - x;
        if ((y + h - 1) >= width()) h = width() - y;
        
        // Set TFT address window to clipped image bounds
        tftSetAddrWindow(x, y, x + w - 1, y + h - 1);
        
        if (bmpDepth == 16) {	//565 format
        
          goodBmp = true; // Supported BMP format -- proceed!
          
          uint8_t buffer[2 * BUFFPIXELCOUNT]; // pixel buffer (contains already formatted data for ILI9341 display)
          
          f_lseek (&bmpFile, 54); //skip header
          
          uint32_t totalPixels = bmpWidth*bmpHeight;
          uint16_t numFullBufferRuns = totalPixels / BUFFPIXELCOUNT;
          for (uint32_t p = 0; p < numFullBufferRuns; p++) {
            // read pixels into the buffer
            f_read(&bmpFile, buffer, 2 * BUFFPIXELCOUNT, &cnt);
            // push them to the diplay
            //tft.pushColors565(buffer, 0, 2 * BUFFPIXELCOUNT);
            sendArr16_SPI1(buffer, 2 * BUFFPIXELCOUNT);
          }
          
          // render any remaining pixels that did not fully fit the buffer
          uint32_t remainingPixels = totalPixels % BUFFPIXELCOUNT;
          if (remainingPixels > 0)
          {
            f_read(&bmpFile, buffer, 2 * remainingPixels, &cnt);
            //tft.pushColors565(buffer, 0, 2 * remainingPixels);
            sendArr16_SPI1(buffer, 2 * remainingPixels);
          }
          
        } else if (bmpDepth == 24) {	// standard 24bit bmp
          
          goodBmp = true; // Supported BMP format -- proceed!
          uint16_t bufferSize = min(w, BUFFPIXELCOUNT);
          uint8_t  sdbuffer[3 * bufferSize]; // pixel in buffer (R+G+B per pixel)
          uint16_t lcdbuffer[bufferSize];  // pixel out buffer (16-bit per pixel)
          
          // BMP rows are padded (if needed) to 4-byte boundary
          rowSize = (bmpWidth * 3 + 3) & ~3;
          
          for (row = 0; row < h; row++) { // For each scanline...
            // Seek to start of scan line.  It might seem labor-
            // intensive to be doing this on every line, but this
            // method covers a lot of gritty details like cropping
            // and scanline padding.  Also, the seek only takes
            // place if the file position actually needs to change
            // (avoids a lot of cluster math in SD library).
            
            if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
              pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
            else     // Bitmap is stored top-to-bottom
              pos = bmpImageoffset + row * rowSize;
            if (bmpFile.curPosition() != pos) { // Need seek?
              f_lseek (&bmpFile, pos);
            }
            
            for (col = 0; col < w; col += bufferSize) {
              // read pixels into the buffer
              f_read(&bmpFile, sdbuffer, 3 * bufferSize, , &cnt);
              
              // convert color
              for (int p = 0; p < bufferSize; p++) {
                b = sdbuffer[3 * p];
                g = sdbuffer[3 * p + 1];
                r = sdbuffer[3 * p + 2];
                lcdbuffer[p] = color565(r, g, b);
              }
              // push buffer to TFT
              //tft.pushColors(lcdbuffer, 0, bufferSize);
              sendArr16_SPI1(lcdbuffer, bufferSize);
            }
            
            // render any remaining pixels that did not fully fit the buffer
            uint16_t remainingPixels = w % bufferSize;
            if (remainingPixels > 0) {
              f_read(&bmpFile, sdbuffer, 3 * remainingPixels, , &cnt);
              
              for (int p = 0; p < remainingPixels; p++) {
                b = sdbuffer[3 * p];
                g = sdbuffer[3 * p + 1];
                r = sdbuffer[3 * p + 2];
                lcdbuffer[p] = color565(r, g, b);
              }
              
              //tft.pushColors(lcdbuffer, 0, remainingPixels);
              sendArr16_SPI1(lcdbuffer, remainingPixels);
            }
          }
        } else {
          //progmemPrint(PSTR("Unsupported Bit Depth."));
        }
        
      }
    }
  }
  
  f_close(&bmpFile);
}
#endif

// -------------  TO DO: ---------- //
/*
*
*
*
*
*       MAKE SOME MAGIC
*            HERE
*
*
*
*/
// -------------------------------- //
