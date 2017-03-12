#include <STMsGPU.h>
#include "gpuTest.h"

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

// ---------------------------- GFX TESTs ------------------- //
void testHLinesHelp(uint16_t x0, uint16_t y0, uint16_t y1)
{
  for(int16_t x1=0; x1 < TFT_W; x1+=6) {
    gpu.drawLine(x0, y0, x1, y1, COLOR_YELLOW);
  }
}

void testVLinesHelp(uint16_t x0, uint16_t y0, uint16_t x1)
{
  for(int16_t y1=0; y1 < TFT_H; y1+=6) {
    gpu.drawLine(x0, y0, x1, y1, COLOR_YELLOW);
  }
}

void testlines(void)
{
  testHLinesHelp(0, 0, TFT_H);
  testVLinesHelp(0, 0, TFT_W);

  gpu.fillScreen(COLOR_BLACK);
  testHLinesHelp(TFT_W, 0, TFT_H);
  testVLinesHelp(TFT_W, 0, 0);

  gpu.fillScreen(COLOR_BLACK);
  testHLinesHelp(0, TFT_H, 0);
  testVLinesHelp(0, TFT_H, TFT_W);

  gpu.fillScreen(COLOR_BLACK);
  testHLinesHelp(TFT_W, TFT_H, 0);
  testVLinesHelp(TFT_W, TFT_H, 0);
}

void testfastlines(void)
{
  for (int16_t y=0; y < TFT_H; y+=5) {
    gpu.drawFastHLine(0, y, TFT_W, COLOR_RED);
  }
  for (int16_t x=0; x < TFT_W; x+=5) {
    gpu.drawFastVLine(x, 0, TFT_H, COLOR_BLUE);
  }
}

void testdrawrects(void)
{
  for (int16_t x=0; x < TFT_W; x+=6) {
    gpu.drawRect(((TFT_W/2) - (x/2)), ((TFT_H/2) -((x/2))), x, x, COLOR_GREEN);
  }
}

void testfillrects(void)
{
  for (int16_t x=TFT_H-1; x > 6; x-=6) {
    gpu.fillRect(TFT_W/2 - x/2, TFT_H/2 - x/2 , x, x, COLOR_YELLOW);
    gpu.drawRect(TFT_W/2 - x/2, TFT_H/2 - x/2 , x, x, COLOR_MAGENTA);
  }
}

void testdrawcircles(void)
{
  for (int16_t x=0; x < TFT_W; x+= (BASE_RADIUS<<1)) {
    for (int16_t y=0; y < TFT_H; y+= (BASE_RADIUS<<1)) {
      gpu.drawCircle(x, y, BASE_RADIUS, COLOR_WHITE);
    }
  }
}

void testfillcircles(void)
{
  for (int16_t x=BASE_RADIUS; x < TFT_W; x+= (BASE_RADIUS<<1)) {
    for (int16_t y=BASE_RADIUS; y < TFT_H; y+= (BASE_RADIUS<<1)) {
      gpu.fillCircle(x, y, BASE_RADIUS, COLOR_BLUE);
    }
  }

  testdrawcircles();
}

void testtriangles(void)
{
  int color = 0xF800;
  int t;
  int w = TFT_W/2;
  int x = TFT_H-1;
  int y = 0;
  int z = TFT_W;

  for(t = 0 ; t <= 20; t+=1) {
    gpu.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testroundrects(void)
{
  uint16_t color = 100;

  for(uint8_t t = 0 ; t <= 4; t++) {
    int x = 0;
    int y = 0;
    int w = TFT_W-2;
    int h = TFT_H-2;
    for(uint8_t i = 0 ; i <= 16; i++) {
      gpu.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}

void testdrawtext(void)
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
  gpu.begin(BAUD_SPEED_1M); // BAUD_SPEED_1M = 1,000,000 bod/s
}

void loop() {
  uint8_t testsCount = FUNC_TO_TEST_COUNT;

  for (uint8_t count = 0; count < testsCount; count++) {
    //(*pArrExecGFXFunc+count)(); // exec test function
    pArrExecGFXFunc[count](); // exec test function

    delay(1000); // little delay to see what happend on screen
    gpu.fillScreen(COLOR_BLACK); // clear screen by black color
  }
}
