/*
 *
 * For STM32_sGPU Project
 * Created: 10.04.2016
 * Last edit: 12.08.2018
 *
 * author: Antonov Alexandr (Bismuth208)
 *
 * For addition info look read.me
 *
 */

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define strlen_P strlen
#endif

#include <uart.h>
#include <systicktimer.h>

#include "STMsGPU_c.h"

#define MAX_TEXT_SIZE   30

// ------------------------------------------------------------------------------------ //
// more RAM used but little faster, less ROM used and less problems whith stack
static cmdBuffer_t cmdBuffer;
//static uint8_t cmdBufferStr[MAX_TEXT_SIZE];

#if USE_GPU_RETURN_RESOLUTION
// at sync, sGPU return it`s LCD resolution,
// but you can ask sGPU once again
static int16_t _width  = 0;
static int16_t _height = 0;
#endif

// ------------------------------------------------------------------------------------ //

void sync_gpu(uint32_t baud)
{
  uint8_t syncData[2] = { 0x42, 0xDD};

  bool syncEstablished = false;

#if !REMOVE_HARDWARE_BSY
  // setup GPU bsy pin
  CHK_GPU_BSY_DDRX &=~ (1 << CHK_GPU_BSY_PXY); // set as input
  CHK_GPU_BSY_PORTX |= (1 << CHK_GPU_BSY_PXY); // pull-up
#endif
  
  uartSetup(baud);

  while(!syncEstablished) {
    while(serialAvailable()==0){
      uartSendArray(syncData, 0x02);  // two bytes
      _delayMS(1000);
    }

    if(serialRead() == SYNC_OK) {
      syncEstablished = true;
      
#if USE_GPU_RETURN_RESOLUTION
      while(serialAvailable() < 3); // wait for resolution
      // get _width
      cmdBuffer.data[1] = serialRead();
      cmdBuffer.data[2] = serialRead();
      // get _height
      cmdBuffer.data[3] = serialRead();
      cmdBuffer.data[4] = serialRead();
      
      _width  = cmdBuffer.par1;
      _height = cmdBuffer.par2;
      
      serialClear();
#endif
    }
  }
}

// ------------------------------------------------------------------------------------ //
/* this function is abstruction layer
 * this allow to simply change the interface
 * and have some protection rules
 */
void sendCommand(void *buf, uint8_t size)
{
#if !REMOVE_HARDWARE_BSY // harware protection
  while(CHK_GPU_BSY_PIN); // wait untill GPU buffer will ready
#else // software protection
  if(serialRead() == BSY_MSG_CODE_WAIT) {
    while (serialRead() != BSY_MSG_CODE_READY);
  }
#endif
  
  uartSendArray((uint8_t*)buf, size);
}

// ------------------------------------------------------------------------------------ //


// ------------------ Base ------------------ //
void gpuDrawPixel(int16_t x, int16_t y, uint16_t color)
{
  cmdBuffer.cmd = DRW_PIXEL;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = color;

  sendCommand(cmdBuffer.data, 7);
}

void gpuFillScreen(uint16_t color)
{
  cmdBuffer.cmd = FLL_SCR;
  cmdBuffer.par1 = color;

  sendCommand(cmdBuffer.data, 3);
}

// ------------- Primitives/GFX ------------- //

void gpuFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  cmdBuffer.cmd = FLL_RECT;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = h;
  cmdBuffer.par5 = color;

  sendCommand(cmdBuffer.data, 11);
}

void gpuDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  cmdBuffer.cmd = DRW_RECT;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = h;
  cmdBuffer.par5 = color;

  sendCommand(cmdBuffer.data, 11);
}

void gpuDrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, uint16_t color)
{
  cmdBuffer.cmd = DRW_ROUND_RECT;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = h;
  cmdBuffer.par5 = radius;
  cmdBuffer.par6 = color;
  
  sendCommand(cmdBuffer.data, 13);
}

void gpuFillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, uint16_t color)
{
  cmdBuffer.cmd = FLL_ROUND_RECT;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = h;
  cmdBuffer.par5 = radius;
  cmdBuffer.par6 = color;
  
  sendCommand(cmdBuffer.data, 13);
}

void gpuDrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  cmdBuffer.cmd = DRW_LINE;
  cmdBuffer.par1 = x0;
  cmdBuffer.par2 = y0;
  cmdBuffer.par3 = x1;
  cmdBuffer.par4 = y1;
  cmdBuffer.par5 = color;

  sendCommand(cmdBuffer.data, 11);
}

void gpuDrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  cmdBuffer.cmd = DRW_V_LINE;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = h;
  cmdBuffer.par4 = color;

  sendCommand(cmdBuffer.data, 9);
}

void gpuDrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  cmdBuffer.cmd = DRW_H_LINE;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = color;

  sendCommand(cmdBuffer.data, 9);
}

void gpuDrawCircle(int16_t x, int16_t y, int16_t r, uint16_t color)
{
  cmdBuffer.cmd = DRW_CIRCLE;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = r;
  cmdBuffer.par4 = color;

  sendCommand(cmdBuffer.data, 9);
}

void gpuFillCircle(int16_t x, int16_t y, int16_t r, uint16_t color)
{
  cmdBuffer.cmd = FLL_CIRCLE;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = r;
  cmdBuffer.par4 = color;

  sendCommand(cmdBuffer.data, 9);
}

void gpuDrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  cmdBuffer.cmd = DRW_TRINGLE;
  cmdBuffer.par1 = x0;
  cmdBuffer.par2 = y0;
  cmdBuffer.par3 = x1;
  cmdBuffer.par4 = y1;
  cmdBuffer.par5 = x2;
  cmdBuffer.par6 = y2;
  cmdBuffer.par7 = color;

  sendCommand(cmdBuffer.data, 15);
}

void gpuFillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  cmdBuffer.cmd = FLL_TRINGLE;
  cmdBuffer.par1 = x0;
  cmdBuffer.par2 = y0;
  cmdBuffer.par3 = x1;
  cmdBuffer.par4 = y1;
  cmdBuffer.par5 = x2;
  cmdBuffer.par6 = y2;
  cmdBuffer.par7 = color;

  sendCommand(cmdBuffer.data, 15);
}

#if USE_GPU_RETURN_RESOLUTION
void gpuGetResolution(void)
{
  serialClear();
  uartSendByte(GET_RESOLUTION);
  
  while(serialAvailable() < 3); // wait for resolution
  // get _width
  cmdBuffer.data[1] = serialRead();
  cmdBuffer.data[2] = serialRead();
  // get _height
  cmdBuffer.data[3] = serialRead();
  cmdBuffer.data[4] = serialRead();
  
  _width  = cmdBuffer.par1;
  _height = cmdBuffer.par2;
}

int16_t gpuWidth(void)
{
  return _width;
}

int16_t gpuHeight(void)
{
  return _height;
}

#else

void getResolution(void)
{
}

int16_t gpuWidth(void)
{
  return GPU_LCD_W;
}

int16_t gpuHeight(void)
{
  return GPU_LCD_H;
}
#endif

// --------------- Font/Print --------------- //

void gpuDrawChar(int16_t x, int16_t y, uint8_t c, uint16_t color, uint16_t bg, uint8_t size)
{
  cmdBuffer.cmd = DRW_CHAR;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = color;
  cmdBuffer.par4 = bg;
  cmdBuffer.data[9] = c;
  cmdBuffer.data[10] = size;

  sendCommand(cmdBuffer.data, 11);
}

void gpuSetCursor(int16_t x, int16_t y)
{
  cmdBuffer.cmd = SET_CURSOR;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;

  sendCommand(cmdBuffer.data, 5);
}

void gpuSetTextColor(uint16_t color)
{
  cmdBuffer.cmd = SET_TXT_CR;
  cmdBuffer.par1 = color;

  sendCommand(cmdBuffer.data, 3);
}

void gpuSetTextColorBG(uint16_t color, uint16_t bg)
{
  cmdBuffer.cmd = SET_TXT_CR_BG;
  cmdBuffer.par1 = color;
  cmdBuffer.par2 = bg;

  sendCommand(cmdBuffer.data, 5);
}

