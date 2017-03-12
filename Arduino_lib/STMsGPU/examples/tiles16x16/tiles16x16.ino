#include <STMsGPU.h>
#include "gpuTest.h"

// --------------------------------------------------------- //

//#define CHK_GPU_BSY_PIN 2 // which pin arduino must check

/* BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
*  If your board have only ONE hardware serial,
*  then you MUST use SoftWareSerial instead!
*  On STM32 boards used Serial1 on PA9 and PA10.
*/
//STMGPU gpu(CHK_GPU_BSY_PIN); // use hardware BSY check, pin used
STMGPU gpu; // use software BSY check, no pin used

// --------------------------------------------------------- //

static uint16_t nextInt = 9;

// --------------------------------------------------------- //
// this is much faster than rand();
uint16_t randNum(void)
{
  nextInt ^= nextInt >> 4;
  nextInt ^= nextInt << 7;
  nextInt ^= nextInt >> 9;
  nextInt = (nextInt * 214013 );
  return nextInt;
}
// --------------------------------------------------------- //

// draw random tile at random position
void testDrawTiles(void)
{
  uint16_t rndPosX, rndPosY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    rndPosX = randNum() % TFT_W;
    rndPosY = randNum() % TFT_H;
    
    // draw random tile from 0 to MAX_TILES, at random position
    gpu.drawTile16x16(rndPosX, rndPosY, randNum()%MAX_TILES);
  }
}

// Draw on screen limited range of tiles
// on screen must apear square 10x8 tiles
void drawRamTileSet16x16(void)
{
  int16_t posX, posY;
  uint8_t count =0;
  
  // draw 10 tiles
  for(uint8_t countY =0; countY <TILE_SHOW_W; countY++) {
    for(uint8_t countX =0; countX <TILE_SHOW_H; countX++) {
      
      // 50 is default position in px on screen
      posX = (50 + ( countX * TLE_16X16_SIZE ));
      posY = (50 + ( countY * TLE_16X16_SIZE ));
      
      gpu.drawTile16x16(posX, posY, count++);
    }
  }
}

// Fill whole screen by random tiles
void fillScreenByTiles(void)
{
  uint8_t xStep, yStep;
  uint8_t maxXSize, maxYSize;

  maxXSize = TFT_W / TLE_16X16_SIZE;
  maxYSize = TFT_H / TLE_16X16_SIZE;

  for (uint8_t i = 0; i < TEST_SAMPLE_SCREENS; i++) {
    for (yStep = 0; yStep < maxYSize; yStep++) {
      for (xStep = 0; xStep < maxXSize; xStep++) {

        // draw random tile form 0 to MAX_TILES
        gpu.drawTile16x16(xStep*TLE_16X16_SIZE, yStep*TLE_16X16_SIZE, randNum()%MAX_TILES);
      }
    } 
  }
}

// ---------------------------------------------------------- //
void setup() {
  // different speeds can be found in library STMsGPU.h
  gpu.begin(BAUD_SPEED_1M); // BAUD_SPEED_1M = 1,000,000 bod/s

  /* load MAX_TILES tiles to sGPU's RAM at RAM_BASE position in it's RAM,
  *  from tileFileName,
  *  located on SD card attached to STM32 sGPU
  *  TLE_START - nunber of tile in tileset from which tiles will be loaded
  *  file name must respond to 8.3 name system
  *  8 chars max for filename, 3 chars max for file extension
  *  sGPU add *.tle extension automatically
  */
  gpu.loadTileSet16x16("pcs16x16", TILE_SET_W-1, RAM_BASE, TLE_START, MAX_TILES);
}


void loop() {
  uint8_t testsCount = FUNC_TO_TEST_COUNT;

  for (uint8_t count = 0; count < testsCount; count++) {
      pArrExecGFXFunc[count](); // exec test function

      delay(500); // little delay to see what happend on screen
      gpu.fillScreen(COLOR_BLACK); // clear screen by black color
  }
}
