/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

#include <stm32f10x.h>
#include <stm32f10x_dma.h>

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <systicktimer.h>

#include <spi.h>

#include "gfx.h"
#include "fonts.h"

#include "gfxDMA.h"

//-------------------------------------------------------------------------------------------//

#ifndef min
#define min(a,b) ((a < b) ? a : b)
#endif

#ifndef sign
#define sign(x) ((x)>0?(1):-(1))
#endif

#ifndef abs
#define abs(x) ((x)>0?(x):-(x))
#endif

#ifndef swap
#define swap(a, b) { int16_t t = a; a = b; b = t; }
#endif

//-------------------------------------------------------------------------------------------//

static uint8_t textsize = 1;
//static uint8_t rotation = 0;

static uint16_t textcolor   = COLOR_WHITE;
static uint16_t textbgcolor = COLOR_WHITE;

static int16_t cursor_x = 0;
static int16_t cursor_y = 0;

static bool wrap = true;   // If set, 'wrap' text at right edge of display
static bool _cp437 = false; // If set, use correct CP437 charset (default is off)

uint16_t charBuffer[192]; // size == 5


//const unsigned char *pFontArr[] ={font5x7EnDOS, font5x7RUS};
unsigned char *pCurrentFont;
uint8_t currentFontNum =0;

//-------------------------------------------------------------------------------------------//

void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  if(r == 0)
    return;
  if (r == 1) {
    drawPixel(x0, y0, color);
    return;
  }
  
  int16_t x = -r, y = 0, err = 2-2*r, e2;
  
  do {
    drawPixel(x0-x, y0+y, color);
    drawPixel(x0+x, y0+y, color);
    drawPixel(x0+x, y0-y, color);
    drawPixel(x0-x, y0-y, color);
    
    e2 = err;
    if (e2 <= y) {
      err += ++y*2+1;
      if ((-x == y) && (e2 <= x)) e2 = 0;
    }
    if (e2 > x) err += ++x*2+1;
  } while (x <= 0);
}

void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  if(r == 0)
    return;
  if (r == 1) {
    drawPixel(x0, y0, color);
    return;
  }
  
  drawFastVLine(x0, y0-r, 2*r, color);
  
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;
  
  while (x<y) {
    if (f >= 0) {
      --y;
      ddF_y += 2;
      f     += ddF_y;
    }
    ++x;
    ddF_x += 2;
    f     += ddF_x;
    
    drawFastVLine(x0+x, y0-y, 2*y, color);
    drawFastVLine(x0+y, y0-x, 2*x, color);
    //left
    drawFastVLine(x0-x, y0-y, 2*y, color);
    drawFastVLine(x0-y, y0-x, 2*x, color);
  }
}

void drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;
  
  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    } 
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}

// Used to do circles and roundrects
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
{
  if (r == 0)
    return;
  if (r == 1) {
    drawPixel(x0, y0, color);
    return;
  }
  
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;
  
  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    
    if (cornername & 0x1) {
      drawFastVLine(x0+x, y0-y, 2*y+delta, color);
      drawFastVLine(x0+y, y0-x, 2*x+delta, color);
    }
    if (cornername & 0x2) {
      drawFastVLine(x0-x, y0-y, 2*y+delta, color);
      drawFastVLine(x0-y, y0-x, 2*x+delta, color);
    }
  }
}

