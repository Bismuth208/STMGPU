#ifndef _PRIMITIVES_H
#define _PRIMITIVES_H

#ifdef __cplusplus
extern "C" {
#endif
  
  // ------------- Primitives/GFX ------------- //
  void tftFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void tftDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, uint16_t color);
  void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, uint16_t color);
  void tftDrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
  void tftDrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void tftDrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  
  void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color);
  void fillCircle(int16_t x, int16_t y0, int16_t r, uint16_t color);
  void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
  void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
  
#ifdef __cplusplus
}
#endif

#endif /* _PRIMITIVES_H */
