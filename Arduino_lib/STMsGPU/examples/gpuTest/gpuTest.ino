#include <STMsGPU.h>
#include "gpuTest.h"

// --------------------------------------------------------- //

//#define CHK_GPU_BSY_PIN 2 // which pin arduino must check

/* BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
*  If your board have only ONE hardware serial,
*  then you MUST use SoftWareSerial instead!
*/
//STMGPU gpu(CHK_GPU_BSY_PIN); // use hardware BSY check, pin used
STMGPU gpu; // use software BSY check, no pin used

// --------------------------------------------------------- //

unsigned long thisMicros = 0;
unsigned long lastMicros = 0;

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
  uint8_t r, g, b;
  uint16_t posX, posY;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = RND_COLOR;
    g = RND_COLOR;
    b = RND_COLOR;

    posX = RND_POSX(1);
    posY = RND_POSY(1);

    gpu.drawPixel(posX, posY, gpu.color565(r, g, b));
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

    gpu.drawLine(posX, posY, posX1, posY1, gpu.color565(r, g, b));
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

    gpu.drawRect(posX, posY, widght, height, gpu.color565(r, g, b));
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

    gpu.fillRect(posX, posY, widght, height, gpu.color565(r, g, b));
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

    gpu.fillTriangle(x1, y1, x2, y2, x3, y3, gpu.color565(r, g, b));
  }
}

void drawRandRoundRect(void)
{
  uint8_t r, g, b;
  uint16_t widght, height;
  uint16_t posX, posY;
  uint16_t color;

  for (uint16_t i = 0; i < TEST_SAMPLE_SIZE; i++) {
    r = RND_COLOR;
    g = RND_COLOR;
    b = RND_COLOR;

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

    color = gpu.color565(r, g, b);
    r = ((randNum() % 6)+4); // reuse for raduis

    gpu.drawRoundRect(posX, posY, widght, height, r, color);
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

    gpu.fillRoundRect(posX, posY,
                (TFT_W - posX), (TFT_H - posY), 
                ((randNum() % 6)+4), gpu.color565(r, g, b));
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

    color = gpu.color565(r, g, b);
    r = ((randNum() % TFT_H)/4); // reuse for radius

    gpu.drawCircle(posX, posY, r, color);
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

    color = gpu.color565(r, g, b);
    r = ((randNum() % TFT_H)/4); // reuse for radius

    gpu.fillCircle(posX, posY, r, color);
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
  //USART_BAUD_9600 = 9600
  //USART_BAUD_57600 = 57600
  //USART_BAUD_115200 = 115200
  //USART_BAUD_1M = 1000000
  gpu.begin(USART_BAUD_1M);
}

void loop() {
  uint8_t testsCount = FUNC_TO_TEST_COUNT;

  for (uint8_t count = 0; count < testsCount; count++) {
      pArrExecGFXFunc[count](); // exec test function

      delay(1000); // little delay to see what happend on screen
      gpu.fillScreen(COLOR_BLACK); // clear screen by black color
  }
}
