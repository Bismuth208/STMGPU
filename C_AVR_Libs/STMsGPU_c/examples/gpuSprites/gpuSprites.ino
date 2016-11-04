#include <stdlib.h>
#include <string.h>

#include <systicktimer.h>

#include <uart.h>
#include <STMsGPU_c.h>

// --------------------------------------------------------- //

#define TFT_W tftWidth()
#define TFT_H tftHeight()

#define TEST_SAMPLE_SIZE    300
#define TEST_SAMPLE_SCREENS 20 // this is equal to 24000 tiles

// --------------------------------------------------------- //

static uint16_t nextInt = 9;

const uint8_t tileFileName[] = "pcs8x8.tle";

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

void gpuMakeSprites(void)
{
  setSpriteTiles(0, 0, 1, 2, 3);
  setSpriteType(0, SPR_2X1_8);
  setSpriteVisible(0, 1);
}

// --------------------------------------------------------- //

// draw random tile at random position
void testDrawSprites(void)
{
  uint16_t rndPosX, rndPosY;

   for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    rndPosX = randNum() % TFT_W;
    rndPosY = randNum() % TFT_H;
    
    // draw sprite 0 at random position
    setSpritePosition(0, rndPosX, rndPosY);
    drawSprite(0);
  }
}

// ---------------------------------------------------------- //
__attribute__ ((noreturn)) int main(void)
{
  initSysTickTimer(); //it`s enable timer0 on atmega328p;

  uartSetup(USART_BAUD_1M);
  sync_gpu();

  gpuLoadTiles();
  gpuMakeSprites();

  for(;;) {

    testDrawSprites();  // exec test function

    _delayMS(500);  // actual 500 // little delay to see what happend on screen
    tftFillScreen(COLOR_BLACK);  // clear screen by black color
  }
}
