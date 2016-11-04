#include <STMsGPU.h>
#include "gpuTest.h"

// --------------------------------------------------------- //

#define TFT_W gpu.width()
#define TFT_H gpu.height()

#define TEST_SAMPLE_SIZE    12000
#define TEST_SAMPLE_SCREENS 20 // this is equal to 24000 tiles

// --------------------------------------------------------- //

//#define CHK_GPU_BSY_PIN 2 // which pin arduino must check

/* BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
*  If your board have only ONE hardware serial,
*  then you MUST use SoftWareSerial instead!
*/
//STMGPU gpu = STMGPU(CHK_GPU_BSY_PIN); // use hardware BSY check, pin used
STMGPU gpu = STMGPU(); // use software BSY check, no pin used

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
    
    // draw random tile form 0 to 90, at random position
    gpu.drawTile8x8(rndPosX, rndPosY, randNum()%90);
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
      
      gpu.drawTile8x8(posX, posY, count);
      
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
        gpu.drawTile8x8(xStep*8, yStep*8, randNum()%90);
      }
    } 
  }
}

// ---------------------------------------------------------- //
void setup() {
  //USART_BAUD_9600 = 9600
  //USART_BAUD_57600 = 57600
  //USART_BAUD_115200 = 115200
  //USART_BAUD_1M = 1000000
  gpu.sync(USART_BAUD_1M);

  /* load 90 tiles to GPU's RAM at 0 position in RAM,
  *  from tileFileName,
  *  located on SD card attached to STM32 GPU
  *  9 - is width of tileSet in tiles ( 9 tiles width == 72 pixels)
  *  file name must respond to 8.3 name system
  *  8 chars max for filename, 3 chars max for file extension
  */
  gpu.loadTileSet8x8(tileFileName, 9, 0, 90);
}


void loop() {
  uint8_t testsCount = FUNC_TO_TEST_COUNT;

  for (uint8_t count = 0; count < testsCount; count++) {
      pArrExecGFXFunc[count](); // exec test function

      delay(500); // little delay to see what happend on screen
      gpu.fillScreen(COLOR_BLACK); // clear screen by black color
  }
}
