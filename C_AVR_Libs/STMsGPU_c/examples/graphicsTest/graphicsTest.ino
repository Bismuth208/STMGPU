#include <stdlib.h>
#include <string.h>

#include <systicktimer.h>
#include <uart.h>

#include <STMsGPU_c.h>

#include "gpuTest.h"

/* BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
 * If your board have only ONE hardware serial,
 * then you MUST use softWare serial instead!
 * moreover arduino libs here totally not supported!
 */

// --------------------------------------------------------- //

static uint16_t nextInt = 9; // absolutely randomised value

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

void drawRandPixels(void)
{
  uint8_t r, g, b;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = RND_COLOR;
    g = RND_COLOR;
    b = RND_COLOR;

    posX = RND_POSX(1);
    posY = RND_POSY(1);

    gpuDrawPixel(posX, posY, color565(r, g, b));
  }
}

void drawRandLines(void)
{
  uint8_t r, g, b;
  uint16_t posX, posY;
  uint16_t posX1, posY1;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = RND_COLOR;
    g = RND_COLOR;
    b = RND_COLOR;

    posX = RND_POSX(1);
    posY = RND_POSY(1);

    posX1 = RND_POSX(1);
    posY1 = RND_POSY(1);

    gpuDrawLine(posX, posY, posX1, posY1, color565(r, g, b));
  }
}


void drawRandRect(void)
{
  uint8_t r, g, b;
  uint16_t widght, height;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = RND_COLOR;
    g = RND_COLOR;
    b = RND_COLOR;

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

    gpuDrawRect(posX, posY, widght, height, RND_565COLOR);
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

    gpuFillRect(posX, posY, widght, height, RND_565COLOR);
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

    gpuFillTriangle(x1, y1, x2, y2, x3, y3, RND_565COLOR);
  }
}

void drawRandRoundRect(void)
{
  uint8_t r, g, b;
  uint16_t widght, height;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = RND_COLOR;
    g = RND_COLOR;
    b = RND_COLOR;

    posX = (randNum() % TFT_W);
    posY = (randNum() % TFT_H);

    widght = (randNum() % (TFT_W -1)+ posX);
    height = (randNum() % (TFT_H -1)+ posY);

    gpuDrawRoundRect(posX, posY,
                widght, height, 
                ((randNum() % 6)+4), color565(r, g, b));
  }
}

void drawRandRoundFillRect(void)
{
  uint8_t r, g, b;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = RND_COLOR;
    g = RND_COLOR;
    b = RND_COLOR;

    posX = RND_POSX(1);
    posY = RND_POSY(1);

    gpuFillRoundRect(posX, posY,
                (TFT_W - posX), (TFT_H - posY), 
                ((randNum() % 6)+4), color565(r, g, b));
  }
}


void drawRandCircle(void)
{
  uint8_t r, g, b;
  uint16_t posX, posY;
  uint16_t color;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = RND_COLOR;
    g = RND_COLOR;
    b = RND_COLOR;

    posX = RND_POSX(1);
    posY = RND_POSY(1);

    color = color565(r, g, b);
    r = ((randNum() % TFT_H)/4); // reuse for radius

    gpuDrawCircle(posX, posY, r, color);
  }
}

void drawRandFillCircle(void)
{
  uint8_t r, g, b;
  uint16_t posX, posY;
  uint16_t color;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = RND_COLOR;
    g = RND_COLOR;
    b = RND_COLOR;

    posX = RND_POSX(1);
    posY = RND_POSY(1);

    color = color565(r, g, b);
    r = ((randNum() % TFT_H)/4); // reuse for radius

    gpuFillCircle(posX, posY, r, color);
  }
}

void matrixScreen(void)
{
  uint8_t colX, rowsY;
  
  colX = TFT_W / 5;
  rowsY = TFT_H / 8;

  gpuSetTextSize(1);
  
  for (uint8_t iScr = 0; iScr < TEST_SAMPLE_SCREENS; iScr++) {
    for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {

      gpuSetCp437(rand() % 2);

      gpuDrawChar((rand() % colX) * 6, (rand() % rowsY) * 8,   // pos X and Y
                   (rand() % 255),                              // number of char
                   (((rand() % 192 + 32) & 0xFC) << 3),         // text color
                   COLOR_BLACK, 1);                             // BG color and size
    }
  }
}

