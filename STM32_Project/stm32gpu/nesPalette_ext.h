#pragma once
#ifndef NESPALETTE_EXT_H
#define NESPALETTE_EXT_H

/*
 * Palette based on Typical Famicom NES (Dendy) Palette.
 * Consist of 80 colors (actually 75; 5 transparent colors 0x0F-0x4F).
 *
 * 8 Gray colors 0x0D-0x3D; 0x1E-0x4E
 *
 * 0x0E pure black.
 * 0x4D pure white.
 * Suggest to use 0x0F-0x4F section for transparent.
 *
 * Colors represented in RGB565 color space.
 *
 * Color adress: 0x00 - 0x4F
 */

//Size: 160 ( 80 * sizeof(uint16_t) )
static const uint16_t nesPalette_ext[] = {
  // 0x00-0x0F
  0x020C, 0x01F4, 0x0096, 0x4012, 0xA00B, 0xC005, 0xB820, 0x88A0,
  0x5960, 0x3220, 0x0240, 0x0226, 0x02CC, 0x8410, 0x0000, 0x0020,
  
  // 0x10-0x1F
  0x04D9, 0x03BF, 0x4ABF, 0x81BF, 0xC0B3, 0xE0E8, 0xF182, 0xD244,
  0xC300, 0x54C0, 0x0460, 0x04AB, 0x04D4, 0xC638, 0x2104, 0x0020,
  
  // 0x20-0x2F
  0x06FD, 0x059F, 0x44BD, 0x837F, 0xE23B, 0xF94D, 0xFB06, 0xF3E2,
  0xE5C3, 0x9EC1, 0x2668, 0x0E91, 0x06F9, 0xE71C, 0x31A6, 0x0020,
  
  // 0x30-0x3F
  0x07DF, 0x06FF, 0x751F, 0xAC1F, 0xEB1E, 0xFB33, 0xFCCE, 0xF589,
  0xFF04, 0xBF01, 0x2F86, 0x0F94, 0x07BB, 0xEF7D, 0x5AEB, 0x0020,
  
  // 0x40-0x4F
  0x9FFF, 0x875F, 0xA6BF, 0xDD5D, 0xFD5F, 0xFD56, 0xFE96, 0xFF34,
  0xFFB3, 0xD752, 0xA771, 0x8FB5, 0x9F9C, 0xFFFF, 0xDEFB, 0x0020,
};

#endif /* NESPALETTE_EXT_H */
