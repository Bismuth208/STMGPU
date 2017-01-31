#include <stdlib.h>
#include <string.h>

#include <systicktimer.h>
#include <uart.h>
#include <STMsGPU_c.h>

#include "gpuTest.h"

// --------------------------------------------------------- //

#define TFT_W gpuWidth()
#define TFT_H gpuHeight()

#define TEST_SAMPLE_SIZE    12000
#define TEST_SAMPLE_SCREENS 80 // this is equal to 24000 tiles

// --------------------------------------------------------- //

static uint16_t nextInt = 9; // absolutely randomised value

void (*pArrExecGFXFunc[])(void) = {
    testDrawTiles,
    drawRamTileSet8x8,
    fillScreenByTiles,
  };

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
  /* load MAX_TILES tiles to GPU's RAM at RAM_BASE position in it's RAM,
  *  from tileFileName, located on SD card attached to STM32 GPU;
  *  TILE_SET_W - is width of tileSet in tiles;
  *  TLE_START - nunber of tile in tileset from which tiles will be loaded;
  *  File name must respond to 8.3 name system,
  *  8 chars max for filename, 3 chars max for file extension.
  *  sGPU add *.tle extension automatically.
  */
  gpuSDLoadTileSet8x8(tileFileName, TILE_SET_W-1, RAM_BASE, TLE_START, MAX_TILES);
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

// Draw on screen limited range of tiles
// on screen must apear square 10x8 tiles
void drawRamTileSet8x8(void)
{
  int16_t posX, posY;
  uint8_t count =0;
  
  // draw MAX_TILES tiles
  for(uint8_t countY =0; countY <TILE_SET_W; countY++) {
    for(uint8_t countX =0; countX <TILE_SET_H; countX++) {
      
      // 50 is default position in px on screen
      posX = (50 + ( countX * TLE_8X8_SIZE ));
      posY = (50 + ( countY * TLE_8X8_SIZE ));
      
      gpuDrawTile8x8(posX, posY, count);
      
      ++count;
    }
  }
}

// Fill whole screen by random tiles
void fillScreenByTiles(void)
{
  uint8_t xStep, yStep;
  uint8_t maxXSize, maxYSize;

  maxXSize = TFT_W / TLE_8X8_SIZE;
  maxYSize = TFT_H / TLE_8X8_SIZE;

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

  //BAUD_SPEED_9600 = 9600
  //BAUD_SPEED_57600 = 57600
  //BAUD_SPEED_115200 = 115200
  //BAUD_SPEED_1M = 1000000
  sync_gpu(BAUD_SPEED_1M);  // establish connection

  gpuLoadTiles();

  uint8_t testsCount = FUNC_TO_TEST_COUNT;

  for(;;) {

    for (uint8_t count = 0; count < testsCount; count++) {
      pArrExecGFXFunc[count]();  // exec test function

       _delayMS(1000);  // actual 500 // little delay to see what happend on screen
      gpuFillScreen(COLOR_BLACK);  // clear screen by black color
    }
  }

  return 0;
}
