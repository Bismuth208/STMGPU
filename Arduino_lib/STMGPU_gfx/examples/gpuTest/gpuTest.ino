#include <STMGPU_gfx.h>
#include "gpuTest.h"

#define TFT_W gpu.width()
#define TFT_H gpu.height()

#define TEST_SAMPLE_SIZE 2000

//#define CHK_GPU_BSY_PIN 2 // which pin arduino must check

// BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
// If your board have only ONE hardware serial,
// then you MUST use SoftWareSerial instead!
//STMGPU gpu = STMGPU(CHK_GPU_BSY_PIN); // use hardware BSY check, D2 pin used
STMGPU gpu = STMGPU(); // use software BSY check, no pin used

unsigned long thisMicros = 0;
unsigned long lastMicros = 0;


// --------------------------------------------------------- //

void drawRandPixels(void)
{
  uint8_t r, g, b;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = rand() % ((255 + 1) - 64) + 64;
    g = rand() % ((255 + 1) - 64) + 64;
    b = rand() % ((255 + 1) - 64) + 64;

    posX = (rand() % (TFT_W-1));
    posY = (rand() % (TFT_H-1));

    gpu.drawPixel(posX, posY, gpu.color565(r, g, b));
  }
}

void drawRandLines(void)
{
  uint8_t r, g, b;
  uint16_t posX, posY;
  uint16_t posX1, posY1;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = rand() % ((255 + 1) - 64) + 64;
    g = rand() % ((255 + 1) - 64) + 64;
    b = rand() % ((255 + 1) - 64) + 64;

    posX = (rand() % (TFT_W-1));
    posY = (rand() % (TFT_H-1));

    posX1 = (rand() % (TFT_W-1));
    posY1 = (rand() % (TFT_H-1));

    gpu.drawLine(posX, posY, posX1, posY1, gpu.color565(r, g, b));
  }
}


void drawRandRect(void)
{
  uint8_t r, g, b;
  uint16_t widght, height;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = rand() % ((255 + 1) - 64) + 64;
    g = rand() % ((255 + 1) - 64) + 64;
    b = rand() % ((255 + 1) - 64) + 64;

    posX = (rand() % (TFT_W-4));
    posY = (rand() % (TFT_H-4));

    widght = (rand() % ((TFT_W - posX -4)));
    height = (rand() % ((TFT_H - posY -4)));

    if (widght <= 1) {
      widght = 2;
    }

    if (height <= 1) {
      height = 2;
    }

    gpu.drawRect(posX, posY, widght, height, gpu.color565(r, g, b));
  }
}

void drawRandFillRect(void)
{
  uint8_t r, g, b;
  uint16_t widght, height;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = rand() % ((255 + 1) - 64) + 64;
    g = rand() % ((255 + 1) - 64) + 64;
    b = rand() % ((255 + 1) - 64) + 64;

    posX = (rand() % (TFT_W-4));
    posY = (rand() % (TFT_H-4));

    widght = (rand() % ((TFT_W - posX -4)));
    height = (rand() % ((TFT_H - posY -4)));

    if (widght <= 1) {
      widght = 2;
    }

    if (height <= 1) {
      height = 2;
    }

    gpu.fillRect(posX, posY, widght, height, gpu.color565(r, g, b));
  }
}

void drawRandRoundRect(void)
{
  uint8_t r, g, b;
  uint16_t widght, height;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = rand() % ((255 + 1) - 64) + 64;
    g = rand() % ((255 + 1) - 64) + 64;
    b = rand() % ((255 + 1) - 64) + 64;

    posX = (rand() % TFT_W);
    posY = (rand() % TFT_H);

    widght = (rand() % (TFT_W -1)+ posX);
    height = (rand() % (TFT_H -1)+ posY);

    gpu.drawRoundRect(posX, posY,
                widght, height, 
                ((rand() % 6)+4), gpu.color565(r, g, b));
  }
}

void drawRandRoundFillRect(void)
{
  uint8_t r, g, b;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = rand() % ((255 + 1) - 64) + 64;
    g = rand() % ((255 + 1) - 64) + 64;
    b = rand() % ((255 + 1) - 64) + 64;

    posX = (rand() % (TFT_W-1));
    posY = (rand() % (TFT_H-1));

    gpu.fillRoundRect(posX, posY,
                (TFT_W - posX), (TFT_H - posY), 
                ((rand() % 6)+4), gpu.color565(r, g, b));
  }
}


void drawRandCircle(void)
{
  uint8_t r, g, b;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = rand() % ((255 + 1) - 64) + 64;
    g = rand() % ((255 + 1) - 64) + 64;
    b = rand() % ((255 + 1) - 64) + 64;

    gpu.drawCircle((rand() % (TFT_W-1)), (rand() % (TFT_H-1)),
               ((rand() % TFT_H)/4), gpu.color565(r, g, b));
  }
}

void drawRandFillCircle(void)
{
  uint8_t r, g, b;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = rand() % ((255 + 1) - 64) + 64;
    g = rand() % ((255 + 1) - 64) + 64;
    b = rand() % ((255 + 1) - 64) + 64;

    gpu.fillCircle((rand() % (TFT_W-1)), (rand() % (TFT_H-1)),
               ((rand() % TFT_H)/4), gpu.color565(r, g, b));
  }
}

void matrixScreen(void)
{
  uint8_t colX, rowsY;
  
  colX = TFT_W / 5;
  rowsY = TFT_H / 8;

  gpu.setTextSize(1);
  
  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {

    gpu.cp437(rand() % 2);

    //drawChar((rand() % colX) * 6, (rand() % rowsY) * 8,        // pos X and Y
      //(rand() % 255),                                        // number of char
      //(((rand() % 192 + 32) & 0xFC) << 3),                   // text color
      //COLOR_BLACK, 1);                                          // BG color and size
  
  
    gpu.setTextColor((((rand() % 192 + 32) & 0xFC) << 3), COLOR_BLACK);
    gpu.printCharPos((rand() % colX) * 6, (rand() % rowsY) * 8, rand() % 255);
  }
}

