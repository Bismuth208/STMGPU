#include <stdlib.h>
#include <string.h>

#include <systicktimer.h>
#include <uart.h>
#include <STMsGPU_c.h>

#include "gpuTest.h"

/* BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
 * If your board have only ONE hardware serial,
 * then you MUST use softWare serial instead!
 * moreover arduino libs here totally not supported!
 */

// --------------------------------------------------------- //
void (*pArrTestFunc[])(void) = {
    testDrawTiles,
    drawRamTileSet8x8,
    fillScreenByTiles,
};

static uint16_t nextInt = 9; // absolutely randomised value

// --------------------------------------------------------- //

uint16_t randNum(void)
{
  nextInt ^= nextInt >> 4;
  nextInt ^= nextInt << 7;
  nextInt ^= nextInt >> 9;
  nextInt = (nextInt * 214013 );
  return nextInt;
}

// --------------------------------------------------------- //

void gpuLoadTiles(void)
{
  /* load MAX_TILES tiles to sGPU's RAM at RAM_BASE position in it's RAM,
  *  from tileFileName,
  *  located on SD card attached to STM32 sGPU
  *  TLE_START - nunber of tile in tileset from which tiles will be loaded
  *  file name must respond to 8.3 name system
  *  8 chars max for filename, 3 chars max for file extension
  *  sGPU add *.tle extension automatically
  */
  gpuLoadTileSet8x8(tileFileName, TILE_SET_W-1, RAM_BASE, TLE_START, MAX_TILES);
}

// --------------------------------------------------------- //

// draw random tile at random position
void testDrawTiles(void)
{
  uint16_t rndPosX, rndPosY;

   for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    rndPosX = randNum() % TFT_W;
    rndPosY = randNum() % TFT_H;
    
    // draw random tile form 0 to MAX_TILES, at random position
    gpuDrawTile8x8(rndPosX, rndPosY, randNum()%MAX_TILES);
  }
}

// Draw on screen limited range of tiles on screen
void drawRamTileSet8x8(void)
{
  int16_t posX, posY;
  uint8_t count =0;
  
  // draw TILE_SET_W*TILE_SET_H tiles
  for(uint8_t countY =0; countY <TILE_SET_W; countY++) {
    for(uint8_t countX =0; countX <TILE_SET_H; countX++) {
      
      // 50 is default position in px on screen
      posX = (50 + ( countX * TLE_8X8_SIZE ));
      posY = (50 + ( countY * TLE_8X8_SIZE ));
      
      gpuDrawTile8x8(posX, posY, count++);
    }
  }
}

// Fill whole screen by random tiles
void fillScreenByTiles(void)
{
  uint8_t xStep, yStep;
  uint8_t maxXSize, maxYSize;

  maxXSize = TFT_W / TLE_8X8_SIZE; // \__ calculate how much
  maxYSize = TFT_H / TLE_8X8_SIZE; // /   tiles in x and y axis

  for (uint8_t i = 0; i < TEST_SAMPLE_SCREENS; i++) {
    for (yStep = 0; yStep < maxYSize; yStep++) {
      for (xStep = 0; xStep < maxXSize; xStep++) {

        // draw random tile form 0 to MAX_TILES
        gpuDrawTile8x8(xStep*TLE_8X8_SIZE, yStep*TLE_8X8_SIZE, randNum()%MAX_TILES);
      }
    } 
  }
}

// ---------------------------------------------------------- //
int main(void)
{
  // it`s enable timer0 on atmega328p;
  // need for delays;
  initSysTickTimer();

  // different speeds can be found in library STMsGPU_c.h
  sync_gpu(BAUD_SPEED_1M);  // BAUD_SPEED_1M = 1,000,000 bod/s

  gpuLoadTiles();

  uint8_t testsCount = FUNC_TO_TEST_COUNT;

  for(;;) {
    for (uint8_t count = 0; count < testsCount; count++) {
      pArrTestFunc[count]();  // exec test function

      _delayMS(1000);  // actual 500 // little delay to see what happend on screen
      gpuFillScreen(COLOR_BLACK);  // clear screen by black color
    }
  }

  return 0;
}
