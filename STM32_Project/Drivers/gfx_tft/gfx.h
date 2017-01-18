#ifndef _GFX_H
#define _GFX_H

#include <stdbool.h>
#include <stdint.h>

#include "ili9341.h"


// Color definitions
#define COLOR_BLACK       0x0000      //   0,   0,   0
#define COLOR_NAVY        0x000F      //   0,   0, 128
#define COLOR_DARKGREEN   0x03E0      //   0, 128,   0
#define COLOR_DARKCYAN    0x03EF      //   0, 128, 128
#define COLOR_MAROON      0x7800      // 128,   0,   0
#define COLOR_PURPLE      0x780F      // 128,   0, 128
#define COLOR_OLIVE       0x7BE0      // 128, 128,   0
#define COLOR_LIGHTGREY   0xC618      // 192, 192, 192
#define COLOR_DARKGREY    0x7BEF      // 128, 128, 128
#define COLOR_BLUE        0x001F      //   0,   0, 255
#define COLOR_GREEN       0x07E0      //   0, 255,   0
#define COLOR_CYAN        0x07FF      //   0, 255, 255
#define COLOR_GREENYELLOW 0xAFE5
#define COLOR_RED         0xF800      // 255,   0,   0
#define COLOR_MAGENTA     0xF81F      // 255,   0, 255
#define COLOR_ORANGE      0xFD20      // 255, 165,   0
#define COLOR_YELLOW      0xFFE0      // 255, 255,   0
#define COLOR_WHITE       0xFFFF      // 255, 255, 255

//-------------------------------------------------------------------------------------------//

void pushColor(uint16_t color);
void drawPixel(int16_t x, int16_t y, uint16_t color);
void fillScreen(uint16_t color);

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
void drawBitmapBG(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg);
void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);

uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
uint16_t conv8to16(uint8_t x);
  
int16_t height(void);
int16_t width(void);
    
uint16_t columns(void);
uint16_t rows(void);

//uint8_t getRotation(void);

// get current cursor position (get rotation safe maximum values, using: width() for x, height() for y)
int16_t getCursorX(void);
int16_t getCursorY(void);
  
//void setTextFont(unsigned char* f);
void drawChar(int16_t x, int16_t y, uint16_t color, uint16_t bg, uint8_t c, uint8_t size);
void setCursor(int16_t x, int16_t y);
void setTextColor(uint16_t c);
void setTextColorBG(uint16_t c, uint16_t b);
void setTextSize(uint8_t s);
void setTextWrap(bool w);
void cp437(bool x);

void setCurrentFont(uint8_t num);

void print(const char *str);
void printStr(void *str, uint16_t size);
void printChar(uint8_t c);
void printCharAt(int16_t x, int16_t y, uint8_t c);

#endif /* _GFX_H */