void gpuSetTextSize(uint8_t size)
{
  cmdBuffer.cmd = SET_TXT_SIZE;
  cmdBuffer.data[1] = size;

  sendCommand(cmdBuffer.data, 2);
}

void gpuSetTextWrap(bool wrap)
{
  cmdBuffer.cmd = SET_TXT_WRAP;
  cmdBuffer.data[1] = wrap;

  sendCommand(cmdBuffer.data, 2);
}

void gpuSetCp437(bool cp)
{
  cmdBuffer.cmd = SET_TXT_437;
  cmdBuffer.data[1] = cp;

  sendCommand(cmdBuffer.data, 2);
}

void gpuPrint(const char *str)
{
  cmdBuffer.cmd = DRW_PRNT;
  cmdBuffer.data[1] = strlen(str);
  
  sendCommand(cmdBuffer.data, 2);
  sendCommand((void*)str, cmdBuffer.data[1]);
}

void gpuPrintLen(const char *str, uint16_t size)
{
  cmdBuffer.cmd = DRW_PRNT;
  cmdBuffer.data[1] = size;
  
  sendCommand(cmdBuffer.data, 2);
  sendCommand((void*)str, size);
}

// make a DDoS to GPU's buffer...
void gpuPrint_P(const char *str)
{
  uint16_t strSize = strlen_P(str);
  
  for (uint16_t count=0; count < strSize; count++) {
    cmdBuffer.cmd = DRW_PRNT_C;
    cmdBuffer.par1 = pgm_read_byte(str + count);
    
    sendCommand(cmdBuffer.data, 2);
  }
}

void gpuPrintChar(uint8_t c)
{
  cmdBuffer.cmd = DRW_PRNT_C;
  cmdBuffer.data[1] = c;

  sendCommand(cmdBuffer.data, 2);
}

void gpuPrintCharAt(int16_t x, int16_t y, uint8_t c)
{
  cmdBuffer.cmd = DRW_PRNT_POS_C;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.data[5] = c;

  sendCommand(cmdBuffer.data, 6);
}


// ---------------- Low Level --------------- //
void gpuSetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  cmdBuffer.cmd = SET_ADR_WIN;
  cmdBuffer.par1 = x0;
  cmdBuffer.par2 = y0;
  cmdBuffer.par3 = x1;
  cmdBuffer.par4 = y1;
  
  sendCommand(cmdBuffer.data, 9);
}

void gpuSetRotation(uint8_t m)
{
  cmdBuffer.cmd = SET_ROTATION;
  cmdBuffer.data[1] = m;
  
  sendCommand(cmdBuffer.data, 2);
}

void gpuSetScrollArea(uint16_t TFA, uint16_t BFA)
{
  cmdBuffer.cmd = SET_SCRL_AREA;
  cmdBuffer.par1 = TFA;
  cmdBuffer.par2 = BFA;
  
  sendCommand(cmdBuffer.data, 5);
}

void gpuScrollAddress(uint16_t VSP)
{
  cmdBuffer.cmd = SET_V_SCRL_ADR;
  cmdBuffer.par1 = VSP;
  
  sendCommand(cmdBuffer.data, 3);
}

void gpuScroll(uint16_t lines, uint16_t yStart)
{
  //uint16_t newYstart;
  
  cmdBuffer.cmd = MAK_SCRL;
  cmdBuffer.par1 = lines;
  cmdBuffer.par2 = yStart;
  
  sendCommand(cmdBuffer.data, 5);
  //return newYstart;
}
/*
void gpuScrollSmooth(uint16_t lines, uint16_t yStart, uint8_t wait)
{
  //uint16_t newYstart;
  
  cmdBuffer.cmd = MAK_SCRL_SMTH;
  cmdBuffer.par1 = lines;
  cmdBuffer.par2 = yStart;
  cmdBuffer.data[5] = wait;
  
  sendCommand(cmdBuffer.data, 6);
  //return newYstart;
}
*/

void gpuSetSleep(bool enable)
{
  cmdBuffer.cmd = SET_SLEEP;
  cmdBuffer.data[1] = enable;
  
  sendCommand(cmdBuffer.data, 2);
}

