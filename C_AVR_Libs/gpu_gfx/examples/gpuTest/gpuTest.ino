#include <stdlib.h>
#include <string.h>

#include <systicktimer.h>

#include <uart.h>
#include <gpu_gfx.h>

#include "gpuTest.h"

#if 0
#define TFT_W 320
#define TFT_H 240
#else
#define TFT_W tftWidth()
#define TFT_H tftHeight()
#endif

#define TEST_SAMPLE_SIZE 2000

static uint16_t nextInt = 1;

unsigned long thisMicros = 0;
unsigned long lastMicros = 0;

void (*pArrExecGFXFunc[])(void) = {
    testdrawtext,
    testlines,
    testfastlines,
    //testdrawrects,
    testfillrects,
    testfillcircles,
    testroundrects,
    testtriangles,
    //mediabuttons,

    drawRandPixels,
    drawRandLines,
    drawRandRect,
    drawRandFillRect,
    //drawRandRoundRect,
    //drawRandRoundFillRect,
    drawRandCircle,
    drawRandFillCircle,
    matrixScreen
  };

uint16_t randNum(void)
{
  nextInt ^= nextInt >> 4;
  nextInt ^= nextInt << 7;
  nextInt ^= nextInt >> 9;
  nextInt = (nextInt * 214013 );
  return nextInt;
}

uint16_t getTemp(void)
{
  // The internal temperature has to be used
  // with the internal reference of 1.1V.
  // Channel 8 can not be selected with
  // the analogRead function yet.

  // Set the internal reference and mux.
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
  ADCSRA |= _BV(ADEN);  // enable the ADC

  //_delay_ms(20);            // wait for voltages to become stable.

  ADCSRA |= _BV(ADSC);  // Start the ADC

  // Detect end-of-conversion
  while (bit_is_set(ADCSRA,ADSC));

  // The returned temperature is in degrees Celcius.
  // Reading register "ADCW" takes care of how to read ADCL and ADCH.
  // The offset of 324.31 could be wrong. It is just an indication.
  //return ((ADCW - 327.31 ) / 1.22);
  return ADCW;
}

void initRand(void)
{
  nextInt += getTemp();  // yes, it real "random"!
}

// --------------------------------------------------------- //

void drawRandPixels(void)
{
  uint8_t r, g, b;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = randNum() % ((255 + 1) - 64) + 64;
    g = randNum() % ((255 + 1) - 64) + 64;
    b = randNum() % ((255 + 1) - 64) + 64;

    posX = (randNum() % (TFT_W-1));
    posY = (randNum() % (TFT_H-1));

    tftDrawPixel(posX, posY, color565(r, g, b));
  }
}

void drawRandLines(void)
{
  uint8_t r, g, b;
  uint16_t posX, posY;
  uint16_t posX1, posY1;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = randNum() % ((255 + 1) - 64) + 64;
    g = randNum() % ((255 + 1) - 64) + 64;
    b = randNum() % ((255 + 1) - 64) + 64;

    posX = (randNum() % (TFT_W-1));
    posY = (randNum() % (TFT_H-1));

    posX1 = (randNum() % (TFT_W-1));
    posY1 = (randNum() % (TFT_H-1));

    tftDrawLine(posX, posY, posX1, posY1, color565(r, g, b));
  }
}


void drawRandRect(void)
{
  uint8_t r, g, b;
  uint16_t widght, height;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = randNum() % ((255 + 1) - 64) + 64;
    g = randNum() % ((255 + 1) - 64) + 64;
    b = randNum() % ((255 + 1) - 64) + 64;

    posX = (randNum() % (TFT_W-4));
    posY = (randNum() % (TFT_H-4));

    widght = (randNum() % ((TFT_W - posX -4)));
    height = (randNum() % ((TFT_H - posY -4)));

    if (widght <= 1) {
      widght = 2;
    }

    if (height <= 1) {
      height = 2;
    }

    tftDrawRect(posX, posY, widght, height, color565(r, g, b));
  }
}

void drawRandFillRect(void)
{
  uint8_t r, g, b;
  uint16_t widght, height;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = randNum() % ((255 + 1) - 64) + 64;
    g = randNum() % ((255 + 1) - 64) + 64;
    b = randNum() % ((255 + 1) - 64) + 64;

    posX = (randNum() % (TFT_W-4));
    posY = (randNum() % (TFT_H-4));

    widght = (randNum() % ((TFT_W - posX -4)));
    height = (randNum() % ((TFT_H - posY -4)));

    if (widght <= 1) {
      widght = 2;
    }

    if (height <= 1) {
      height = 2;
    }

    tftFillRect(posX, posY, widght, height, color565(r, g, b));
  }
}

