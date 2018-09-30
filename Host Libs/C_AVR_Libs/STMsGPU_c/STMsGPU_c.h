#include <stdbool.h>

#ifndef _STMSGPU_C_H
#define _STMSGPU_C_H

#include "sGPU_registers.h"

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
#define USE_GPU_RETURN_RESOLUTION  0

// Describe end resolution of LCD/TFT if its known
#if !USE_GPU_RETURN_RESOLUTION
#define GPU_LCD_W   320
#define GPU_LCD_H   240
#endif
// ------------------------------------------------------------------- //

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

#ifdef __cplusplus
extern "C" {
#endif
  
#pragma pack(push, 1)
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
#pragma pack(pop)
  
  typedef enum {
    BAUD_SPEED_9600 = 9600,
    BAUD_SPEED_57600 = 57600,
    BAUD_SPEED_115200 = 115200,
    BAUD_SPEED_1M = 1000000
  } baudSpeed_t;
  
  // ------------------------------------------------------------------- //
  // This is really awesome and powerfull tool!
  // but it is not ready...
  //void printg(char *format, ...);
  //void printg_P(const char *format, ...);
  
  // ------------------------------------------------------------------- //
  
  void sync_gpu(uint32_t baud);
  void sendCommand(void *buf, uint8_t size);
  
  // ------------------ Base ------------------ //
  void gpuDrawPixel(int16_t x, int16_t y, uint16_t color);
  void gpuFillScreen(uint16_t color);
  
  // ------------- Primitives/GFX ------------- //
  void gpuFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void gpuDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void gpuDrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, uint16_t color);
  void gpuFillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, uint16_t color);
  void gpuDrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
  void gpuDrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void gpuDrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  
  void gpuDrawCircle(int16_t x, int16_t y, int16_t r, uint16_t color);
  void gpuFillCircle(int16_t x, int16_t y0, int16_t r, uint16_t color);
  void gpuDrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
  void gpuFillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
  
  void gpuScroll(uint16_t lines, uint16_t yStart);
  void gpuScrollSmooth(uint16_t lines, uint16_t yStart, uint8_t wait);
  
  void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
  void drawBitmapBG(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg);
  void gpuDrawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
  
  uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
  uint16_t conv8to16(uint8_t x);
  
  // --------------- Font/Print --------------- //
  
  // get current cursor position (get rotation safe maximum values, using: width() for x, height() for y)
  //int16_t getCursorX(void);
  //int16_t getCursorY(void);
  
  //void setTextFont(unsigned char* f);
  void gpuDrawChar(int16_t x, int16_t y, uint8_t c, uint16_t color, uint16_t bg, uint8_t size);
  void gpuSetCursor(int16_t x, int16_t y);
  void gpuSetTextColor(uint16_t color);
  void gpuSetTextColorBG(uint16_t color, uint16_t bg);
  void gpuSetTextSize(uint8_t size);
  void gpuSetTextWrap(bool wrap);
  void gpuSetCp437(bool cp);
  
  void gpuPrint(const char *str);
  void gpuPrint_P(const char *str);
  void gpuPrintChar(uint8_t c);
  void gpuPrintCharAt(int16_t x, int16_t y, uint8_t c);
  
  // ---------------- Low Level --------------- //
  void gpuSetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
  void gpuSetRotation(uint8_t m);
  void gpuSetScrollArea(uint16_t TFA, uint16_t BFA);
  void gpuScrollAddress(uint16_t VSP);
  void gpuSetSleep(bool enable);
  void gpuSetIdleMode(bool mode);
  void gpuSetDispBrightness(uint8_t brightness);
  void gpuSetInvertion(bool i);
  //void gpuSetGamma(uint8_t gamma);
  void gpuPushColor(uint16_t color);
  
  void gpuWriteCommand(uint8_t c);
  void gpuWriteData(uint8_t d);
  void gpuWriteWordData(uint16_t c);
  
  // ------------------- Tile ----------------- //
  // ---- tile 8x8 ---- //
  void gpuLoadTile8x8(const char *tileSetArrName, uint8_t tileSetW,
                   uint8_t ramTileNum, uint8_t tileNum);
  void gpuLoadTileSet8x8(const char *tileSetArrName, uint8_t tileSetW,
                      uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax);
  void gpuDrawTile8x8(int16_t posX, int16_t posY, uint8_t tileNum);
  
  // ---- tile 16x16 ---- //
  void gpuLoadTile16x16(const char *tileSetArrName, uint8_t tileSetW,
                     uint8_t ramTileNum, uint8_t tileNum);
  void gpuLoadTileSet16x16(const char *tileSetArrName, uint8_t tileSetW,
                        uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax);
  void gpuDrawTile16x16(int16_t posX, int16_t posY, uint8_t tileNum);
  // ---- tile 32x32 ---- //
  void gpuLoadTile32x32(const char *tileSetArrName, uint8_t tileSetW,
                     uint8_t ramTileNum, uint8_t tileNum);
  void gpuLoadTileSet32x32(const char *tileSetArrName, uint8_t tileSetW,
                        uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax);
  void gpuDrawTile32x32(int16_t posX, int16_t posY, uint8_t tileNum);
  
  
  void gpuLoadTileMap(const char *fileName);
  void gpuDrawBackgroundMap(void);
  
  // ----------------- Sprite ----------------- //
  void gpuSetSpritePosition(uint8_t sprNum, uint16_t posX, uint16_t posY);
  void gpuSetSpriteType(uint8_t sprNum, uint8_t type);
  void gpuSetSpriteVisible(uint8_t sprNum, uint8_t state);
  void gpuSetSpriteTiles(uint8_t sprNum, uint8_t tle1, uint8_t tle2, uint8_t tle3, uint8_t tle4);
  void gpuSetSpritesAutoRedraw(uint8_t state);
  void gpuDrawSprite(uint8_t sprNum);
  void gpuDrawSpriteAt(uint8_t sprNum, uint16_t posX, uint16_t posY);
  bool gpuGetSpriteCollision(uint8_t sprNum1, uint8_t sprNum2);
  
  // ---------------- SD card ----------------- //
  void gpuLoadPalette(const char *palleteArrName);
  void gpuPrintBMP(const char* fileName);
  void gpuPrintBMPat(uint16_t x, uint16_t y, const char* fileName);
  
  // ------------------ Sound ----------------- //
  void gpuPlayNote(uint16_t freq, uint16_t duration);
  
  // --------------- GUI commands -------------- //
  void gpuSetTextSizeGUI(uint8_t size);
  void gpuSetTextColorGUI(uint16_t text, uint16_t bg);
  void gpuSetColorWindowGUI(uint16_t frame, uint16_t border);
  
  void gpuDrawWindowGUI(int16_t posX, int16_t posY, int16_t w, int16_t h);
  void gpuDrawTextWindowGUI(int16_t posX, int16_t posY,
                     int16_t w, int16_t h, const char *text);
  void gpuDrawTextWindowGUI_P(int16_t posX, int16_t posY,
                     int16_t w, int16_t h, const char *text);
  
  // --------------- '3D' engine --------------- //
  void gpuRenderFrame(void);
  void gpuMoveCamera(uint8_t direction);
  void gpuSetCamPosition(uint16_t posX, uint16_t posY, uint16_t angle);
  void gpuSetWallCollision(bool state);
  void gpuGetCamPosition(uint16_t *pArrPos);
  // in future will be replaced by texture id;
  // also it will be ranamed to setFloorSkyTexture
  void gpuSetSkyFloor(uint16_t sky, uint16_t floor);

  // ------------------ General ----------------- //
  void setBusyMode(bool state);
  void swReset(void);
  void pingCommand(void);
  void setDebugGPIOState(bool state);
  
  
  // ------------------------------------------------------------------- //
  
  //uint16_t columns(void);
  //uint16_t rows(void);
  
  //uint8_t getRotation(void);
  
  void gpuGetResolution(void);
  int16_t gpuHeight(void);
  int16_t gpuWidth(void);
  
#ifdef __cplusplus
}
#endif


#endif /* _STMSGPU_C_H */