void gpuSetIdleMode(bool mode)
{
  cmdBuffer.cmd = SET_IDLE;
  cmdBuffer.data[1] = mode;
  
  sendCommand(cmdBuffer.data, 2);
}

void gpuSetDispBrightness(uint8_t brightness)
{
  cmdBuffer.cmd = SET_BRIGHTNES;
  cmdBuffer.data[1] = brightness;
  
  sendCommand(cmdBuffer.data, 2);
}

void gpuSetInvertion(bool i)
{
  cmdBuffer.cmd = SET_INVERTION;
  cmdBuffer.data[1] = i;
  
  sendCommand(cmdBuffer.data, 2);
}

//void gpuSetGamma(uint8_t gamma);
// SET_GAMMA

void gpuPushColor(uint16_t color)
{
  cmdBuffer.cmd = PSH_CR;
  cmdBuffer.par1 = color;
  
  sendCommand(cmdBuffer.data, 3);
}

void gpuWriteCommand(uint8_t c)
{
  cmdBuffer.cmd = WRT_CMD;
  cmdBuffer.data[1] = c;
  
  sendCommand(cmdBuffer.data, 2);
}

void gpuWriteData(uint8_t d)
{
  cmdBuffer.cmd = WRT_DATA;
  cmdBuffer.data[1] = d;
  
  sendCommand(cmdBuffer.data, 2);
}

void gpuWriteWordData(uint16_t c)
{
  cmdBuffer.cmd = WRT_DATA_U16;
  cmdBuffer.par1 = c;
  
  sendCommand(cmdBuffer.data, 3);
}

// ------------------- Tile ----------------- //
void sendTileData(uint8_t tileType, int16_t posX, int16_t posY, uint8_t tileNum)
{
  cmdBuffer.cmd = tileType;
  cmdBuffer.par1 = posX;
  cmdBuffer.par2 = posY;
  cmdBuffer.data[5] = tileNum;
  
  sendCommand(cmdBuffer.data, 6);
}

void loadTileBase(uint8_t tileType, const char *tileSetArrName, uint8_t tileSetW,
                          uint8_t ramTileNum, uint8_t tileNum)
{
  cmdBuffer.cmd = tileType;
  cmdBuffer.data[1] = strlen(tileSetArrName);
  cmdBuffer.data[2] = tileSetW;
  cmdBuffer.data[3] = ramTileNum;
  cmdBuffer.data[4] = tileNum;
  
  sendCommand(cmdBuffer.data, 5);
  sendCommand((void*)tileSetArrName, cmdBuffer.data[1]); // send name of file
}

void loadTilesBase(uint8_t tileType, const char *tileSetArrName, uint8_t tileSetW,
                          uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax)
{
  cmdBuffer.cmd = tileType;
  cmdBuffer.data[1] = strlen(tileSetArrName);
  cmdBuffer.data[2] = tileSetW;
  cmdBuffer.data[3] = ramTileBase;
  cmdBuffer.data[4] = tileMin;
  cmdBuffer.data[5] = tileMax;
  
  sendCommand(cmdBuffer.data, 6);
  sendCommand((void*)tileSetArrName, cmdBuffer.data[1]); // send name of file
}

// ---- tile 8x8 ---- //
void gpuLoadTile8x8(const char *tileSetArrName, uint8_t tileSetW, uint8_t ramTileNum, uint8_t tileNum)
{
  loadTileBase(LDD_TLE_8, tileSetArrName, tileSetW, ramTileNum, tileNum);
}

void gpuLoadTileSet8x8(const char *tileSetArrName, uint8_t tileSetW, uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax)
{
  loadTilesBase(LDD_TLES_8, tileSetArrName, tileSetW, ramTileBase, tileMin, tileMax);
}

void gpuDrawTile8x8(int16_t posX, int16_t posY, uint8_t tileNum)
{
  sendTileData(DRW_TLE_8, posX, posY, tileNum);
}

// ---- tile 16x16 ---- //
void gpuLoadTile16x16(const char *tileSetArrName, uint8_t tileSetW, uint8_t ramTileNum, uint8_t tileNum)
{
  loadTileBase(LDD_TLE_16, tileSetArrName, tileSetW, ramTileNum, tileNum);
}

