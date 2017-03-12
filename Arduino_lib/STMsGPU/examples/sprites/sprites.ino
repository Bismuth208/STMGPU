#include <STMsGPU.h>

// --------------------------------------------------------- //

#define TFT_W gpu.width()
#define TFT_H gpu.height()

#define TEST_SAMPLE_SIZE    300
#define TEST_SAMPLE_SCREENS 20 // this is equal to 24000 tiles

// this is need to load textures from *.tle file 
// located on SD card - correctly
#define MAX_TILES 70
#define RAM_BASE 0
#define TLE_START 0
#define TILE_SET_W 9 // this is width of tileSet in tiles ( one tile width == 8 pixels)

#define SPRITE_NUMBER 0

// ---------------------------------------------------------- //
//#define CHK_GPU_BSY_PIN 2 // which pin arduino must check

/* BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
*  If your board have only ONE hardware serial,
*  then you MUST use SoftWareSerial instead!
*  On STM32 boards used Serial1 on PA9 and PA10.
*/
//STMGPU gpu(CHK_GPU_BSY_PIN); // use hardware BSY check, pin used
STMGPU gpu; // use software BSY check, no pin used
// ---------------------------------------------------------- //

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
// setup single sprite - number SPRITE_NUMBER
void gpuMakeSprite(void)
{
  // set tiles for sprite
  // always 4 numbers, even when sprite consist of 1 tile,
  // not used tiles may be set to any value (best is 0)
  gpu.setSpriteTiles(SPRITE_NUMBER, 0, 1, 0, 0);
  // set sprite type: 
  // SPR_2X1_8 mean: 2 sprites high, 1 widht, 8x8 pix single tile
  gpu.setSpriteType(SPRITE_NUMBER, SPR_2X1_8);
  // if you do not set this to 1, then the sprite will not be drawn
  gpu.setSpriteVisible(SPRITE_NUMBER, 1);
}

// --------------------------------------------------------- //
// draw random tile at random position
void drawRandSprites(void)
{
  uint16_t rndPosX, rndPosY;

   for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    rndPosX = randNum() % TFT_W;
    rndPosY = randNum() % TFT_H;
    
    // draw sprite SPRITE_NUMBER at random position
    gpu.drawSprite(SPRITE_NUMBER, rndPosX, rndPosY);
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
  gpu.loadTileSet8x8("pcs8x8", TILE_SET_W, RAM_BASE, TLE_START, MAX_TILES);

  gpuMakeSprite();
}

void loop() {
  drawRandSprites();
  delay(500);  // little delay to see what happend on screen
  gpu.fillScreen(COLOR_BLACK); // clear screen by black color
}
