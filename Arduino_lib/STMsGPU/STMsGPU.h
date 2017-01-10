#ifndef _STMSGPU_H
#define _STMSGPU_H

#include <stdint.h>

#if ARDUINO >= 100
#include "Arduino.h"
#include "Print.h"
#else
#include "WProgram.h"
#endif
//#include <Adafruit_GFX.h>
#ifdef __AVR
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#endif

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
//#define NOT_USED        0x1F


// ---------------- Low Level --------------- //
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
#define LDD_TLES_8      0x31    // load tiles 8x8 size from SD
#define LDD_TLES_RG_8   0x32    // load region of tiles 8x8 size from SD
#define DRW_TLE_8       0x33    // draw tile 8x8 size on TFT screen

#define LDD_TLE_16      0x34    // load tile 16x16 size from SD
#define LDD_TLES_16     0x35    // load tiles 16x16 size from SD
#define LDD_TLES_RG_16  0x36    // load region of tiles 16x16 size from SD
#define DRW_TLE_16      0x37    // draw tile 16x16 size on TFT screen

#define LDD_TLE_32      0x38    // load tile 32x32 size from SD
#define LDD_TLES_32     0x39    // load tiles 32x32 size from SD
#define LDD_TLES_RG_32  0x3A    // load region of tiles 32x32 size from SD
#define DRW_TLE_32      0x3B    // draw tile 32x32 size on TFT screen

#define LDD_TLE_MAP     0x3C    // load background tile map 8x8 from SD
#define DRW_TLE_MAP     0x3D    // draw background tile map 8x8 on TFT screen

#define LDD_TLE_U       0x3E    // load specified tile size from SD
#define DRW_TLE_U       0x3F    // draw specified tile size on TFT screen

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
   
typedef union {
  uint8_t data[12];
  struct {
    uint8_t cmd;
    uint8_t  par0;
    uint16_t par1;
    uint16_t par2;
    uint16_t par3;
    uint16_t par4;
    uint8_t  par5;
    //uint8_t  align;
  };
} cmdBuffer2_t;
//#pragma pack(pop)
// ------------------------------------------------------------------- //

typedef enum {
  USART_BAUD_9600 = 9600,
  USART_BAUD_57600 = 57600,
  USART_BAUD_115200 = 115200,
  USART_BAUD_1M = 1000000
} baudSpeed_t;
   
class STMGPU : public Print {
  
public:
  
  STMGPU(int8_t bsyPin);
  STMGPU();
  
  void  begin(uint32_t baudRate),
        sendCommand(void *buf, uint8_t size);

// ------------------ Base ------------------ //
  void  drawPixel(int16_t x, int16_t y, uint16_t color),
        fillScreen(uint16_t color);
   
// ------------- Primitives/GFX ------------- //
  void  fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
        drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
        drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, uint16_t color),
        fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, uint16_t color),
        drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color),
        drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color),
        drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color),
  
        drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color),
        fillCircle(int16_t x, int16_t y0, int16_t r, uint16_t color),
        drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color),
        fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
  
  uint16_t  scroll(uint16_t lines, uint16_t yStart),
            scrollSmooth(uint16_t lines, uint16_t yStart, uint8_t wait);
  
  //void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
  //void drawBitmapBG(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg);
  void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
  
  uint16_t  color565(uint8_t r, uint8_t g, uint8_t b),
            conv8to16(uint8_t x);
  
  //uint16_t columns(void);
  //uint16_t rows(void);
  
  //uint8_t getRotation(void);
  
  void getResolution(void);
  int16_t width(void);
  int16_t height(void);
   
// --------------- Font/Print --------------- //

// get current cursor position (get rotation safe maximum values, using: width() for x, height() for y)
  //int16_t getCursorX(void);
  //int16_t getCursorY(void);
  
  //void setTextFont(unsigned char* f);
  void  drawChar(int16_t x, int16_t y, uint8_t c, uint16_t color, uint16_t bg, uint8_t size),
        setCursor(int16_t x, int16_t y),
        setTextColor(uint16_t color),
        setTextColor(uint16_t color, uint16_t bg),
        setTextSize(uint8_t size),
        setTextWrap(bool wrap),
        cp437(bool cp);
  
#if ARDUINO >= 100
  virtual size_t write(uint8_t);
#else
  virtual void   write(uint8_t);
#endif
  
  //virtual size_t print(const char[]);
  
  void printAt(int16_t x, int16_t y, char),
       printAt(int16_t x, int16_t y, const char *str),
       printAt(int16_t x, int16_t y, const String &str),
       printAt(int16_t x, int16_t y, const __FlashStringHelper* str);

// ---------------- Low Level --------------- //
  void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1),
       setRotation(uint8_t m),
       setScrollArea(uint16_t TFA, uint16_t BFA),
       scrollAddress(uint16_t VSP),
       setSleep(bool enable),
       setIdleMode(bool mode),
       setDispBrightness(uint8_t brightness),
       setInvertion(bool i),
  //     setGamma(uint8_t gamma),
       pushColor(uint16_t color);
  
  void writeCommand(uint8_t c),
       writeData(uint8_t d),
       writeWordData(uint16_t c);
  
// ------------------- Tile ----------------- //
  void loadTileFromSet8x8(const char *tileSetArrName, uint8_t tileSetW,
                            uint8_t ramTileNum, uint8_t tileNum);
  void loadTileSet8x8(const char *tileSetArrName, uint8_t tileSetW,
                        uint8_t ramTileBase, uint8_t tileMax);
  void loadRegionOfTileSet8x8(const char *tileSetArrName, uint8_t tileSetW,
                                uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax);
  void drawTile8x8(int16_t posX, int16_t posY, uint8_t tileNum);
  void loadTileMap(const char *fileName);
  //void loadTileMap(const String &str);
  //void loadTileMap(const __FlashStringHelper* str);
  void drawTileMap(void);

// ----------------- Sprite ----------------- //
  void setSpritePosition(uint8_t sprNum, uint16_t posX, uint16_t posY);
  void setSpriteType(uint8_t sprNum, uint8_t type);
  void setSpriteVisible(uint8_t sprNum, uint8_t state);
  void setSpriteTiles(uint8_t sprNum, uint8_t tle1, uint8_t tle2,
                      uint8_t tle3, uint8_t tle4);
  void setSpritesAutoRedraw(uint8_t state);
  void drawSprite(uint8_t sprNum);
  void drawSprite(uint8_t sprNum, uint16_t posX, uint16_t posY);
  bool getSpriteCollision(uint8_t sprNum1, uint8_t sprNum2);
  
  
// ----------------- SD card ---------------- //
  void printBMP(const char *fileName);
  void printBMP(const __FlashStringHelper* str);
  void printBMP(uint16_t x, uint16_t y, const String &str);
  void printBMP(uint16_t x, uint16_t y, const char *fileName);
  void printBMP(uint16_t x, uint16_t y, const __FlashStringHelper* str);
  
private:
  // more RAM used but little faster, less ROM used and less problems whith stack
  cmdBuffer_t cmdBuffer;
  
#if defined (__AVR__) || defined(TEENSYDUINO)
  int8_t  _bsyPin;
#elif defined (__arm__)
  int32_t _bsyPin;
#elif defined (ARDUINO_ARCH_ARC32)
  int8_t  _bsyPin;
#elif defined (ESP8266)
  int32_t  _bsyPin;
#endif

// at sync, GPU return it`s LCD resolution,
// but you can ask GPU once again
  int16_t _width;
  int16_t _height;
  bool _useHardwareBsy;
};

#endif /* _STMSGPU_H */
