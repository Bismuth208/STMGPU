// -------------  TO DO: ---------- //
/*
*
*
*
*
*       MAKE SOME MAGIC
*            HERE
*
*
*
*/
// -------------------------------- //

#pragma once
#ifndef GPUWORKER_H
#define GPUWORKER_H

// -------------------------------- //

typedef union {
  uint8_t data[14];
  struct {
    uint16_t par1;
    uint16_t par2;
    uint16_t par3;
    uint16_t par4;
    uint16_t par5;
    uint16_t par6;
    uint16_t par7;
  };
} cmdBuffer_t;

typedef union {
  uint8_t data[10];
  struct {
    uint8_t  par0;
    uint16_t par1;
    uint16_t par2;
    uint16_t par3;
    uint16_t par4;
    uint8_t par5;
  };
} cmdBuffer2_t;
// -------------------------------- //

void init_GPU(void);
void sync_CPU(void);
__noreturn __task void run_GPU(void);

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
#define GET_RESOLUTION  0x0F  //

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


// --------------- Tile/Sprite -------------- //
#define LDD_TLE_8       0x30    // load tile 8x8 size from SD
#define LDD_TLES_8      0x31    // load tiles 8x8 size from SD
#define LDD_TLES_RG_8   0x32    // load region of tiles 8x8 size from SD
#define DRW_TLE_8_POS   0x33    // draw tile 8x8 size on TFT screen
//#define SET_TLE_POS     0x34    // set tile position in tile screen
//#define LDD_TLE_SCR     0x35    // load tile screen from SD
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


// ---------------- NOT_USED ---------------- //
//#define NOT_USED        0x40
//#define NOT_USED        0x41
//#define NOT_USED        0x42
//#define NOT_USED        0x43
//#define NOT_USED        0x44
//#define NOT_USED        0x45
//#define NOT_USED        0x46
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
//#define NOT_USED        0x50
//#define NOT_USED        0x51
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


// --------------- GUI commands -------------- //
//#define SET_WND_CR        0x60          // Set window colors
//#define DRW_WND        0x61           // draw window
//#define DRW_WND_TXT        0x62       // draw window whith text
//#define NOT_USED        0x63
//#define NOT_USED        0x64
//#define NOT_USED        0x65
//#define NOT_USED        0x66
//#define NOT_USED        0x67
//#define NOT_USED        0x68
//#define NOT_USED        0x69
//#define NOT_USED        0x6A
//#define NOT_USED        0x6B
//#define NOT_USED        0x6C
//#define NOT_USED        0x6D
//#define NOT_USED        0x6E
//#define NOT_USED        0x6F

// ---------------- NOT_USED ---------------- //
// -------------- 0x70 - 0xFF --------------- //



/*
//void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
//void drawBitmapBG(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg);
//void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
*/

// ------------------------------------------------------------------- //


#endif /* GPUWORKER_H */