void gpuLoadTileSet16x16(const char *tileSetArrName, uint8_t tileSetW, uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax)
{
  loadTilesBase(LDD_TLES_16, tileSetArrName, tileSetW, ramTileBase, tileMin, tileMax);
}

void gpuDrawTile16x16(int16_t posX, int16_t posY, uint8_t tileNum)
{
  sendTileData(DRW_TLE_16, posX, posY, tileNum);
}

// ---- tile 32x32 ---- //
void gpuLoadTile32x32(const char *tileSetArrName, uint8_t tileSetW, uint8_t ramTileNum, uint8_t tileNum)
{
  loadTileBase(LDD_TLE_32, tileSetArrName, tileSetW, ramTileNum, tileNum);
}

void gpuLoadTileSet32x32(const char *tileSetArrName, uint8_t tileSetW, uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax)
{
  loadTilesBase(LDD_TLES_32, tileSetArrName, tileSetW, ramTileBase, tileMin, tileMax);
}

void gpuDrawTile32x32(int16_t posX, int16_t posY, uint8_t tileNum)
{
  sendTileData(DRW_TLE_32, posX, posY, tileNum);
}

void gpuLoadTileMap(const char *fileName)
{
  cmdBuffer.cmd = LDD_TLE_MAP;
  cmdBuffer.data[1] = strlen(fileName);
  
  sendCommand(cmdBuffer.data, 2);
  sendCommand((void*)fileName, cmdBuffer.data[1]); // send name of file
}

void gpuDrawTileMap(void)
{
  cmdBuffer.cmd = DRW_TLE_MAP;
  
  sendCommand(cmdBuffer.data, 1);
}

// ----------------- Sprite ----------------- //
void gpuSetSpritePosition(uint8_t sprNum, uint16_t posX, uint16_t posY)
{
  cmdBuffer.cmd = SET_SPR_POS;
  cmdBuffer.par1 = (uint16_t)sprNum;
  cmdBuffer.par2 = posX;
  cmdBuffer.par3 = posY;
  
  sendCommand(cmdBuffer.data, 7);
}

void gpuSetSpriteType(uint8_t sprNum, uint8_t type)
{
  cmdBuffer.cmd = SET_SPR_TYPE;
  cmdBuffer.data[1] = sprNum;
  cmdBuffer.data[2] = type;
  
  sendCommand(cmdBuffer.data, 3);
}

void gpuSetSpriteVisible(uint8_t sprNum, uint8_t state)
{
  cmdBuffer.cmd = SET_SPR_VISBL;
  cmdBuffer.data[1] = sprNum;
  cmdBuffer.data[2] = state;
  
  sendCommand(cmdBuffer.data, 3);
}

void gpuSetSpriteTiles(uint8_t sprNum, uint8_t tle1, uint8_t tle2, uint8_t tle3, uint8_t tle4)
{
  cmdBuffer.cmd = SET_SPR_TLE;
  cmdBuffer.data[1] = sprNum;
  cmdBuffer.data[2] = tle1;
  cmdBuffer.data[3] = tle2;
  cmdBuffer.data[4] = tle3;
  cmdBuffer.data[5] = tle4;
  
  sendCommand(cmdBuffer.data, 6);
}

void gpuSetSpritesAutoRedraw(uint8_t state)
{
  cmdBuffer.cmd = SET_SPR_AUT_R;
  cmdBuffer.data[1] = state;
  
  sendCommand(cmdBuffer.data, 2);
}

void gpuDrawSprite(uint8_t sprNum)
{
  cmdBuffer.cmd = DRW_SPR;
  cmdBuffer.data[1] = sprNum;
  
  sendCommand(cmdBuffer.data, 2);
}

// in future i fix that...
void gpuDrawSpriteAt(uint8_t sprNum, uint16_t posX, uint16_t posY)
{
  gpuSetSpritePosition(sprNum, posX, posY);
  
  cmdBuffer.cmd = DRW_SPR;
  cmdBuffer.data[1] = sprNum;
  
  sendCommand(cmdBuffer.data, 2);
}

