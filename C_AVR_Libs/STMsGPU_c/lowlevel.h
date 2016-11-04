#ifndef _LOWLEVEL_H
#define _LOWLEVEL_H

#ifdef __cplusplus
extern "C" {
#endif

  // ---------------- Low Level --------------- //
  void tftSetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
  void tftSetRotation(uint8_t m);
  void tftSetScrollArea(uint16_t TFA, uint16_t BFA);
  void tftScrollAddress(uint16_t VSP);
  void tftSetSleep(bool enable);
  void tftSetIdleMode(bool mode);
  void tftSetDispBrightness(uint8_t brightness);
  void tftSetInvertion(bool i);
  //void setGamma(uint8_t gamma);
  void tftPushColor(uint16_t color);
  
  void writeCommand(uint8_t c);
  void writeData(uint8_t d);
  void writeWordData(uint16_t c);
  
#ifdef __cplusplus
}
#endif

#endif /* _LOWLEVEL_H */
