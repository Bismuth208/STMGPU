#include <stdbool.h>

#ifndef _STMSGPU_C_H
#define _STMSGPU_C_H

#include "tiles.h"
#include "sprites.h"
#include "lowlevel.h"
#include "text.h"
#include "primitives.h"
#include "sdcard.h"


// Check busy GPU`s pin before every transfer
// Protect GPU`s buffer from overflow
#define USE_BSY_PIN 0

// This is which pin CPU must check for attached bsy line from GPU
// PD2 is digital pin D2 on arduino
#define CHK_GPU_BSY_DDRX  DDRD
#define CHK_GPU_BSY_PORTX PORTD
#define CHK_GPU_BSY_PINX  PIND
#define CHK_GPU_BSY_PXY   PD2

#define CHK_GPU_BSY_PIN     (CHK_GPU_BSY_PINX & (1 << CHK_GPU_BSY_PXY))

// -------------------------- Command list --------------------------- //
// CLR  - CLEAR
// FLL  - FILL
// DRW  - DRAW
// PSH  - PUSH
// CR   - COLOR
// PRNT - PRINT
// POS  - POSITION
// WRT  - WRITE
// LDD  - LOAD
// UPD  - UPDATE
// SCR  - SCREEN
// MAK  - MAKE
// TLE  - TILE
// SCRL - SCROLL
// SMTH - SMOOTH

// ------------------ Base ------------------ //
//#define NOT_USED         0x00
#define FLL_SCR         0x01
//#define NOT_USED          0x02
#define DRW_PIXEL       0x03

// ------------- Primitives/GFX ------------- ///
#define FLL_RECT        0x04
#define DRW_RECT        0x05
#define DRW_ROUND_RECT  0x06
#define FLL_ROUND_RECT  0x07
#define DRW_LINE        0x08
#define DRW_V_LINE      0x09
#define DRW_H_LINE      0x0A
#define DRW_CIRCLE      0x0B
#define FLL_CIRCLE      0x0C
#define DRW_TRINGLE     0x0D
#define FLL_TRINGLE     0x0E
#define GET_RESOLUTION  0x0F

// --------------- Font/Print --------------- //
#define DRW_CHAR        0x10    // drawChar()
#define DRW_PRNT        0x11    // print()
#define DRW_PRNT_C      0x12    // printChar()
#define DRW_PRNT_POS_C  0x13    // printCharPos()
#define SET_CURSOR      0x14    // setCursor()
#define SET_TXT_CR      0x15    // setTextColor()
#define SET_TXT_CR_BG   0x16    // setTextColorBG()
#define SET_TXT_SIZE    0x17    // setTextSize()
#define SET_TXT_WRAP    0x18    // setTextWrap()
#define SET_TXT_437     0x19    // cp437()
//#define SET_TXT_FONT    0x1A    // setTextFont()
//#define NOT_USED        0x1B
//#define NOT_USED        0x1C
//#define NOT_USED        0x1D
//#define NOT_USED        0x1E
//#define NOT_USED        0x1F


// ---------------- Low Level --------------- //
#define SET_ADR_WIN     0x20
#define SET_ROTATION    0x21
#define SET_SCRL_AREA   0x22
#define WRT_CMD         0x23    // writeCommand()
#define WRT_DATA        0x24    // writeData()
#define WRT_DATA_U16    0x25    // writeWordData()
#define SET_V_SCRL_ADR  0x26    // tftScrollAddress()
#define SET_SLEEP       0x27    // tftSetSleep()
#define SET_IDLE        0x28    // tftSetIdleMode()
#define SET_BRIGHTNES   0x29    // tftSetDispBrightness()
#define SET_INVERTION   0x2A    // tftSetInvertion()
#define SET_GAMMA       0x2B    // setGamma()
#define MAK_SCRL        0x2C    // tftScroll()
#define MAK_SCRL_SMTH   0x2D    // tftScrollSmooth()
#define PSH_CR          0x2E
//#define NOT_USED        0x2F