bool gpuGetSpriteCollision(uint8_t sprNum1, uint8_t sprNum2)
{
  cmdBuffer.cmd = GET_SRP_COLISN;
  cmdBuffer.data[1] = sprNum1;
  cmdBuffer.data[2] = sprNum2;
  
  sendCommand(cmdBuffer.data, 3);
  
  while(!serialAvailable() ); // wait for state

  return serialRead();
}


// ---------------- SD card ----------------- //
void gpuLoadPalette(const char *palleteArrName)
{
  cmdBuffer.cmd = LDD_USR_PAL;
  cmdBuffer.data[1] = strlen(palleteArrName);
  
  sendCommand(cmdBuffer.data, 2);
  sendCommand((void*)palleteArrName, cmdBuffer.data[1]); // send name of file
}

void sendBaseBMP(uint16_t x, uint16_t y, uint16_t size)
{
  cmdBuffer.cmd = DRW_BMP_FIL;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.data[5] = size;
  
  sendCommand(cmdBuffer.data, 6);
}

void gpuPrintBMP(const char *fileName)
{
  sendBaseBMP(0, 0, strlen(fileName));
  sendCommand((void*)fileName, cmdBuffer.data[5]); // send name of file
}

void gpuPrintBMPat(uint16_t x, uint16_t y, const char *fileName)
{
  sendBaseBMP(x, y, strlen(fileName));
  sendCommand((void*)fileName, cmdBuffer.data[5]); // send name of file
}

// ------------------ Sound ----------------- //
void gpuPlayNote(uint16_t freq, uint16_t duration)
{
  cmdBuffer.cmd = SND_PLAY_TONE;
  cmdBuffer.par1 = freq;
  cmdBuffer.par2 = duration;
  
  sendCommand(cmdBuffer.data, 5);
}

// --------------- GUI commands -------------- //
void gpuSetTextSizeGUI(uint8_t size)
{
  cmdBuffer.cmd = SET_WND_TXT_SZ;
  cmdBuffer.data[1] = size;
  
  sendCommand(cmdBuffer.data, 2);
}

void gpuSetTextColorGUI(uint16_t text, uint16_t bg)
{
  cmdBuffer.cmd = SET_WND_CR_TXT;
  cmdBuffer.par1 = text;
  cmdBuffer.par2 = bg;
  
  sendCommand(cmdBuffer.data, 5);
}

void gpuSetColorWindowGUI(uint16_t frame, uint16_t border)
{
  cmdBuffer.cmd = SET_WND_CR;
  cmdBuffer.par1 = frame;
  cmdBuffer.par2 = border;
  
  sendCommand(cmdBuffer.data, 5);
}

void gpuDrawWindowGUI(int16_t posX, int16_t posY, int16_t w, int16_t h)
{
  cmdBuffer.cmd = DRW_WND_AT;
  cmdBuffer.par1 = posX;
  cmdBuffer.par2 = posY;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = h;
  
  sendCommand(cmdBuffer.data, 9);
}

void gpuDrawTextWindowGUI(int16_t posX, int16_t posY,
                          int16_t w, int16_t h, const char *text)
{
  cmdBuffer.cmd = DRW_WND_TXT;
  cmdBuffer.par1 = posX;
  cmdBuffer.par2 = posY;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = h;
  cmdBuffer.data[9] = strlen(text);
  
  sendCommand(cmdBuffer.data, 10);
  sendCommand((void*)text, cmdBuffer.par5);
}

void gpuDrawTextWindowGUI_P(int16_t posX, int16_t posY,
                             int16_t w, int16_t h, const char *text)
{
  cmdBuffer.cmd = DRW_WND_TXT;
  cmdBuffer.par1 = posX;
  cmdBuffer.par2 = posY;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = h;
  cmdBuffer.data[9] = strlen_P(text);
  
  sendCommand(cmdBuffer.data, 10);
  
  for (uint8_t count=0; count < cmdBuffer.data[9]; count++) {
    sendCommand(pgm_read_byte(text + count), 1);
  }
}

