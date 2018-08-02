#ifndef _STMSGPU_H
#define _STMSGPU_H

#include <stdint.h>
#include <stdarg.h>

#if ARDUINO >= 100
#include "Arduino.h"
#include "Print.h"
#else
#include "WProgram.h"
#endif
#ifdef __AVR
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#endif

#if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) \
  || defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
 #include <SoftwareSerial.h>
#endif

#include "sGPU_registers.h"

// Color definitions
#define COLOR_BLACK       0x0000      //   0,   0,   0
#define COLOR_NAVY        0x000F      //   0,   0, 128
#define COLOR_DARKGREEN   0x03E0      //   0, 128,   0
#define COLOR_DARKCYAN    0x03EF      //   0, 128, 128
#define COLOR_MAROON      0x7800      // 128,   0,   0
#define COLOR_PURPLE      0x780F      // 128,   0, 128
#define COLOR_OLIVE       0x7BE0      // 128, 128,   0
#define COLOR_DARKGREY    0x7BEF      // 128, 128, 128
#define COLOR_LIGHTGREY   0xC618      // 192, 192, 192
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
/*
 * WARNING!
 * THIS IS REALLY DENGEROUS DEFINE!
 * 
 * It's remove check for BSY pin!
 * This define created for reduce ROM size and RAM,
 * also increase speed of code execution by little.
 *
 */
#define REMOVE_HARDWARE_BSY  1
// ------------------------------------------------------------------- //
// ------------------------------------------------------------------- //
/*
 * WARNING!
 * THIS IS ALSO REALLY DENGEROUS DEFINE!
 *
 * It's remove check for returning resolution!
 * This define created for reduce ROM size and RAM,
 * also increase speed of code execution by little.
 *
 */
#define USE_GPU_RETURN_RESOLUTION  1

// Describe end resolution of LCD/TFT if its known
#if !USE_GPU_RETURN_RESOLUTION
 #define GPU_LCD_W   320
 #define GPU_LCD_H   240
#endif
// ------------------------------------------------------------------- //

#define TLE_8X8     1
#define TLE_16X16   2
#define TLE_32X32   3

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

// Srites for tiles 32x32 - avaliable only on sGPU PRO version!
#define SPR_1X1_32 8
#define SPR_1X2_32 9
#define SPR_2X1_32 10
#define SPR_2X2_32 11

// ------------------------------------------------------------------- //
// Move direction defines for 3D
#define MOVE_UP            0x01
#define MOVE_DOWN          0x02
#define MOVE_LEFT          0x04
#define MOVE_RIGHT         0x08
#define MOVE_CLOCKWISE_R   0x10
#define MOVE_CLOCKWISE_L   0x20

// definitions for texture sizes
#define TEXTURE_MODE_0     0  // 8x8 tiles
#define TEXTURE_MODE_1     1  // 16x16 tiles
#define TEXTURE_MODE_2     2  // 32x32 tiles (only PRO version!)

// ------------------------------------------------------------------- //
// DO NOT REMOVE THIS PRAGMA DIRECTIVE!!
#pragma pack(push, 1)
typedef union {
  uint8_t data[15];
  struct {
  	uint8_t  cmd;  // if no #pragma, then here will be hole in 1 byte...
    //uint16_t cmd;
    uint16_t par1;
    uint16_t par2;
    uint16_t par3;
    uint16_t par4;
    uint16_t par5;
    uint16_t par6;
    uint16_t par7;
  };
} cmdBuffer_t;
#pragma pack(pop)
// ------------------------------------------------------------------- //

// Baud rate speeds for serial
typedef enum {
  BAUD_SPEED_9600 = 9600,
  BAUD_SPEED_57600 = 57600,
  BAUD_SPEED_115200 = 115200,
  BAUD_SPEED_1M = 1000000
} baudSpeed_t;

   
class STMGPU : public Print {
  
public:
#if !REMOVE_HARDWARE_BSY
  STMGPU(int8_t bsyPin = -1);
#endif
  STMGPU();
  
  void  begin(baudSpeed_t baudRate = BAUD_SPEED_57600) __attribute__((optimize("-O2")));
  void  iDelay(uint16_t duty);

// ------------------ Base ------------------ //
  void  swReset(void),
        drawPixel(int16_t x, int16_t y, uint16_t color),
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
  
  void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color),
       drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg),
       drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color),
       drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg),
       drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
  
  uint16_t  color565(uint8_t r, uint8_t g, uint8_t b),
            conv8to16(uint8_t x);
  
  //uint8_t getRotation(void);
  
#if USE_GPU_RETURN_RESOLUTION
  void getResolution(void)  __attribute__((optimize("-O2")));
  int16_t width(void)  __attribute__((always_inline)) {return _width;}
  int16_t height(void) __attribute__((always_inline)) {return _height;}
#else
  void getResolution(void){}
  int16_t width(void)  __attribute__((always_inline)) {return GPU_LCD_W;}
  int16_t height(void) __attribute__((always_inline)) {return GPU_LCD_H;}
#endif
  
// --------------- Font/Print --------------- //

