#include <stdlib.h>
#include <string.h>

#include <systicktimer.h>

#include <uart.h>
#include <STMsGPU_c.h>

#include "gpuTest.h"

// --------------------------------------------------------- //

#define TFT_W gpuWidth()
#define TFT_H gpuHeight()

#define TEST_SAMPLE_SIZE 2000
#define TEST_SAMPLE_SCREENS 2

#define MIN_COLOR 32
#define MAX_COLOR 255
#define COLOR_RANGE (((MAX_COLOR + 1) - MIN_COLOR) + MIN_COLOR)
#define RND_COLOR (randNum() % COLOR_RANGE)

#define RND_POSX(offset) (randNum() % (TFT_W-offset))
#define RND_POSY(offset) (randNum() % (TFT_H-offset)) 

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

    gpuDrawRect(posX, posY, widght, height, color565(r, g, b));
  }
}

void drawRandFillRect(void)
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

    gpuFillRect(posX, posY, widght, height, color565(r, g, b));
  }
}

void drawRandTriangles(void)
{
  uint8_t r, g, b;
  uint16_t x1, x2, x3;
  uint16_t y1, y2, y3;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = RND_COLOR;
    g = RND_COLOR;
    b = RND_COLOR;

    x1 = RND_POSX(4);
    y1 = RND_POSY(4);

    x2 = RND_POSX(4);
    y2 = RND_POSY(4);

    x3 = RND_POSX(4);
    y3 = RND_POSY(4);

    gpuFillTriangle(x1, y1, x2, y2, x3, y3, color565(r, g, b));
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
void testlines(void)
{
  uint16_t color = COLOR_YELLOW;

  for (int16_t x=0; x < TFT_W; x+=6) {
    gpuDrawLine(0, 0, x, TFT_H-1, color);
  }
  for (int16_t y=0; y < TFT_H; y+=6) {
    gpuDrawLine(0, 0, TFT_W-1, y, color);
  }

  gpuFillScreen(COLOR_BLACK);
  for (int16_t x=0; x < TFT_W; x+=6) {
    gpuDrawLine(TFT_W-1, 0, x, TFT_H-1, color);
  }
  for (int16_t y=0; y < TFT_H; y+=6) {
    gpuDrawLine(TFT_W-1, 0, 0, y, color);
  }

  gpuFillScreen(COLOR_BLACK);
  for (int16_t x=0; x < TFT_W; x+=6) {
    gpuDrawLine(0, TFT_H-1, x, 0, color);
  }
  for (int16_t y=0; y < TFT_H; y+=6) {
    gpuDrawLine(0, TFT_H-1, TFT_W-1, y, color);
  }

  gpuFillScreen(COLOR_BLACK);
  for (int16_t x=0; x < TFT_W; x+=6) {
    gpuDrawLine(TFT_W-1, TFT_H-1, x, 0, color);
  }
  for (int16_t y=0; y < TFT_H; y+=6) {
    gpuDrawLine(TFT_W-1, TFT_H-1, 0, y, color);
  }
}

void testfastlines(void)
{
  uint16_t color1 = COLOR_RED;
  uint16_t color2 = COLOR_BLUE;

  for (int16_t y=0; y < TFT_H; y+=5) {
    gpuDrawFastHLine(0, y, TFT_W, color1);
  }
  for (int16_t x=0; x < TFT_W; x+=5) {
    gpuDrawFastVLine(x, 0, TFT_H, color2);
  }
}

void testdrawrects(void)
{
  uint16_t color = COLOR_GREEN;

  for (int16_t x=0; x < TFT_W; x+=6) {
    gpuDrawRect(((TFT_W/2) - (x/2)), ((TFT_H/2) -((x/2))), x, x, color);
  }
}

void testfillrects(void)
{
  uint16_t color1 = COLOR_YELLOW;
  uint16_t color2 = COLOR_MAGENTA;

  for (int16_t x=TFT_H-1; x > 6; x-=6) {
    gpuFillRect(TFT_W/2 - x/2, TFT_H/2 - x/2 , x, x, color1);
    gpuDrawRect(TFT_W/2 - x/2, TFT_H/2 - x/2 , x, x, color2);
  }
}

void testdrawcircles(void)
{
  uint8_t radius = 10;
  uint16_t color = COLOR_WHITE; 

  for (int16_t x=0; x < TFT_W+radius; x+=radius*2) {
    for (int16_t y=0; y < TFT_H+radius; y+=radius*2) {
      gpuDrawCircle(x, y, radius, color);
    }
  }
}

void testfillcircles(void)
{
  uint8_t radius = 10;
  uint16_t color = COLOR_BLUE;

  for (int16_t x=radius; x < TFT_W; x+=radius*2) {
    for (int16_t y=radius; y < TFT_H; y+=radius*2) {
      gpuFillCircle(x, y, radius, color);
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
  gpuPrintPGR(Loremipsum);

  gpuSetTextSize(2);
  gpuSetTextColor(COLOR_YELLOW);
  gpuPrintPGR(textSize2);

  gpuSetTextSize(3);
  gpuSetTextColor(COLOR_RED);
  gpuPrintPGR(textSize3);

  gpuSetTextSize(4);
  gpuSetTextColor(COLOR_GREEN);
  gpuPrintPGR(textSize4);
}

// ---------------------------------------------------------- //
int main(void)
{
  // it`s enable timer0 on atmega328p;
  // need for delays;
  initSysTickTimer();

  //BAUD_SPEED_9600 = 9600
  //BAUD_SPEED_57600 = 57600
  //BAUD_SPEED_115200 = 115200
  //BAUD_SPEED_1M = 1000000
  sync_gpu(BAUD_SPEED_1M);  // establish connection

  uint8_t testsCount = FUNC_TO_TEST_COUNT;

  for(;;) {

    for (uint8_t count = 0; count < testsCount; count++) {
      pArrExecGFXFunc[count]();

      _delayMS(1000);  // actual 500
      gpuFillScreen(COLOR_BLACK);
    }
  }

  return 0;
}
