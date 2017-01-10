#ifndef _SDCARD_H
#define _SDCARD_H

#ifdef __cplusplus
extern "C" {
#endif

  // ---------------- SD card ----------------- //
  void SDLoadPalette(const char *palleteArrName);
  void SDPrintBMP(uint16_t x, uint16_t y, const char* fileName);
  
#ifdef __cplusplus
}
#endif

#endif /* _SDCARD_H */