// --------------- '3D' engine --------------- //
void gpuRenderFrame(void)
{
  // yep... only 1 byte. I make this for protect sGPU from DDoS.
  cmdBuffer.cmd = RENDER_MAP;
  sendCommand(cmdBuffer.data, 1);
}

void gpuMoveCamera(uint8_t direction)
{
  cmdBuffer.cmd = MOVE_CAMERA;
  cmdBuffer.data[1] = direction;
  
  sendCommand(cmdBuffer.data, 2);
}

void gpuSetCamPosition(uint16_t posX, uint16_t posY, uint16_t angle)
{
  cmdBuffer.cmd = SET_CAM_POS;
  cmdBuffer.par1 = posX;
  cmdBuffer.par2 = posY;
  cmdBuffer.par3 = angle;
  
  sendCommand(cmdBuffer.data, 7);
}

void gpuSetWallCollision(bool state)
{
  cmdBuffer.cmd = SET_WALL_CLD;
  cmdBuffer.data[1] = state;
  
  sendCommand(cmdBuffer.data, 2);
}

void gpuGetCamPosition(uint16_t *pArrPos)
{
  cmdBuffer.cmd = GET_CAM_POS;
  sendCommand(cmdBuffer.data, 1);
  
  while(serialAvailable < 6) { // wait for: posX, posY and angle
    pArrPos[0] = serialRead();
    pArrPos[0] |= serialRead()<<8;
    
    pArrPos[1] = serialRead();
    pArrPos[1] |= serialRead()<<8;
    
    pArrPos[2] = serialRead();
    pArrPos[2] |= serialRead()<<8;
  }
}

void gpuSetSkyFloor(uint16_t sky, uint16_t floor)
{
  cmdBuffer.cmd = SET_BACKGRND;
  cmdBuffer.par1 = sky;
  cmdBuffer.par2 = floor;
  
  sendCommand(cmdBuffer.data, 5);
}

// ------------------ General ----------------- //
#if !REMOVE_HARDWARE_BSY
// software or hardware
void setBusyMode(bool state)
{
  cmdBuffer.cmd = BSY_SELECT;
  cmdBuffer.data[1] = state;
  
  sendCommand(cmdBuffer.data, 2);
}
#endif

void pingCommand(void)
{
  cmdBuffer.cmd = CMD_GPU_PING;
  sendCommand(cmdBuffer.data, 1);

  bool waitPingAnswer = true;
  while(waitPingAnswer) {
      while(serialAvailable()==0);

      if(serialRead() == GPU_MSG_CODE_PING) {
        waitPingAnswer = false;
    }
  }
}

void swReset(void)
{
  cmdBuffer.cmd = CMD_GPU_SW_RESET;
  sendCommand(cmdBuffer.data, 1);
}

void setDebugGPIOState(bool state)
{
  cmdBuffer.cmd = SET_DBG_GPIO_PIN;
  cmdBuffer.data[1] = state;
  
  sendCommand(cmdBuffer.data, 2);
}

// -------------------- ___ ---------------------- //

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Draw a 1-bit image (bitmap) at the specified (x,y) position from the
// provided bitmap buffer (must be PROGMEM memory) using the specified
// foreground color (unset bits are transparent).
void gpuDrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
                        int16_t w, int16_t h, uint16_t color)
{
  
  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++) {
      if(i & 7) byte <<= 1;
      else      byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
      if(byte & 0x80) gpuDrawPixel(x+i, y+j, color);
    }
  }
}

// Draw a 1-bit image (bitmap) at the specified (x,y) position from the
// provided bitmap buffer (must be PROGMEM memory) using the specified
// foreground (for set bits) and background (for clear bits) colors.
void gpuDrawBitmapBG(int16_t x, int16_t y, const uint8_t *bitmap,
                        int16_t w, int16_t h, uint16_t color, uint16_t bg)
{
  
  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(i & 7) byte <<= 1;
      else      byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
      if(byte & 0x80) gpuDrawPixel(x+i, y+j, color);
      else            gpuDrawPixel(x+i, y+j, bg);
    }
  }
}

void gpuDrawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
  int16_t i, j, byteWidth = (w + 7) / 8;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i % 8))) {
        gpuDrawPixel(x+i, y+j, color);
      }
    }
  }
}