// ------------------- Tile ----------------- //
#define LDD_TLE_8       0x30    // load tile 8x8 size from SD
#define LDD_TLES_8      0x31    // load tiles 8x8 size from SD
#define LDD_TLES_RG_8   0x32    // load region of tiles 8x8 size from SD
#define DRW_TLE_8_POS   0x33    // draw tile 8x8 size on TFT screen
#define LDD_TLE_MAP     0x34    // load background tile map 8x8 from SD
#define DRW_TLE_MAP     0x35    // draw background tile map 8x8 on TFT screen
//#define DRW_TLE_SCR     0x36    // draw tile screen on TFT screen
//#define MAK_METTLE      0x37    // group tiles to metatile
//#define DRW_METTLE_SCR  0x38    // draw metatile on screen
//#define DRW_METTLE_POS  0x39    // draw metatile in tile screen
//#define DRW_METSCR_POS  0x3A    // draw all tile screens on screen
//#define NOT_USED        0x3B
//#define NOT_USED        0x3C
//#define NOT_USED        0x3D
//#define NOT_USED        0x3E
//#define NOT_USED        0x3F


// ----------------- Sprite ----------------- //
#define SET_SPR_POS     0x40    // set sprite position
#define SET_SPR_TYPE    0x41    // set sprite type 1x2:8, 2x2:8; 1x2:16, 2x2:16;
#define SET_SPR_VISBL   0x42    // enable draw on screen
#define SET_SPR_TLE     0x43    // set tiles for sprite
#define SET_SPR_AUT_R   0x44    // enable or disable autoredraw sprite
#define DRW_SPR         0x45    // draw sprite
#define GET_SRP_COLISN  0x46    // get sprites collision
//#define NOT_USED        0x47
//#define NOT_USED        0x48
//#define NOT_USED        0x49
//#define NOT_USED        0x4A
//#define NOT_USED        0x4B
//#define NOT_USED        0x4C
//#define NOT_USED        0x4D
//#define NOT_USED        0x4E
//#define NOT_USED        0x4F


// ----------------- SD card ---------------- //
#define LDD_USR_PAL     0x50    // load user palette from SD card
#define DRW_MBP_FIL     0x51    // draw bmp file located on SD card
//#define NOT_USED        0x52
//#define NOT_USED        0x53
//#define NOT_USED        0x54
//#define NOT_USED        0x55
//#define NOT_USED        0x56
//#define NOT_USED        0x57
//#define NOT_USED        0x58
//#define NOT_USED        0x59
//#define NOT_USED        0x5A
//#define NOT_USED        0x5B
//#define NOT_USED        0x5C
//#define NOT_USED        0x5D
//#define NOT_USED        0x5E
//#define NOT_USED        0x5F


// ---------------- NOT_USED ---------------- //
// -------------- 0x60 - 0xFF --------------- //


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

// ------------------------------------------------------------------- //

// Sprites for tiles 8x8
#define SPR_1X1_8 0
#define SPR_1X2_8 1
#define SPR_2X1_8 2
#define SPR_2X2_8 3

// Sprites for tiles 16x16
#define SPR_1X1_16 4
#define SPR_1X2_16 5
#define SPR_2X1_16 6
#define SPR_2X2_16 7

// Srites for tiles 32x32 - avaliable only on GPU PRO version!
#define SPR_1X1_32 8
#define SPR_1X2_32 9
#define SPR_2X1_32 10
#define SPR_2X2_32 11

// ------------------------------------------------------------------- //

#ifdef __cplusplus
extern "C" {
#endif
  
  //#pragma pack(push, 1)
  typedef union {
    uint8_t data[15];
    struct {
      uint8_t  cmd;
      uint16_t par1;
      uint16_t par2;
      uint16_t par3;
      uint16_t par4;
      uint16_t par5;
      uint16_t par6;
      uint16_t par7;
    };
  } cmdBuffer_t;
  //#pragma pack(pop)
  
  // ------------------------------------------------------------------- //
  
  void sync_gpu(void);
  void sendCommand(void *buf, uint8_t size);
  
  // ------------------ Base ------------------ //
  void tftDrawPixel(int16_t x, int16_t y, uint16_t color);
  void tftFillScreen(uint16_t color);
  
  
  // ------------------------------------------------------------------- //
  
  void tftScroll(uint16_t lines, uint16_t yStart);
  void tftScrollSmooth(uint16_t lines, uint16_t yStart, uint8_t wait);
  
  //void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
  //void drawBitmapBG(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg);
  void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
  
  uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
  uint16_t conv8to16(uint8_t x);
  
  //uint16_t columns(void);
  //uint16_t rows(void);
  
  //uint8_t getRotation(void);
  
  void getResolution(void);
  int16_t tftHeight(void);
  int16_t tftWidth(void);
  
  
#ifdef __cplusplus
}
#endif


#endif /* _STMSGPU_C_H */