void drawRandRoundRect(void)
{
  uint8_t r, g, b;
  uint16_t widght, height;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = randNum() % ((255 + 1) - 64) + 64;
    g = randNum() % ((255 + 1) - 64) + 64;
    b = randNum() % ((255 + 1) - 64) + 64;

    posX = (randNum() % TFT_W);
    posY = (randNum() % TFT_H);

    widght = (randNum() % (TFT_W -1)+ posX);
    height = (randNum() % (TFT_H -1)+ posY);

    drawRoundRect(posX, posY,
                widght, height, 
                ((randNum() % 6)+4), color565(r, g, b));
  }
}

void drawRandRoundFillRect(void)
{
  uint8_t r, g, b;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = randNum() % ((255 + 1) - 64) + 64;
    g = randNum() % ((255 + 1) - 64) + 64;
    b = randNum() % ((255 + 1) - 64) + 64;

    posX = (randNum() % (TFT_W-1));
    posY = (randNum() % (TFT_H-1));

    fillRoundRect(posX, posY,
                (TFT_W - posX), (TFT_H - posY), 
                ((randNum() % 6)+4), color565(r, g, b));
  }
}


void drawRandCircle(void)
{
  uint8_t r, g, b;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = randNum() % ((255 + 1) - 64) + 64;
    g = randNum() % ((255 + 1) - 64) + 64;
    b = randNum() % ((255 + 1) - 64) + 64;

    drawCircle((randNum() % (TFT_W-1)), (randNum() % (TFT_H-1)),
               ((randNum() % TFT_H)/4), color565(r, g, b));
  }
}

void drawRandFillCircle(void)
{
  uint8_t r, g, b;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = randNum() % ((255 + 1) - 64) + 64;
    g = randNum() % ((255 + 1) - 64) + 64;
    b = randNum() % ((255 + 1) - 64) + 64;

    fillCircle((randNum() % (TFT_W-1)), (randNum() % (TFT_H-1)),
               ((randNum() % TFT_H)/4), color565(r, g, b));
  }
}

void matrixScreen(void)
{
  uint8_t colX, rowsY;
  
  colX = TFT_W / 5;
  rowsY = TFT_H / 8;

  setTextSize(1);
  
  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {

    cp437(randNum() % 2);

    //drawChar((randNum() % colX) * 6, (randNum() % rowsY) * 8,        // pos X and Y
      //(randNum() % 255),                                        // number of char
      //(((randNum() % 192 + 32) & 0xFC) << 3),                   // text color
      //COLOR_BLACK, 1);                                          // BG color and size
  
  
    setTextColorBG((((randNum() % 192 + 32) & 0xFC) << 3), COLOR_BLACK);
    printCharPos((randNum() % colX) * 6, (randNum() % rowsY) * 8, randNum() % 255);
  }
}

// ---------------------------- GFX TESTs ------------------- //
void testlines(void)
{
  uint16_t color = COLOR_YELLOW;

  for (int16_t x=0; x < TFT_W; x+=6) {
    tftDrawLine(0, 0, x, TFT_H-1, color);
  }
  for (int16_t y=0; y < TFT_H; y+=6) {
    tftDrawLine(0, 0, TFT_W-1, y, color);
  }

  tftFillScreen(COLOR_BLACK);
  for (int16_t x=0; x < TFT_W; x+=6) {
    tftDrawLine(TFT_W-1, 0, x, TFT_H-1, color);
  }
  for (int16_t y=0; y < TFT_H; y+=6) {
    tftDrawLine(TFT_W-1, 0, 0, y, color);
  }

  tftFillScreen(COLOR_BLACK);
  for (int16_t x=0; x < TFT_W; x+=6) {
    tftDrawLine(0, TFT_H-1, x, 0, color);
  }
  for (int16_t y=0; y < TFT_H; y+=6) {
    tftDrawLine(0, TFT_H-1, TFT_W-1, y, color);
  }

  tftFillScreen(COLOR_BLACK);
  for (int16_t x=0; x < TFT_W; x+=6) {
    tftDrawLine(TFT_W-1, TFT_H-1, x, 0, color);
  }
  for (int16_t y=0; y < TFT_H; y+=6) {
    tftDrawLine(TFT_W-1, TFT_H-1, 0, y, color);
  }
}