// ---------------------------- GFX TESTs ------------------- //
void testlines(void)
{
  uint16_t color = COLOR_YELLOW;

  for (int16_t x=0; x < TFT_W; x+=6) {
    gpu.drawLine(0, 0, x, TFT_H-1, color);
  }
  for (int16_t y=0; y < TFT_H; y+=6) {
    gpu.drawLine(0, 0, TFT_W-1, y, color);
  }

  gpu.fillScreen(COLOR_BLACK);
  for (int16_t x=0; x < TFT_W; x+=6) {
    gpu.drawLine(TFT_W-1, 0, x, TFT_H-1, color);
  }
  for (int16_t y=0; y < TFT_H; y+=6) {
    gpu.drawLine(TFT_W-1, 0, 0, y, color);
  }

  gpu.fillScreen(COLOR_BLACK);
  for (int16_t x=0; x < TFT_W; x+=6) {
    gpu.drawLine(0, TFT_H-1, x, 0, color);
  }
  for (int16_t y=0; y < TFT_H; y+=6) {
    gpu.drawLine(0, TFT_H-1, TFT_W-1, y, color);
  }

  gpu.fillScreen(COLOR_BLACK);
  for (int16_t x=0; x < TFT_W; x+=6) {
    gpu.drawLine(TFT_W-1, TFT_H-1, x, 0, color);
  }
  for (int16_t y=0; y < TFT_H; y+=6) {
    gpu.drawLine(TFT_W-1, TFT_H-1, 0, y, color);
  }
}

void testfastlines(void)
{
  uint16_t color1 = COLOR_RED;
  uint16_t color2 = COLOR_BLUE;

  for (int16_t y=0; y < TFT_H; y+=5) {
    gpu.drawFastHLine(0, y, TFT_W, color1);
  }
  for (int16_t x=0; x < TFT_W; x+=5) {
    gpu.drawFastVLine(x, 0, TFT_H, color2);
  }
}

void testdrawrects(void)
{
  uint16_t color = COLOR_GREEN;

  for (int16_t x=0; x < TFT_W; x+=6) {
    gpu.drawRect(((TFT_W/2) - (x/2)), ((TFT_H/2) -((x/2))), x, x, color);
  }
}

void testfillrects(void)
{
  uint16_t color1 = COLOR_YELLOW;
  uint16_t color2 = COLOR_MAGENTA;

  for (int16_t x=TFT_H-1; x > 6; x-=6) {
    gpu.fillRect(TFT_W/2 - x/2, TFT_H/2 - x/2 , x, x, color1);
    gpu.drawRect(TFT_W/2 - x/2, TFT_H/2 - x/2 , x, x, color2);
  }
}

void testdrawcircles(void)
{
  uint8_t radius = 10;
  uint16_t color = COLOR_WHITE; 

  for (int16_t x=0; x < TFT_W+radius; x+=radius*2) {
    for (int16_t y=0; y < TFT_H+radius; y+=radius*2) {
      gpu.drawCircle(x, y, radius, color);
    }
  }
}

void testfillcircles(void)
{
  uint8_t radius = 10;
  uint16_t color = COLOR_BLUE;

  for (int16_t x=radius; x < TFT_W; x+=radius*2) {
    for (int16_t y=radius; y < TFT_H; y+=radius*2) {
      gpu.fillCircle(x, y, radius, color);
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

void mediabuttons(void)
{
  // play
  gpu.fillRoundRect(25, 10, 78, 60, 8, COLOR_WHITE);
  gpu.fillTriangle(42, 20, 42, 60, 90, 40, COLOR_RED);
  delay(500);
  // pause
  gpu.fillRoundRect(25, 90, 78, 60, 8, COLOR_WHITE);
  gpu.fillRoundRect(39, 98, 20, 45, 5, COLOR_GREEN);
  gpu.fillRoundRect(69, 98, 20, 45, 5, COLOR_GREEN);
  delay(500);
  // play color
  gpu.fillTriangle(42, 20, 42, 60, 90, 40, COLOR_BLUE);
  delay(50);
  // pause color
  gpu.fillRoundRect(39, 98, 20, 45, 5, COLOR_RED);
  gpu.fillRoundRect(69, 98, 20, 45, 5, COLOR_RED);
  // play color
  gpu.fillTriangle(42, 20, 42, 60, 90, 40, COLOR_GREEN);
}

void testdrawtext(void)
{
  gpu.setCursor(0, 0);
  gpu.setTextColor(COLOR_WHITE);
  gpu.setTextWrap(true);

  //gpu.printPGR((char*)Loremipsum);
  gpu.printPGR((char*)Loremipsum2);
}

// ---------------------------------------------------------- //
void setup() {
  srand(9);   // yes, it real, true dice roll "random"!

  //USART_BAUD_9600 = 9600
  //USART_BAUD_57600 = 57600
  //USART_BAUD_115200 = 115200
  //USART_BAUD_1M = 1000000
  gpu.sync(USART_BAUD_1M);
  gpu.fillScreen(COLOR_BLACK);
}

void loop() {
  uint8_t testsCount = FUNC_TO_TEST_COUNT;

  for (uint8_t count = 0; count < testsCount; count++) {
      pArrExecGFXFunc[count](); // exec test function

      delay(1000); // little delay to see what happend on screen
      gpu.fillScreen(COLOR_BLACK); // clear screen by black color
  }
}
