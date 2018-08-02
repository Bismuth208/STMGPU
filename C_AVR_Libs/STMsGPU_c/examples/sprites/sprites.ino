#include <stdlib.h>
#include <string.h>

#include <systicktimer.h>

#include <uart.h>
#include <STMsGPU_c.h>

/* BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
 * If your board have only ONE hardware serial,
 * then you MUST use softWare serial instead!
 * moreover arduino libs here totally not supported!
 */

// --------------------------------------------------------- //
#define TFT_W gpuWidth()
#define TFT_H gpuHeight()

#define TEST_SAMPLE_SIZE    300
#define TEST_SAMPLE_SCREENS 20 // this is equal to 24000 tiles

// this is need to load textures from *.tle file 
// located on SD card - correctly
#define MAX_TILES 70
#define RAM_BASE 0
#define TLE_START 0
#define TILE_SET_W 10 // this is width of tileSet in tiles ( one tile width == 8 pixels)

#define SPRITE_NUMBER 0

// --------------------------------------------------------- //

static uint16_t nextInt = 9; // absolutely randomised value

const uint8_t tileFileName[] = "pcs8x8";

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

void gpuMakeSprites(void)
{
  // set tiles for sprite
  // always 4 numbers, even when sprite consist of 1 tile,
  // not used tiles may be set to any value (best is 0)
  gpuSetSpriteTiles(SPRITE_NUMBER, 0, 1, 0, 0);
  // set sprite type: 
  // SPR_2X1_8 mean: 2 sprites high, 1 widht, 8x8 pix single tile
  gpuSetSpriteType(SPRITE_NUMBER, SPR_2X1_8);
  // if you do not set this to 1, then the sprite will not be drawn
  gpuSetSpriteVisible(SPRITE_NUMBER, 1);
}

// --------------------------------------------------------- //

// draw random tile at random position
void testDrawSprites(void)
{
  uint16_t rndPosX, rndPosY;

   for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    rndPosX = randNum() % TFT_W;
    rndPosY = randNum() % TFT_H;
    
    // draw sprite SPRITE_NUMBER at random position
    gpuSetSpritePosition(SPRITE_NUMBER, rndPosX, rndPosY);
    gpuDrawSprite(SPRITE_NUMBER);
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
  gpuMakeSprites();

  for(;;) {
    testDrawSprites();  // exec test function

    _delayMS(500);               // little delay to see what happend on screen
    gpuFillScreen(COLOR_BLACK);  // clear screen by black color
  }

  return 0;
}
