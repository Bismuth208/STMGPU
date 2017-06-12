#include <STMsGPU.h>
#include "gpuTest.h"

// ---------------------------------------------------------- //
/*
 * which pin arduino must check, 
 * but by default this functianality is disabled to save
 * RAM and ROM memory.
 * To enable it go to STMsGPU.h and 
 * set define: 'REMOVE_HARDWARE_BSY' to 0
 */ 
//#define CHK_GPU_BSY_PIN 2

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

void drawRandPixels(void)
{
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {

    posX = RND_POSX(1);
    posY = RND_POSY(1);

    gpu.drawPixel(posX, posY, RND_565COLOR);
  }
}

void drawRandLines(void)
{
  uint16_t posX, posY;
  uint16_t posX1, posY1;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {

    posX = RND_POSX(1);
    posY = RND_POSY(1);

    posX1 = RND_POSX(1);
    posY1 = RND_POSY(1);

    gpu.drawLine(posX, posY, posX1, posY1, RND_565COLOR);
  }
}


void drawRandRect(void)
{
  uint16_t widght, height;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {

    posX = RND_POSX(4);
    posY = RND_POSY(4);

    widght = RND_POSX(posX-4);
    height = RND_POSY(posY-4);

    if (widght <= 1) {
      widght = 2;
    }

    if (height <= 1) {
      height = 2;
    }

    gpu.drawRect(posX, posY, widght, height, RND_565COLOR);
  }
}

void drawRandFillRect(void)
{
  uint16_t widght, height;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {

    posX = RND_POSX(4);
    posY = RND_POSY(4);

    widght = RND_POSX(posX-4);
    height = RND_POSY(posY-4);

    if (widght <= 1) {
      widght = 2;
    }

    if (height <= 1) {
      height = 2;
    }

    gpu.fillRect(posX, posY, widght, height, RND_565COLOR);
  }
}

void drawRandTriangles(void)
{
  uint16_t x1, x2, x3;
  uint16_t y1, y2, y3;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {

    x1 = RND_POSX(4);
    y1 = RND_POSY(4);

    x2 = RND_POSX(4);
    y2 = RND_POSY(4);

    x3 = RND_POSX(4);
    y3 = RND_POSY(4);

    gpu.fillTriangle(x1, y1, x2, y2, x3, y3, RND_565COLOR);
  }
}

void drawRandRoundRect(void)
{
  uint8_t r;
  uint16_t widght, height;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {

    posX = RND_POSX(1);
    posY = RND_POSY(1);

    widght = RND_POSX(posX-1);
    height = RND_POSY(posY-1);

    if (widght <= 1) {
      widght = 2;
    }

    if (height <= 1) {
      height = 2;
    }

    r = ((randNum() % 6)+4); // for raduis

    gpu.drawRoundRect(posX, posY, widght, height, r, RND_565COLOR);
  }
}

void drawRandRoundFillRect(void)
{
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {

    posX = RND_POSX(1);
    posY = RND_POSY(1);

    gpu.fillRoundRect(posX, posY,
                (TFT_W - posX), (TFT_H - posY), 
                ((randNum() % 6)+4), RND_565COLOR);
  }
}


void drawRandCircle(void)
{
  uint8_t r;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    posX = RND_POSX(1);
    posY = RND_POSY(1);
    r = ((randNum() % TFT_H)/4); // for radius   

    gpu.drawCircle(posX, posY, r, RND_565COLOR);
  }
}

void drawRandFillCircle(void)
{
  uint8_t r =0;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    posX = RND_POSX(1);
    posY = RND_POSY(1);
    r = ((randNum() % TFT_H)/4); // for radius
    
    gpu.fillCircle(posX, posY, r, RND_565COLOR);
  }
}

void matrixScreen(void)
{
  uint8_t colX, rowsY;
  
  colX = TFT_W / 5;
  rowsY = TFT_H / 8;

  gpu.setTextSize(1);
  
  for (uint8_t iScr = 0; iScr < TEST_SAMPLE_SCREENS; iScr++) {
    for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {

      gpu.cp437(randNum() % 2);

      gpu.drawChar((randNum() % colX) * 6, (randNum() % rowsY) * 8,   // pos X and Y
                   (randNum() % 255),                              // number of char
                   (((randNum() % 192 + 32) & 0xFC) << 3),         // text color
                   COLOR_BLACK, 1);                             // BG color and size
    }
  }
}

void drawText(void)
{
  gpu.setCursor(0, 0);
  gpu.setTextWrap(true);

  gpu.setTextSize(1);
  gpu.setTextColor(COLOR_WHITE);
  gpu.print(FS(Loremipsum2));

  gpu.setTextSize(2);
  gpu.setTextColor(COLOR_YELLOW);
  gpu.print(F("Text size 2\n"));

  gpu.setTextSize(3);
  gpu.setTextColor(COLOR_RED);
  gpu.print(F("Even bigger 3\n"));

  gpu.setTextSize(4);
  gpu.setTextColor(COLOR_GREEN);
  gpu.print(F("Seruious txt\n"));
}

// ---------------------------------------------------------- //
void setup() {
  // different speeds can be found in library STMsGPU.h
  // if call gpu.begin() without param speed will be BAUD_SPEED_57600
  gpu.begin(BAUD_SPEED_1M); // BAUD_SPEED_1M = 1,000,000 bod/s
}

void loop() {
  uint8_t testsCount = FUNC_TO_TEST_COUNT;

  for (uint8_t count = 0; count < testsCount; count++) {
    pArrTestFunc[count](); // exec test function

    // little delay to see what happend on screen
    gpu.iDelay(1000); // reque less ROM space and equal to delay()
    gpu.fillScreen(COLOR_BLACK); // clear screen by black color
  }
}
