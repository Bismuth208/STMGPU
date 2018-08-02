/*
 * author: Antonov Alexandr (Bismuth208)
 * created: ??.??.2016
 * last edit: 31.08.2018
 * 
 * This file contains protocol commands id for sGPU.
 *
 * All commands must be exact one byte, no more, no less!
 * All commands placed in range: 0x00 - 0xff;
 * All commands divided in logical subrenges;
 * All commands may contain on of shortcuts in name:
 *   CLR  - CLEAR
 *   FLL  - FILL
 *   DRW  - DRAW
 *   PSH  - PUSH
 *   CR   - COLOR
 *   PRNT - PRINT
 *   POS  - POSITION
 *   WRT  - WRITE
 *   LDD  - LOAD
 *   UPD  - UPDATE
 *   SCR  - SCREEN
 *   MAK  - MAKE
 *   TLE  - TILE
 *   SCRL - SCROLL
 *   SMTH - SMOOTH
 *
 * WARNING!
 * Any changes at any time are possible!
 * No backward capability is provided!
 *
 */

#ifndef _SGPU_REGISRERS_H
#define _SGPU_REGISRERS_H

#define T_SGPU_REGISRERS_VER "sGPU registers: v0.0.5"

//===========================================================================//

// -------------------------- Command list --------------------------- //
// ------------------ Base ------------------ //  
//#define NOT_USED         0x00
#define FLL_SCR         0x01
#define GPU_SW_RESET    0x02
#define DRW_PIXEL       0x03

// ------------- Primitives/GFX ------------- // 
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
#define DRW_PRNT_POS_C  0x13    // printCharAt()
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

// ---------------- Low Level --------------- //
#define SET_BRGHTNS_F   0x1F    // setDispBrightnessFade()
#define SET_ADR_WIN     0x20
#define SET_ROTATION    0x21
#define SET_SCRL_AREA   0x22
#define WRT_CMD         0x23    // writeCommand()
#define WRT_DATA        0x24    // writeData()
#define WRT_DATA_U16    0x25    // writeWordData()
#define SET_V_SCRL_ADR  0x26    // scrollAddress()
#define SET_SLEEP       0x27    // setSleep()
#define SET_IDLE        0x28    // setIdleMode()
#define SET_BRIGHTNES   0x29    // setDispBrightness()
#define SET_INVERTION   0x2A    // setInvertion()
#define SET_GAMMA       0x2B    // setGamma()
#define MAK_SCRL        0x2C    // scrollScreen()
#define MAK_SCRL_SMTH   0x2D    // scrollScreenSmooth()
#define PSH_CR          0x2E

// ------- BSY protect selection ------------ //
#define BSY_SELECT      0x2F

// ------------------- Tile ----------------- //
#define LDD_TLE_8       0x30    // load tile 8x8 size from SD
//#define NOT_USED        0x31
#define LDD_TLES_8      0x32    // load region of tiles 8x8 size from SD
#define DRW_TLE_8       0x33    // draw tile 8x8 size on TFT screen

#define LDD_TLE_16      0x34    // load tile 16x16 size from SD
//#define NOT_USED        0x35
#define LDD_TLES_16     0x36    // load region of tiles 16x16 size from SD
#define DRW_TLE_16      0x37    // draw tile 16x16 size on TFT screen

#define LDD_TLE_32      0x38    // load tile 32x32 size from SD
//#define NOT_USED        0x39
#define LDD_TLES_32     0x3A    // load region of tiles 32x32 size from SD
#define DRW_TLE_32      0x3B    // draw tile 32x32 size on TFT screen

#define LDD_TLE_MAP     0x3C    // load background tile map 8x8 from SD
#define DRW_TLE_MAP     0x3D    // draw background tile map 8x8 on TFT screen

//#define LDD_TLE_U       0x3E    // load specified tile size from SD
//#define DRW_TLE_U       0x3F    // draw specified tile size on TFT screen

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
#define DRW_BMP_FIL     0x51    // draw bmp file located on SD card
#define LDD_SND_FIL     0x52    // load sound file
//#define LDD_CSV_FIL     0x53    // load and exec *.csv file
//#define NOT_USED        0x54
//#define NOT_USED        0x55
//#define NOT_USED        0x56
//#define NOT_USED        0x57
//#define NOT_USED        0x58 // make screenshot to SD card
//#define NOT_USED        0x59
//#define NOT_USED        0x5A
//#define NOT_USED        0x5B 

// ------------------ Sound ----------------- //
//#define NOT_USED        0x5C
//#define NOT_USED        0x5D
#define SND_PLAY_TONE   0x5E
#define SND_PLAY_BUF    0x5F

// --------------- GUI commands -------------- //
#define SET_WND_CR      0x60    // Set window colors
#define SET_WND_CR_TXT  0x61    // set colors for GUI text
#define SET_WND_TXT_SZ  0x62    // set GUI text size
#define DRW_WND_AT      0x63    // draw window at position
#define DRW_WND_TXT     0x64    // draw window whith text
//#define DRW_BTN_NUM     0x65    // draw numerated buttons

// --------------- '3D' engine --------------- //
#define SET_BACKGRND      0x66
//#define NOT_USED        0x67
#define RENDER_MAP        0x68  // render walls
#define MOVE_CAMERA       0x69
#define SET_CAM_POS       0x6A  // set current camera position
//#define RENDER_BCKGRND    0x6B  // render background; sky, floor
#define SET_RENDER_QA     0x6C  // set render quality
#define SET_TEXTURE_MODE  0x6D  // 8x8, 16x16 or 32x32(pro only)
#define SET_WALL_CLD      0x6E  // set wall collision state
#define GET_CAM_POS       0x6F  // get current camera position
#define SET_RENDER_FPS    0x70  // set FPS limit to render

// ---------------- NOT_USED ---------------- //
// -------------- 0x80 - 0xFE --------------- //

// ------------------ Debug ----------------- //
#define SET_DBG_GPIO_PIN    0xFF

//===========================================================================//
#endif /* _SGPU_REGISRERS_H */
