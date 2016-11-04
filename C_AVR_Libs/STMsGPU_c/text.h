#ifndef _TEXT_H
#define _TEXT_H

#ifdef __cplusplus
extern "C" {
#endif
  
  // --------------- Font/Print --------------- //
  
  // get current cursor position (get rotation safe maximum values, using: width() for x, height() for y)
  //int16_t getCursorX(void);
  //int16_t getCursorY(void);
  
  //void setTextFont(unsigned char* f);
  void drawChar(int16_t x, int16_t y, uint8_t c, uint16_t color, uint16_t bg, uint8_t size);
  void setCursor(int16_t x, int16_t y);
  void setTextColor(uint16_t color);
  void setTextColorBG(uint16_t color, uint16_t bg);
  void setTextSize(uint8_t size);
  void setTextWrap(bool wrap);
  void cp437(bool cp);
  
  void print(const char *str);
  void tftPrintPGR(const char *str);
  void printChar(uint8_t c);
  void printCharPos(int16_t x, int16_t y, uint8_t c);
  
#ifdef __cplusplus
}
#endif

#endif /* _TEXT_H */