// ---------------------------- GFX TESTs ------------------- //
void testHLinesHelp(uint16_t x0, uint16_t y0, uint16_t y1)
{
  for(int16_t x1=0; x1 < TFT_W; x1+=6) {
    gpuDrawLine(x0, y0, x1, y1, COLOR_YELLOW);
  }
}

void testVLinesHelp(uint16_t x0, uint16_t y0, uint16_t x1)
{
  for(int16_t y1=0; y1 < TFT_H; y1+=6) {
    gpuDrawLine(x0, y0, x1, y1, COLOR_YELLOW);
  }
}

void testlines(void)
{
  testHLinesHelp(0, 0, TFT_H);
  testVLinesHelp(0, 0, TFT_W);

  gpuFillScreen(COLOR_BLACK);
  testHLinesHelp(TFT_W, 0, TFT_H);
  testVLinesHelp(TFT_W, 0, 0);

  gpuFillScreen(COLOR_BLACK);
  testHLinesHelp(0, TFT_H, 0);
  testVLinesHelp(0, TFT_H, TFT_W);

  gpuFillScreen(COLOR_BLACK);
  testHLinesHelp(TFT_W, TFT_H, 0);
  testVLinesHelp(TFT_W, TFT_H, 0);
}

void testfastlines(void)
{
  for (int16_t y=0; y < TFT_H; y+=5) {
    gpuDrawFastHLine(0, y, TFT_W, COLOR_RED);
  }
  for (int16_t x=0; x < TFT_W; x+=5) {
    gpuDrawFastVLine(x, 0, TFT_H, COLOR_BLUE);
  }
}

void testdrawrects(void)
{
  for (int16_t x=0; x < TFT_W; x+=6) {
    gpuDrawRect(((TFT_W/2) - (x/2)), ((TFT_H/2) -((x/2))), x, x, COLOR_GREEN);
  }
}

void testfillrects(void)
{
  for (int16_t x=TFT_H-1; x > 6; x-=6) {
    gpuFillRect(TFT_W/2 - x/2, TFT_H/2 - x/2 , x, x, COLOR_YELLOW);
    gpuDrawRect(TFT_W/2 - x/2, TFT_H/2 - x/2 , x, x, COLOR_MAGENTA);
  }
}

void testdrawcircles(void)
{
  for (int16_t x=0; x < TFT_W+BASE_RADIUS; x+= (BASE_RADIUS<<1)) {
    for (int16_t y=0; y < TFT_H+BASE_RADIUS; y+= (BASE_RADIUS<<1)) {
      gpuDrawCircle(x, y, BASE_RADIUS, COLOR_WHITE);
    }
  }
}

void testfillcircles(void)
{
  for (int16_t x=BASE_RADIUS; x < TFT_W; x+= (BASE_RADIUS<<1)) {
    for (int16_t y=BASE_RADIUS; y < TFT_H; y+= (BASE_RADIUS<<1)) {
      gpuFillCircle(x, y, BASE_RADIUS, COLOR_BLUE);
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
    gpuDrawTriangle(w, y, y, x, z, x, color);
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
      gpuDrawRoundRect(x, y, w, h, 5, color);
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
  gpuSetCursor(0, 0);
  gpuSetTextWrap(true);

  gpuSetTextSize(1);
  gpuSetTextColor(COLOR_WHITE);
  gpuPrint_P(Loremipsum);

  gpuSetTextSize(2);
  gpuSetTextColor(COLOR_YELLOW);
  gpuPrint_P(textSize2);

  gpuSetTextSize(3);
  gpuSetTextColor(COLOR_RED);
  gpuPrint_P(textSize3);

  gpuSetTextSize(4);
  gpuSetTextColor(COLOR_GREEN);
  gpuPrint_P(textSize4);
}

// ---------------------------------------------------------- //
int main(void)
{
  // it`s enable timer0 on atmega328p;
  // need for delays;
  initSysTickTimer();

  // different speeds can be found in library STMsGPU_c.h
  sync_gpu(BAUD_SPEED_1M); // BAUD_SPEED_1M = 1,000,000 bod/s

  uint8_t testsCount = FUNC_TO_TEST_COUNT;

  for(;;) {

    for (uint8_t count = 0; count < testsCount; count++) {
      pArrTestFunc[count]();

      _delayMS(1000);  // actual 500
      gpuFillScreen(COLOR_BLACK);
    }
  }

  return 0;
}