// get current cursor position (get rotation safe maximum values, using: width() for x, height() for y)
  //int16_t getCursorX(void);
  //int16_t getCursorY(void);
  //uint16_t columns(void);
  //uint16_t rows(void);
  
  //void setTextFont(unsigned char* f);
  void  drawChar(int16_t x, int16_t y, uint8_t c, uint16_t color, uint16_t bg, uint8_t size),
        setCursor(int16_t x, int16_t y),
        setTextColor(uint16_t color),
        setTextColor(uint16_t color, uint16_t bg),
        setTextSize(uint8_t size),
        setTextWrap(bool wrap),
        cp437(bool cp);
  
  virtual size_t write(uint8_t) __attribute__((optimize("-O2")));
  
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
  void sendTileData(uint8_t tileType, int16_t posX, int16_t posY, uint8_t tileNum) __attribute__((optimize("-O2")));
  void loadTileBase(uint8_t tileType, const char *tileSetArrName, uint8_t tileSetW,
                            uint8_t ramTileNum, uint8_t tileNum) __attribute__((optimize("-O2")));
  void loadTileBase(uint8_t tileType, const char *tileSetArrName, uint8_t tileSetW,
                    uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax) __attribute__((optimize("-O2")));
  // ---- tile 8x8 ---- //
  void loadTile8x8(const char *tileSetArrName, uint8_t tileSetW,
                            uint8_t ramTileNum, uint8_t tileNum);
  void loadTileSet8x8(const char *tileSetArrName, uint8_t tileSetW,
                                uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax);
  void drawTile8x8(int16_t posX, int16_t posY, uint8_t tileNum);
  
  // ---- tile 16x16 ---- //
  void loadTile16x16(const char *tileSetArrName, uint8_t tileSetW,
                   uint8_t ramTileNum, uint8_t tileNum);
  void loadTileSet16x16(const char *tileSetArrName, uint8_t tileSetW,
                      uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax);
  void drawTile16x16(int16_t posX, int16_t posY, uint8_t tileNum);
  // ---- tile 32x32 ---- //
  void loadTile32x32(const char *tileSetArrName, uint8_t tileSetW,
                     uint8_t ramTileNum, uint8_t tileNum);
  void loadTileSet32x32(const char *tileSetArrName, uint8_t tileSetW,
                        uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax);
  void drawTile32x32(int16_t posX, int16_t posY, uint8_t tileNum);
  
  // ---- tile universal ---- //
  //void drawTile(int16_t posX, int16_t posY, uint8_t tileType, uint8_t tileNum);
  
  
  void loadTileMap(const char *fileName);
  //void loadTileMap(const String &str);
  //void loadTileMap(const __FlashStringHelper* str);
  void drawTileMap(void);

// ----------------- Sprite ----------------- //
  void setSpritePosition(uint8_t sprNum, uint16_t posX, uint16_t posY);
  void setSpriteType(uint8_t sprNum, uint8_t type);
  void setSpriteVisible(uint8_t sprNum, uint8_t state);
  void setSpriteTiles(uint8_t sprNum, uint8_t tle1, uint8_t tle2, uint8_t tle3, uint8_t tle4);
  void setSpritesAutoRedraw(uint8_t state);
  void drawSprite(uint8_t sprNum);
  void drawSprite(uint8_t sprNum, uint16_t posX, uint16_t posY);
  bool getSpriteCollision(uint8_t sprNum1, uint8_t sprNum2);
  
  
// ----------------- SD card ---------------- //
  void loadPalette(const char *palleteArrName);
  void sendBaseBMP(uint16_t x, uint16_t y, uint16_t size) __attribute__((optimize("-O2"))); // Little help
  void printBMP(const char *fileName);
  void printBMP(const __FlashStringHelper* str);
  void printBMP(uint16_t x, uint16_t y, const String &str);
  void printBMP(uint16_t x, uint16_t y, const char *fileName);
  void printBMP(uint16_t x, uint16_t y, const __FlashStringHelper* str);
  
  
// ------------------ Sound ----------------- //
  void playNote(uint16_t freq, uint16_t duration);
  
  
// --------------- GUI commands -------------- //
  void setTextSizeGUI(uint8_t size);
  void setTextColorGUI(uint16_t text, uint16_t bg);
  void setColorWindowGUI(uint16_t frame, uint16_t border);
  
  void drawWindowGUI(int16_t posX, int16_t posY, int16_t w, int16_t h);
  void drawWindowGUI(int16_t posX, int16_t posY,
                         int16_t w, int16_t h, const char *text);
  void drawWindowGUI(int16_t posX, int16_t posY,
                         int16_t w, int16_t h, const __FlashStringHelper* str);
  
  
  // --------------- '3D' engine --------------- //
  void renderFrame(void);
  void moveCamera(uint8_t direction);
  void setCamPosition(uint16_t posX, uint16_t posY, uint16_t angle);
  void setWallCollision(bool state);
  void getCamPosition(uint16_t *pArrPos);
  // in future will be replaced by texture id;
  // also it will be ranamed to setFloorSkyTexture
  void setSkyFloor(uint16_t sky, uint16_t floor);

  // ------------------ Debug ----------------- //
  void setDebugGPIOState(bool state);

  
private:
#if defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) \
  || defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  
  SoftwareSerial *pSerial; // for boards which have no hardware serial port
#else
  HardwareSerial *pSerial; // because different boards have different serial ports
#endif
  
  void sendCommand(void *buf, uint8_t size) __attribute__((optimize("-O2")));
  void sendCommand(uint8_t cmd, uint8_t i, ...);
  
  // more RAM used but little faster, less ROM used and less problems whith stack
  cmdBuffer_t cmdBuffer;
  
#if !REMOVE_HARDWARE_BSY
#if defined (__AVR__) || defined(TEENSYDUINO) || defined (ARDUINO_ARCH_ARC32)
  int8_t  _bsyPin;
#elif defined (__arm__) || defined (ESP8266)
  int32_t _bsyPin;
#endif
  
  bool _useHardwareBsy;
#endif /* REMOVE_HARDWARE_BSY */

#if USE_GPU_RETURN_RESOLUTION
// at sync, sGPU return it`s LCD resolution,
// but you can ask sGPU once again
  int16_t _width;
  int16_t _height;
#endif
};

#endif /* _STMSGPU_H */
