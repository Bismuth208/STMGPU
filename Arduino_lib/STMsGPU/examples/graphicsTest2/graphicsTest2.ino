/***************************************************
  This is our GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include <STMsGPU.h>

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

// --------------------------------------------------------- //
void (*testFuctions[])(void) = {
  testFillScreen,
  testText,
  testLines,
  testFastLines,
  testRects,
  testFilledRects,
  testFilledCircles,
  testTriangles,
  testFilledTriangles,
  testRoundRects,
  testFilledRoundRects,
  testRotation
};
// --------------------------------------------------------- //

void setup() {
  // different speeds can be found in library STMsGPU.h
  gpu.begin(BAUD_SPEED_1M); // BAUD_SPEED_1M = 1,000,000 bod/s
}

void loop(void)
{
  for(auto &func : testFuctions) {
    gpu.fillScreen(COLOR_BLACK);
    func(); // exec test function

    // little delay to see what happend on screen
    //gpu.iDelay(500); // reque less ROM space and equal to delay()
  }
}

void testFillScreen()
{
  gpu.fillScreen(COLOR_RED);
  gpu.fillScreen(COLOR_GREEN);
  gpu.fillScreen(COLOR_BLUE);
  gpu.fillScreen(COLOR_BLACK);
}

void testText()
{
  gpu.setCursor(0, 0);
  gpu.setTextColor(COLOR_WHITE);  gpu.setTextSize(1);
  gpu.println(F("Hello World!"));
  gpu.setTextColor(COLOR_YELLOW); gpu.setTextSize(2);
  gpu.println(1234.56);
  gpu.setTextColor(COLOR_RED);    gpu.setTextSize(3);
  gpu.println(0xDEADBEEF, HEX);
  gpu.println();
  gpu.setTextColor(COLOR_GREEN);  gpu.setTextSize(5);
  gpu.println(F("Groop"));
  gpu.setTextSize(2);
  gpu.println(F("I implore thee,"));
  gpu.setTextSize(1);
  gpu.println(F("my foonting turlingdromes."));
  gpu.println(F("And hooptiously drangle me"));
  gpu.println(F("with crinkly bindlewurdles,"));
  gpu.println(F("Or I will rend thee"));
  gpu.println(F("in the gobberwarts"));
  gpu.println(F("with my blurglecruncheon,"));
  gpu.println(F("see if I don't!"));
}

void testHLinesHelp(uint16_t x0, uint16_t y0, uint16_t y1)
{
  int w = gpu.width();

  for(int16_t x1=0; x1 < w; x1+=6) {
    gpu.drawLine(x0, y0, x1, y1, COLOR_CYAN);
  }
}

void testVLinesHelp(uint16_t x0, uint16_t y0, uint16_t x1)
{
  int h = gpu.height();

  for(int16_t y1=0; y1 < h; y1+=6) {
    gpu.drawLine(x0, y0, x1, y1, COLOR_CYAN);
  }
}

void testLines(void)
{
  int w = gpu.width(),
      h = gpu.height();

  testHLinesHelp(0, 0, h);
  testVLinesHelp(0, 0, w);

  gpu.fillScreen(COLOR_BLACK);
  testHLinesHelp(w, 0, h);
  testVLinesHelp(w, 0, 0);

  gpu.fillScreen(COLOR_BLACK);
  testHLinesHelp(0, h, 0);
  testVLinesHelp(0, h, w);

  gpu.fillScreen(COLOR_BLACK);
  testHLinesHelp(w, h, 0);
  testVLinesHelp(w, h, 0);
}

void testFastLines(void)
{
  int           x, y, w = gpu.width(), h = gpu.height();

  for(y=0; y<h; y+=5) gpu.drawFastHLine(0, y, w, COLOR_RED);
  for(x=0; x<w; x+=5) gpu.drawFastVLine(x, 0, h, COLOR_BLUE);
}

void testRects(void)
{
  int           n, i, i2,
                cx = gpu.width()  / 2,
                cy = gpu.height() / 2;

  n     = min(gpu.width(), gpu.height());
  for(i=2; i<n; i+=6) {
    i2 = i / 2;
    gpu.drawRect(cx-i2, cy-i2, i, i, COLOR_GREEN);
  }
}

void testFilledRects(void)
{
  int           n, i, i2,
                cx = gpu.width()  / 2 - 1,
                cy = gpu.height() / 2 - 1;

  n = min(gpu.width(), gpu.height());
  for(i=n; i>0; i-=6) {
    i2    = i / 2;
    gpu.fillRect(cx-i2, cy-i2, i, i, COLOR_YELLOW);
    gpu.drawRect(cx-i2, cy-i2, i, i, COLOR_MAGENTA);
  }
}

void testFilledCircles(void)
{
  uint8_t radius = 10;

  int x, y, w = gpu.width(), h = gpu.height(), r2 = radius * 2;

  for(x=radius; x<w; x+=r2) {
    for(y=radius; y<h; y+=r2) {
      gpu.fillCircle(x, y, radius, COLOR_MAGENTA);
    }
  }

  testCircles();
}

void testCircles(void)
{
  uint8_t radius = 10;

  int           x, y, r2 = radius * 2,
                w = gpu.width()  + radius,
                h = gpu.height() + radius;

  // Screen is not cleared for this one
  for(x=0; x<w; x+=r2) {
    for(y=0; y<h; y+=r2) {
      gpu.drawCircle(x, y, radius, COLOR_WHITE);
    }
  }
}

void testTriangles(void)
{
  int           n, i, cx = gpu.width()  / 2 - 1,
                      cy = gpu.height() / 2 - 1;

  n     = min(cx, cy);
  for(i=0; i<n; i+=5) {
    gpu.drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      gpu.color565(i, i, i));
  }
}

void testFilledTriangles(void)
{
  int           i, cx = gpu.width()  / 2 - 1,
                   cy = gpu.height() / 2 - 1;

  for(i=min(cx,cy); i>10; i-=5) {
    gpu.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      gpu.color565(0, i*10, i*10));

    gpu.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      gpu.color565(i*10, i*10, 0));
  }
}

void testRoundRects(void)
{
  int           w, i, i2,
                cx = gpu.width()  / 2 - 1,
                cy = gpu.height() / 2 - 1;

  w     = min(gpu.width(), gpu.height());

  for(i=0; i<w; i+=6) {
    i2 = i / 2;
    gpu.drawRoundRect(cx-i2, cy-i2, i, i, i/8, gpu.color565(i, 0, 0));
  }
}

void testFilledRoundRects(void)
{
  int           i, i2,
                cx = gpu.width()  / 2 - 1,
                cy = gpu.height() / 2 - 1;

  for(i=min(gpu.width(), gpu.height()); i>20; i-=6) {
    i2 = i / 2;
    gpu.fillRoundRect(cx-i2, cy-i2, i, i, i/8, gpu.color565(0, i, 0));
  }
}

void testRotation(void)
{
  for(uint8_t rotation=0; rotation<4; rotation++) {
    gpu.setRotation(rotation);
    gpu.fillScreen(COLOR_BLACK);
    testText();
  }
  gpu.setRotation(1);
}