// Bresenham's algorithm
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  if (y0 == y1) {
    if (x1 > x0) {
      drawFastHLine(x0, y0, x1 - x0, color);
    } else if (x1 < x0) {
      drawFastHLine(x1, y0, x0 - x1, color);
    } else {
      drawPixel(x0, y0, color);
    }
    return;
  } else if (x0 == x1) {
    if (y1 > y0) {
      drawFastVLine(x0, y0, y1 - y0, color);
    } else {
      drawFastVLine(x0, y1, y0 - y1, color);
    }
    return;
  }
  
  bool steep = abs(y1 - y0) > abs(x1 - x0);
  
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }
  
  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }
  
  int16_t dx = x1 - x0;
  int16_t dy = abs(y1 - y0);
  
  int16_t err = dx / 2;
  int16_t ystep;
  
  int16_t xbegin = x0;
  int16_t len;
  
  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }
  
  if (steep) {
    for (; x0<=x1; x0++) {
      err -= dy;
      if (err < 0) {
        len = x0 - xbegin;
        if (len) {
          drawFastVLine(y0, xbegin, len, color);
        } else {
          drawPixel(y0, x0, color);
        }
        xbegin = x0 + 1;
        y0 += ystep;
        err += dx;
      }
    }
    if (x0 > xbegin + 1) {
      drawFastVLine(y0, xbegin, x0 - xbegin, color);
    }
    
  } else {
    for (; x0<=x1; x0++) {
      err -= dy;
      if (err < 0) {
        len = x0 - xbegin;
        if (len) {
          drawFastHLine(xbegin, y0, len, color);
        } else {
          drawPixel(x0, y0, color);
        }
        xbegin = x0 + 1;
        y0 += ystep;
        err += dx;
      }
    }
    if (x0 > xbegin + 1) {
      drawFastHLine(xbegin, y0, x0 - xbegin, color);
    }
  }
  
}

// Draw a rectangle
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{  
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y+h-1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x+w-1, y, h, color);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  // rudimentary clipping (drawChar w/big text requires this)
  if((x >= _width) || (y >= _height)) return;
  if((x + w - 1) >= _width)  w = _width  - x;
  if((y + h - 1) >= _height) h = _height - y;
  
  setAddrWindow(x, y, x+w-1, y+h-1);
  
#if USE_FSMC
  for(y=h; y>0; y--) {
    for(x=w; x>0; x--) {
      FSMC_SEND_DATA(color);
    }
  }
#else
  fillColor_DMA1_SPI1(color, w*h);
#endif // USE_FSMC
}

// Draw a rounded rectangle
void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  if(r) { // if radius > 0
    // smarter version
    drawFastHLine(x+r  , y    , w-2*r, color); // Top
    drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
    drawFastVLine(x    , y+r  , h-2*r, color); // Left
    drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
    // draw four corners
    drawCircleHelper(x+r    , y+r    , r, 1, color);
    drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
    drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
    drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
  }
}

// Fill a rounded rectangle
void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  if(r) { // if radius > 0
    // smarter version
    fillRect(x+r, y, w-2*r, h, color);
    
    // draw four corners
    fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
    fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
  }
}

// Draw a triangle
void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}

// Fill a triangle
void fillTriangle ( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  int16_t a, b, y, last;
  
  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2) {
    swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }
  
  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    drawFastHLine(a, y0, b-a, color);
    return;
  }
  
  int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
  int32_t
    sa   = 0,
    sb   = 0;
  
  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it
  
  for(y=y0; y<=last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    drawFastHLine(a, y, b-a, color);
  }
  
  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) swap(a,b);
    drawFastHLine(a, y, b-a, color);
  }
}

void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
  int16_t i, j, byteWidth = (w + 7) / 8;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        drawPixel(x+i, y+j, color);
      }
    }
  }
}

// Draw a 1-bit color bitmap at the specified x, y position from the
// provided bitmap buffer (must be PROGMEM memory) using color as the
// foreground color and bg as the background color.
void drawBitmapBG(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg)
{
  int16_t i, j, byteWidth = (w + 7) / 8;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        drawPixel(x+i, y+j, color);
      } else {
      	drawPixel(x+i, y+j, bg);
      }
    }
  }
}

//Draw XBitMap Files (*.xbm), exported from GIMP,
//Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
//C Array can be directly used with this function
void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
  int16_t i, j, byteWidth = (w + 7) / 8;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i % 8))) {
        drawPixel(x+i, y+j, color);
      }
    }
  }
}

//=================================== Print section ========================//

void setCurrentFont(uint8_t num)
{
  /*
  if(num > (sizeof(pFontArr)/sizeof(pFontArr[0]))) {
    return;
  }
  pCurrentFont = (unsigned char*)pFontArr[num];
  */
  currentFontNum = num;
}

void print(const char *str)
{
  uint16_t size = strlen(str);
  
  while (size--) {
    printChar(*(const uint8_t *)str++);
  }
}

