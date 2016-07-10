#include <stdlib.h>
#include <string.h>

#include <systicktimer.h>

#if 1
#include <uart.h>
#include <gpu_gfx.h>
#else
//#include <avrspi.h>
//#include <ili9341.h>
//#include <st7735.h>
//#include <gfx.h>
#endif

#include "gpuTest.h"

#if 0
#define TFT_W 320
#define TFT_H 240
#else
#define TFT_W tftWidth()
#define TFT_H tftHeight()
#endif

#define TEST_SAMPLE_SIZE 2000

//#define BAUDRATE 9600
//#define BAUDRATE 57600
//#define BAUDRATE 115200
#define BAUDRATE 1000000

//static uint16_t nextInt = 4;
static uint32_t nextInt = 1;

uint16_t roundsPassed =0;

void (*pArrExecGFXFunc[])(void) = {
    testdrawtext,
    testlines,
    testfastlines,
    //testdrawrects,
    testfillrects,
    testfillcircles,
    testroundrects,
    testtriangles,
    //mediabuttons

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

/*
// xorshift + LCPRNG
uint16_t randNum(void)
{
  nextInt ^= nextInt >> 2; // 4
  nextInt ^= nextInt << 5; // 7
  nextInt ^= nextInt >> 9; // 9
  //nextInt = (nextInt * 214013 ) & 0x7fff;
  nextInt = (nextInt * 27103 ) & 0x7fff;
  return nextInt;
}
*/
// xorshift
/*
uint32_t randNum(void)
{
  nextInt ^= nextInt >> 6; // 12
  nextInt ^= nextInt << 11; // 25
  nextInt ^= nextInt >> 15; // 27
  nextInt = ( nextInt * 3538123 ) & 0x7fffffff;
  //nextInt = ( nextInt * 2348138948 ) & 0x7fffffff;
  return nextInt;
}
*/
uint32_t randNum(void)
{
  nextInt ^= nextInt >> 6;
  nextInt ^= nextInt << 11;
  nextInt ^= nextInt >> 15;
  return ( nextInt * 3538123 );
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

#if 0
void reverse(char s[])
{
  int i, j;
  char c;
  for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

void itoa(int n, char s[])
{
  int i, sign;
  if ((sign = n) < 0) //записываем знак 
    n = -n; // делаем n положительным числом 
  i = 0;
  do { //генерируем цифры в обратном порядке 
    s[i++] = n % 10 + '0'; //берем следующую цифру 
  } while ((n /= 10) > 0); // удаляем 
  
  if (sign < 0)
    s[i++] = '-';
  s[i] = '\0';
  reverse(s);
}
#endif

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
               ((randNum() % TFT_H)/2), color565(r, g, b));
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
               ((randNum() % TFT_H)/2), color565(r, g, b));
  }
}

void matrixScreen()
{
  uint8_t colX, rowsY;
  
  colX = TFT_W / 5;
  rowsY = TFT_H / 8;

  setTextSize(1);
  
  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {

    cp437(randNum() % 2);
/*
    drawChar((randNum() % colX) * 6, (randNum() % rowsY) * 8,        // pos X and Y
      (randNum() % 255),                                        // number of char
      (((randNum() % 192 + 32) & 0xFC) << 3),                   // text color
      COLOR_BLACK, 1);                                          // BG color and size
  */
  
    
    setCursor((randNum() % colX) * 6, (randNum() % rowsY) * 8);
    setTextColorBG((((randNum() % 192 + 32) & 0xFC) << 3), COLOR_BLACK);
    printChar(randNum() % 255);
    
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
    //tftDrawRect(TFT_W/2 -x/2, TFT_H/2 -x/2 , x, x, color);
  }
}

void testfillrects(void)
{
  uint16_t color1 = COLOR_YELLOW;
  uint16_t color2 = COLOR_MAGENTA;

  for (int16_t x=TFT_W-1; x > 6; x-=6) {
    tftFillRect(TFT_W/2 -x/2, TFT_H/2 -x/2 , x, x, color1);
    tftDrawRect(TFT_W/2 -x/2, TFT_H/2 -x/2 , x, x, color2);
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

  for(t = 0 ; t <= 15; t+=1) {
    drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testroundrects(void)
{
  int color = 100;
  int i;
  int t;

  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = TFT_W-2;
    int h = TFT_H-2;
    for(i = 0 ; i <= 16; i+=1) {
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
  setTextColor(COLOR_WHITE);
  setTextWrap(true);

  //tftPrintPGR((char*)Loremipsum);
  tftPrintPGR((char*)Loremipsum2);
  //print("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ");
}

// ---------------------------------------------------------- //
void drawRounds(void)
{
  char buf[10];

  memset(buf, 0, 10);

  setCursor(0, 0);
  setTextColor(COLOR_WHITE);

  ++roundsPassed;

  print("Rounds passed: ");
  itoa(roundsPassed, buf, 10);
  print(buf);
}

// ---------------------------------------------------------- //
__attribute__ ((noreturn)) int main(void)
{
  initSysTickTimer(); //it`s enable timer0 on atmega328p;
  initRand();

  #if 1
  uartSetup(BAUDRATE);
  sync_gpu();
  #else
  initSPI();

  //tftBegin();         // initialize a ILI9341 chip
  initR(INITR_BLACKTAB);
  tftSetRotation(1);
  #endif

  tftFillScreen(COLOR_BLACK);

  uint32_t timerCount =0;

  uint8_t count =0;
  uint8_t testsCount = (sizeof(pArrExecGFXFunc)/sizeof(pArrExecGFXFunc[0]));

  for(;;) {

    for (count = 0; count < testsCount; count++) {
      pArrExecGFXFunc[count]();

       _delayMS(1000);  // actual 500
      tftFillScreen(COLOR_BLACK);
    }
    

    /* hardtest 
    pArrExecGFXFunc[randNum() % 14]();
    tftFillScreen(COLOR_BLACK);
    */
    
    //drawRounds();
    //_delayMS(4000);  // actual 2000
    //tftFillScreen(COLOR_BLACK);
  }
}
