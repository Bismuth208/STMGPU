#include <STMsGPU.h>

// --------------------------------------------------------- //

#define TFT_W gpu.width()
#define TFT_H gpu.height()

#define TEST_SAMPLE_SIZE    300
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

const char tileFileName[] = "pcs8x8.tle";

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

void gpuMakeSprites(void)
{
  gpu.setSpriteTiles(0, 0, 1, 2, 3);
  gpu.setSpriteType(0, SPR_2X1_8);
  gpu.setSpriteVisible(0, 1);
}

// --------------------------------------------------------- //

// draw random tile at random position
void drawRandSprites(void)
{
  uint16_t rndPosX, rndPosY;

   for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    rndPosX = randNum() % TFT_W;
    rndPosY = randNum() % TFT_H;
    
    // draw sprite 0 at random position
    gpu.setSpritePosition(0, rndPosX, rndPosY);
    gpu.drawSprite(0);
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

  gpuMakeSprites();
}

void loop() {

  drawRandSprites();
  delay(500);
  gpu.fillScreen(COLOR_BLACK); // clear screen by black color
}