void printCharAt(int16_t x, int16_t y, uint8_t c)
{
  setCursor(x, y);
  printChar(c);
}

void printChar(uint8_t c)
{
  if (c == '\n') {
    cursor_y += textsize*8;
    cursor_x  = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
    cursor_x += textsize*6;
    if (wrap && (cursor_y > (_height - textsize*8))) {
      cursor_y = 0;
      cursor_x += textsize*5;
    }
    if (wrap && (cursor_x > (_width - textsize*6))) {
      cursor_y += textsize*8;
      cursor_x = 0;
    }
    
  }
}

// Draw a character
void drawChar(int16_t x, int16_t y, uint8_t c, uint16_t fgcolor, uint16_t bgcolor, uint8_t size)
{
  // Rudimentary clipping
  if((x >= _width)            || // Clip right
     (y >= _height)           || // Clip bottom
       ((x + 6 * size - 1) < 0) || // Clip left  TODO: this is not correct
         ((y + 8 * size - 1) < 0))   // Clip top   TODO: this is not correct
    return;
  
  if(!_cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior
  
  if (fgcolor == bgcolor) {
    uint8_t mask = 0x01;
    int16_t xoff, yoff;
    
    // This transparent approach is only about 20% faster
    // Don't need to clip here since the called rendering primitives all clip
    if (size == 1) {
      for (yoff=0; yoff < 8; yoff++) {
        uint8_t line = 0;
        for (xoff=0; xoff < 5; xoff++) {
          //if (pCurrentFont[c * 5 + xoff] & mask) line |= 1;
          if (font5x7EnDOS[c * 5 + xoff] & mask) line |= 1;
          line <<= 1;
        }
        line >>= 1;
        xoff = 0;
        while (line) {
          if (line == 0x1F) {
            drawFastHLine(x + xoff, y + yoff, 5, fgcolor);
            break;
          } else if (line == 0x1E) {
            drawFastHLine(x + xoff, y + yoff, 4, fgcolor);
            break;
          } else if ((line & 0x1C) == 0x1C) {
            drawFastHLine(x + xoff, y + yoff, 3, fgcolor);
            line <<= 4;
            xoff += 4;
          } else if ((line & 0x18) == 0x18) {
            drawFastHLine(x + xoff, y + yoff, 2, fgcolor);
            line <<= 3;
            xoff += 3;
          } else if ((line & 0x10) == 0x10) {
            drawPixel(x + xoff, y + yoff, fgcolor);
            line <<= 2;
            xoff += 2;
          } else {
            line <<= 1;
            xoff += 1;
          }
        }
        mask = mask << 1;
      }
    } else {
      for (yoff=0; yoff < 8; yoff++) {
        uint8_t line = 0;
        for (xoff=0; xoff < 5; xoff++) {
          //if (pCurrentFont[c * 5 + xoff] & mask) line |= 1;
          if (font5x7EnDOS[c * 5 + xoff] & mask) line |= 1;
          line <<= 1;
        }
        line >>= 1;
        xoff = 0;
        while (line) {
          if (line == 0x1F) {
            fillRect(x + xoff * size, y + yoff * size,
                        5 * size, size, fgcolor);
            break;
          } else if (line == 0x1E) {
            fillRect(x + xoff * size, y + yoff * size,
                        4 * size, size, fgcolor);
            break;
          } else if ((line & 0x1C) == 0x1C) {
            fillRect(x + xoff * size, y + yoff * size,
                        3 * size, size, fgcolor);
            line <<= 4;
            xoff += 4;
          } else if ((line & 0x18) == 0x18) {
            fillRect(x + xoff * size, y + yoff * size,
                        2 * size, size, fgcolor);
            line <<= 3;
            xoff += 3;
          } else if ((line & 0x10) == 0x10) {
            fillRect(x + xoff * size, y + yoff * size,
                        size, size, fgcolor);
            line <<= 2;
            xoff += 2;
          } else {
            line <<= 1;
            xoff += 1;
          }
        }
        mask = mask << 1;
      }
    }
  } else {
    // solid background
    
    setAddrWindow(x, y, x + 6 * size - 1, y + 8 * size - 1);
    
    uint8_t xr, yr;
    uint8_t mask = 0x01;
    uint16_t color;    
    uint8_t bufCount = 0;
    
    for (y=0; y < 8; y++) {
      for (yr=0; yr < size; yr++) {
        for (x=0; x < 5; x++) {
          //if (pCurrentFont[c * 5 + x] & mask) {
          if (font5x7EnDOS[c * 5 + x] & mask) {
            color = fgcolor;
          } else {
            color = bgcolor;
          }
          for (xr=0; xr < size; xr++) {
            charBuffer[bufCount++] = color;
          }
        }
        for (xr=0; xr < size; xr++) {
          charBuffer[bufCount++] = bgcolor;
        }
      }
      mask = mask << 1;
    }
    
    bufCount++;
    
#if USE_FSMC
    while(bufCount--) {
      FSMC_SEND_DATA(charBuffer[bufCount]); // fix this mirror in future
    }
#else
    sendData16_Fast_DMA1_SPI1(charBuffer, bufCount);
#endif /* USE_FSMC */
  }
}

uint16_t columns()
{
  return (width() / (textsize*6));
}

uint16_t rows()
{
  return (height() / (textsize*8));
}

void setCursor(int16_t x, int16_t y)
{
  cursor_x = x;
  cursor_y = y;
}

int16_t getCursorX(void)
{
  return cursor_x;
}

int16_t getCursorY(void)
{
  return cursor_y;
}

void setTextSize(uint8_t s)
{
  textsize = (s > 0) ? s : 1;
}

void setTextColor(uint16_t c)
{
  // For 'transparent' background, we'll set the bg 
  // to the same as fg instead of using a flag
  textcolor = textbgcolor = c;
}

void setTextColorBG(uint16_t c, uint16_t b)
{
  textcolor   = c;
  textbgcolor = b; 
}

void setTextWrap(bool w)
{
  wrap = w;
}
//==========================================================================//

// Enable (or disable) Code Page 437-compatible charset.
// There was an error in glcdfont.c for the longest time -- one character
// (#176, the 'light shade' block) was missing -- this threw off the index
// of every character that followed it.  But a TON of code has been written
// with the erroneous character indices.  By default, the library uses the
// original 'wrong' behavior and old sketches will still work.  Pass 'true'
// to this function to use correct CP437 character values in your code.
void cp437(bool x) {
  _cp437 = x;
}

// Return the size of the display (per current rotation)
int16_t width(void)
{
  return _width;
}

int16_t height(void)
{
  return _height;
}

/***************************************************************************/

void pushColor(uint16_t color)
{
#if USE_FSMC
  FSMC_SEND_DATA(color);
#else
  sendData16_SPI1(color);
#endif /* USE_FSMC */
}

void drawPixel(int16_t x, int16_t y, uint16_t color)
{
  if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
  
  setAddrPixel(x, y);
  
#if USE_FSMC
  FSMC_SEND_DATA(color);
#else
  sendData16_SPI1(color);
#endif /* USE_FSMC */
}

void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((y+h) >= _height) h = _height-y;
  
  setVAddrWindow(x, y, y+h);
  
#if USE_FSMC
  while (h--) {
    FSMC_SEND_DATA(color);
  }
#else
  fillColor_DMA1_SPI1(color, h);
#endif /* USE_FSMC */
}

void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((x+w) >= _width)  w = _width-x;
  
  setHAddrWindow(x, y, x+w);
  
#if USE_FSMC
  while (w--) {
    FSMC_SEND_DATA(color);
  }
#else
  fillColor_DMA1_SPI1(color, w);
#endif /* USE_FSMC */
}

void fillScreen(uint16_t color)
{
  setAddrWindow(0, 0, _width-1, _height-1);
  
  fillColor_DMA1_SPI1(color, _width * _height);
}

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// converts rgb 332 to rgb 565
uint16_t conv8to16(uint8_t x)
{
  uint16_t r = x>>5;
  uint16_t g = (x&0x1c)>2;
  uint16_t b = x&0x03;
  return (((r<<13) + (r<<10))&0xf800) + (g<<8) + (g<<5) + (b<<3) + (b<<1) + (b>>1);
}

/*
uint8_t getRotation(void)
{
return rotation;
}
*/
