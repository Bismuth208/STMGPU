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

#ifndef _GPUWORKER_H
#define _GPUWORKER_H

//===========================================================================//

#define MAX_TEXT_SIZE   256
#define SYNC_SEQUENCE   0x42DD
#define SYNC_OK         0xCC

//===========================================================================//
#define MAX_FILL_BUF    90      // in percent, warning if buffer overfilled
#define MIN_FILL_BUF    5

#define CALC_BUF_FILL(a)   ((SERIAL_BUFFER_SIZE/100)*a)

// calculated value
#define CALC_MAX_FILL_SIZE      3800    // for 95 %   \ __ and SERIAL_BUFFER_SIZE = 4096 
#define CALC_MIN_FILL_SIZE      200     // for 5 %   /

// Buffer bsy indication, CPU MUST check this pin EVERYTIME before send any command!
// othervice, undefined behavor, can happen evething (buffer overflow, wrong commands)
#define GPU_BSY_PIN             GPIO_Pin_11
#define GPU_BSY_PORT            GPIOA

// mini stm32 board
//#define GPU_BSY_LED_PIN         GPIO_Pin_5
//#define GPU_BSY_LED_PORT        GPIOE
// maple mini board
#define GPU_BSY_LED_PIN         GPIO_Pin_13
#define GPU_BSY_LED_PORT        GPIOC


#define USE_BSY_PROTECTION      1
#define USE_HARD_BSY            0       // 1 - use GPIO, 0 - send message to CPU

#define BSY_MSG_CODE_WAIT       0xEE
#define BSY_MSG_CODE_READY      0xEA


//===========================================================================//
#define T_SELECT_WAY    "Selected interface: "
#define T_USART_WAY     "USART_1\n"
#define T_INIT_BUF      "Init command buffer... "
#define T_WAIT_SYNC     "Waiting for sync... "
#define T_TFT_SIZE      "Sending TFT size to host... "
#define T_GPU_START     "Start GPU...\n"

#define T_OK            "ok.\n"
#define T_FAIL          "fail.\n"

//===========================================================================//


//===========================================================================//

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

typedef struct {
  uint8_t cmdBufferStr[MAX_TEXT_SIZE];
  cmdBuffer_t cmdBuffer;
} cmdStructBuf_t;

//===========================================================================//

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
//#define NOT_USED        0x36
//#define NOT_USED        0x37
//#define NOT_USED        0x38
//#define NOT_USED        0x39
//#define NOT_USED        0x3A
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


// --------------- GUI commands -------------- //
#define SET_WND_CR      0x60    // Set window colors
#define DRW_WND         0x61    // draw window
#define DRW_WND_TXT     0x62    // draw window whith text
#define DRW_BTN_NUM     0x63    // draw numerated buttons
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

//===========================================================================//



void init_GPU(void);
void sync_CPU(void);
__noreturn __task void run_GPU(void);



/*
//void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
//void drawBitmapBG(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg);
//void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
*/

// ------------------------------------------------------------------- //


#endif /* _GPUWORKER_H */