void testfastlines(void)
{
  uint16_t color1 = COLOR_RED;
  uint16_t color2 = COLOR_BLUE;

  for (int16_t y=0; y < TFT_H; y+=5) {
    tftDrawFastHLine(0, y, TFT_W, color1);
  }
  for (int16_t x=0; x < TFT_W; x+=5) {
    tftDrawFastVLine(x, 0, TFT_H, color2);
  }
}

void testdrawrects(void)
{
  uint16_t color = COLOR_GREEN;

  for (int16_t x=0; x < TFT_W; x+=6) {
    tftDrawRect(((TFT_W/2) - (x/2)), ((TFT_H/2) -((x/2))), x, x, color);
  }
}

void testfillrects(void)
{
  uint16_t color1 = COLOR_YELLOW;
  uint16_t color2 = COLOR_MAGENTA;

  for (int16_t x=TFT_H-1; x > 6; x-=6) {
    tftFillRect(TFT_W/2 - x/2, TFT_H/2 - x/2 , x, x, color1);
    tftDrawRect(TFT_W/2 - x/2, TFT_H/2 - x/2 , x, x, color2);
  }
}

void testdrawcircles(void)
{
  uint8_t radius = 10;
  uint16_t color = COLOR_WHITE; 

  for (int16_t x=0; x < TFT_W+radius; x+=radius*2) {
    for (int16_t y=0; y < TFT_H+radius; y+=radius*2) {
      drawCircle(x, y, radius, color);
    }
  }
}

void testfillcircles(void)
{
  uint8_t radius = 10;
  uint16_t color = COLOR_BLUE;

  for (int16_t x=radius; x < TFT_W; x+=radius*2) {
    for (int16_t y=radius; y < TFT_H; y+=radius*2) {
      fillCircle(x, y, radius, color);
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
    drawTriangle(w, y, y, x, z, x, color);
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
      drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}

void mediabuttons(void)
{
  // play
  fillRoundRect(25, 10, 78, 60, 8, COLOR_WHITE);
  fillTriangle(42, 20, 42, 60, 90, 40, COLOR_RED);
  _delayMS(500);
  // pause
  fillRoundRect(25, 90, 78, 60, 8, COLOR_WHITE);
  fillRoundRect(39, 98, 20, 45, 5, COLOR_GREEN);
  fillRoundRect(69, 98, 20, 45, 5, COLOR_GREEN);
  _delayMS(500);
  // play color
  fillTriangle(42, 20, 42, 60, 90, 40, COLOR_BLUE);
  _delayMS(50);
  // pause color
  fillRoundRect(39, 98, 20, 45, 5, COLOR_RED);
  fillRoundRect(69, 98, 20, 45, 5, COLOR_RED);
  // play color
  fillTriangle(42, 20, 42, 60, 90, 40, COLOR_GREEN);
}

void testdrawtext(void)
{
  setCursor(0, 0);
  setTextWrap(true);

  setTextSize(1);
  setTextColor(COLOR_WHITE);
  tftPrintPGR((char*)Loremipsum);

  setTextSize(2);
  setTextColor(COLOR_YELLOW);
  tftPrintPGR((char*)textSize2);

  setTextSize(3);
  setTextColor(COLOR_RED);
  tftPrintPGR((char*)textSize3);

  setTextSize(4);
  setTextColor(COLOR_GREEN);
  tftPrintPGR((char*)textSize4);
}

void drawSpendedTime(void)
{
  static char buf[6];

  itoa(thisMicros - lastMicros, buf, 10);

  print(buf);

  lastMicros = thisMicros;
  thisMicros = uptime();

}

// ---------------------------------------------------------- //
__attribute__ ((noreturn)) int main(void)
{
  initSysTickTimer(); //it`s enable timer0 on atmega328p;
  initRand();

  uartSetup(USART_BAUD_1M);
  sync_gpu();

  tftFillScreen(COLOR_BLACK);

  //uint32_t timerCount =0;

  uint8_t count =0;
  uint8_t testsCount = (sizeof(pArrExecGFXFunc)/sizeof(pArrExecGFXFunc[0]));

  for(;;) {

    for (count = 0; count < testsCount; count++) {
      pArrExecGFXFunc[count]();

      _delayMS(1000);  // actual 500
      tftFillScreen(COLOR_BLACK);
    }
  }
}
