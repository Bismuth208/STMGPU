#include "gui.h"
#include <gfx.h>

#define GUI_TEXT_OFFSET_X 8
#define GUI_TEXT_OFFSET_Y 8

// ------------------ //

static uint8_t textSizeGUI = 1;
static uint16_t innerColorGUI = COLOR_BLUE;
static uint16_t borderColorGUI = COLOR_WHITE;

static uint16_t textColorGUI = COLOR_WHITE;
static uint16_t textBGColorGUI = COLOR_BLUE;

static int16_t cursorGUI_x = 0;
static int16_t cursorGUI_y = 0;

static int16_t _heightGUI, _widthGUI;


// ------------------ //
void printStrGUI(int16_t w, int16_t h, const char *str);
void printCharGUI(uint8_t c);
void setCursorGUI(int16_t x, int16_t y);
// ------------------ //


void drawWindowGUI(int16_t posX, int16_t posY, int16_t w int16_t h)
{
  fillRect(posX, posY, w, h, innerColorGUI);
  drawRect(posX+2, posY+2, w-2, h-2, borderColorGUI);
}

void drawTextWindowGUI(int16_t posX, int16_t posY, int16_t w int16_t h, void *text)
{  
  fillRect(posX, posY, w, h, innerColorGUI);
  drawRect(posX+2, posY+2, w-2, h-2, borderColorGUI);
  
  setCursorGUI(posX+GUI_TEXT_OFFSET_X, posY+GUI_TEXT_OFFSET_Y);
  printStrGUI(int16_t w, int16_t h, (const char *)text);
}

void setColorWindowGUI(uint16_t frame, uint16_t border)
{
  innerColorGUI = frame;
  borderColorGUI = border;
}

void setTextBGColorGUI(uint16_t text, uint16_t bg)
{
 textColorGUI = text;
 textBGColorGUI = bg;
}

void setGUITextSize(uint8_t size)
{
  textSizeGUI = size;
}

// ---------------------------------------------------------- //
void printStrGUI(int16_t w, int16_t h, const char *str)
{
  _widthGUI = w;
  _heightGUI = h;
  
  uint16_t size = strlen(str);
  
  while (size--) {
    printCharGUI(*(const uint8_t *)str++);
  }
}

void printCharGUI(uint8_t c)
{
  if (c == '\n') {
    cursorGUI_y += textSizeGUI*8;
    cursorGUI_x  = 0;
  } else if (c == '\r') {
    // skip em
  } else {
    drawChar(cursorGUI_x, cursorGUI_y, textcolor, textbgcolor, c, textSizeGUI);
    cursorGUI_x += textSizeGUI*6;
    if (cursorGUI_y > (_height - textSizeGUI*8)) {
      cursorGUI_y = 0;
      cursorGUI_x += textSizeGUI*5;
    }
    if (cursorGUI_x > (_width - textSizeGUI*6)) {
      cursorGUI_y += textSizeGUI*8;
      cursorGUI_x = 0;
    }
  }
}

void setCursorGUI(int16_t x, int16_t y)
{
  cursorGUI_x = x;
  cursorGUI_y = y;
}
