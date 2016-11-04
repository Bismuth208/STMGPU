#include <stdlib.h>
#include <string.h>

#include <systicktimer.h>

#include <uart.h>
#include <STMsGPU_c.h>

#include "gpuTest.h"

// --------------------------------------------------------- //

#define TFT_W tftWidth()
#define TFT_H tftHeight()

#define TEST_SAMPLE_SIZE    12000
#define TEST_SAMPLE_SCREENS 80 // this is equal to 24000 tiles

// --------------------------------------------------------- //

static uint16_t nextInt = 9;

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
  // load 90 tiles to GPU's RAM at 0 position in RAM,
  // from tileFileName,
  // located on SD card attached to STM32 GPU
  // 9 - is width of tileSet in tiles ( 9 tiles width == 72 pixels)
  // file name must respond to 8.3 name system
  // 8 chars max for filename, 3 chars max for file extension
  SDLoadTileSet8x8((const char*)tileFileName, 9, 0, 90);
}

// --------------------------------------------------------- //

// draw random tile at random position
void testDrawTiles(void)
{
  uint16_t rndPosX, rndPosY;

   for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    rndPosX = randNum() % TFT_W;
    rndPosY = randNum() % TFT_H;
    
    // draw random tile form 0 to 90, at random position
    drawTile8x8(rndPosX, rndPosY, randNum()%90);
  }
}

// Draw on screen limited range of tiles
// on screen must apear square 10x9 tiles
void drawRamTileSet8x8(void)
{
  int16_t posX, posY;
  uint8_t count =0;
  
  // draw 90 tiles
  for(uint8_t countY =0; countY <10; countY++) {
    for(uint8_t countX =0; countX <9; countX++) {
      
      posX = (50 + ( countX * 8 )); // \__ 50 is default position in px on screen
      posY = (50 + ( countY * 8 )); // /   8 is single tile size, each tile 8x8 in px
      
      drawTile8x8(posX, posY, count);
      
      ++count;
    }
  }
}

// Fill whole screen by random tiles
void fillScreenByTiles(void)
{
  uint8_t xStep, yStep;
  uint8_t maxXSize, maxYSize;

  maxXSize = TFT_W / 8;
  maxYSize = TFT_H / 8;

  for (uint8_t i = 0; i < TEST_SAMPLE_SCREENS; i++) {
    for (yStep = 0; yStep < maxYSize; yStep++) {
      for (xStep = 0; xStep < maxXSize; xStep++) {

        // draw random tile form 0 to 90
        drawTile8x8(xStep*8, yStep*8, randNum()%90);
      }
    } 
  }
}

// ---------------------------------------------------------- //
__attribute__ ((noreturn)) int main(void)
{
  initSysTickTimer(); //it`s enable timer0 on atmega328p;

  uartSetup(USART_BAUD_1M);
  sync_gpu();

  gpuLoadTiles();

  uint32_t timerCount =0;

  uint8_t count =0;
  uint8_t testsCount = FUNC_TO_TEST_COUNT;

  for(;;) {

    for (count = 0; count < testsCount; count++) {
      pArrExecGFXFunc[count]();  // exec test function

       _delayMS(1000);  // actual 500 // little delay to see what happend on screen
      tftFillScreen(COLOR_BLACK);  // clear screen by black color
    }
  }
}
