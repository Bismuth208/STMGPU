/*
 *
 * For STM32_GPU Project
 * Creation start: 10.04.2016 20:21 (UTC+4)
 *
 * Created by: Antonov Alexandr (Bismuth208)
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

#include "gpu_gfx.h"

#define SYNC_SEQUENCE   0x42DD
#define SYNC_OK         0xCC

#define MAX_TEXT_SIZE   30

#define CHK_GPU_BSY_PIN     (PIND & (1 << PD2)) // check bsy GPU pin

// ------------------------------------------------------------------------------------ //

static cmdBuffer_t cmdBuffer;
static cmdBuffer2_t cmd_T_Buf;
//static uint8_t cmdBufferStr[MAX_TEXT_SIZE];

// at sync, GPU return it`s LCD resolution,
// but you can ask GPU once again
static int16_t _width  = 0;
static int16_t _height = 0;

// ------------------------------------------------------------------------------------ //

void sync_gpu(void)
{
  uint8_t syncData[2] = { 0x42, 0xDD};

  bool syncEstablished = false;
  
  // setup GPU bsy pin
  DDRD &=~ (1 << PD2); // set as input
  PORTD |= (1 << PD2); // pull-up

  while(!syncEstablished) {
    while(serialAvailable()==0){
      uartSendArray(syncData, 0x02);
      _delayMS(1000);
    }

    if(serialRead() == SYNC_OK) {
      syncEstablished = true;
      
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
    }
  }
}

// ------------------------------------------------------------------------------------ //
void sendCommand(void *buf, uint8_t size)
{
  while(CHK_GPU_BSY_PIN); // wait untill GPU buffer will ready
  
  uartSendArray((uint8_t*)buf, size);
}

// ------------------------------------------------------------------------------------ //


// ------------------ Base ------------------ //

void tftDrawPixel(int16_t x, int16_t y, uint16_t color)
{
  cmdBuffer.cmd = DRW_PIXEL;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = color;

  sendCommand(cmdBuffer.data, 7);
}

void tftFillScreen(uint16_t color)
{
  cmdBuffer.cmd = FLL_SCR;
  cmdBuffer.par1 = color;

  sendCommand(cmdBuffer.data, 3);
}

// ------------- Primitives/GFX ------------- //

void tftFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  cmdBuffer.cmd = FLL_RECT;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = h;
  cmdBuffer.par5 = color;

  sendCommand(cmdBuffer.data, 11);
}

void tftDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  cmdBuffer.cmd = DRW_RECT;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = h;
  cmdBuffer.par5 = color;

  sendCommand(cmdBuffer.data, 11);
}

void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, uint16_t color)
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

void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, uint16_t color)
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

void tftDrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  cmdBuffer.cmd = DRW_LINE;
  cmdBuffer.par1 = x0;
  cmdBuffer.par2 = y0;
  cmdBuffer.par3 = x1;
  cmdBuffer.par4 = y1;
  cmdBuffer.par5 = color;

  sendCommand(cmdBuffer.data, 11);
}

void tftDrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  cmdBuffer.cmd = DRW_V_LINE;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = h;
  cmdBuffer.par4 = color;

  sendCommand(cmdBuffer.data, 9);
}

void tftDrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  cmdBuffer.cmd = DRW_H_LINE;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = w;
  cmdBuffer.par4 = color;

  sendCommand(cmdBuffer.data, 9);
}

void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color)
{
  cmdBuffer.cmd = DRW_CIRCLE;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = r;
  cmdBuffer.par4 = color;

  sendCommand(cmdBuffer.data, 9);
}

void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color)
{
  cmdBuffer.cmd = FLL_CIRCLE;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;
  cmdBuffer.par3 = r;
  cmdBuffer.par4 = color;

  sendCommand(cmdBuffer.data, 9);
}

void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
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

void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
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

void getResolution(void)
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

int16_t tftHeight(void)
{
  return _height;
}

int16_t tftWidth(void)
{
  return _width;
}

// --------------- Font/Print --------------- //

void drawChar(int16_t x, int16_t y, uint8_t c, uint16_t color, uint16_t bg, uint8_t size)
{
  
  cmd_T_Buf.cmd = DRW_CHAR;
  cmd_T_Buf.par0 = c;
  cmd_T_Buf.par1 = x;
  cmd_T_Buf.par2 = y;
  cmd_T_Buf.par3 = color;
  cmd_T_Buf.par4 = bg;
  cmd_T_Buf.par5 = size;

  sendCommand(cmd_T_Buf.data, 11);
   
  /*
  setTextSize(size);
  //setCursor(x, y);
  setTextColorBG(color, bg);
  //printChar(c);
   
  printCharPos(x, y, c);
   */
}

void setCursor(int16_t x, int16_t y)
{
  cmdBuffer.cmd = SET_CURSOR;
  cmdBuffer.par1 = x;
  cmdBuffer.par2 = y;

  sendCommand(cmdBuffer.data, 5);
}

void setTextColor(uint16_t color)
{
  cmdBuffer.cmd = SET_TXT_CR;
  cmdBuffer.par1 = color;

  sendCommand(cmdBuffer.data, 3);
}

void setTextColorBG(uint16_t color, uint16_t bg)
{
  cmdBuffer.cmd = SET_TXT_CR_BG;
  cmdBuffer.par1 = color;
  cmdBuffer.par2 = bg;

  sendCommand(cmdBuffer.data, 5);
}

void setTextSize(uint8_t size)
{
  cmdBuffer.cmd = SET_TXT_SIZE;
  cmdBuffer.par1 = size;

  sendCommand(cmdBuffer.data, 2);
}

void setTextWrap(bool wrap)
{
  cmdBuffer.cmd = SET_TXT_WRAP;
  cmdBuffer.par1 = wrap;

  sendCommand(cmdBuffer.data, 2);
}

void cp437(bool cp)
{
  cmdBuffer.cmd = SET_TXT_437;
  cmdBuffer.par1 = cp;

  sendCommand(cmdBuffer.data, 2);
}

/*
void print(const char *str)
{
  uint8_t strSize = strlen(str);
  
  cmdBufferStr[0] = DRW_PRNT;
  cmdBufferStr[1] = DRW_PRNT;
  
  memcpy(cmdBufferStr[2], str, strSize);
  
  sendCommand(cmdBufferStr, strSize + 0x02);
}
 
void tftPrintPGR(const char *str)
{
 
}
*/

// make a DDoS to GPU's buffer...
void print(const char *str)
{
  uint16_t strSize = strlen(str);
  
  for (uint16_t count=0; count < strSize; count++) {
    cmdBuffer.cmd = DRW_PRNT_C;
    cmdBuffer.par1 = str[count];
    
    sendCommand(cmdBuffer.data, 2);
  }
}

// make a DDoS to GPU's buffer...
void tftPrintPGR(const char *str)
{
  uint16_t strSize = strlen_P(str);
  
  for (uint16_t count=0; count < strSize; count++) {
    cmdBuffer.cmd = DRW_PRNT_C;
    cmdBuffer.par1 = pgm_read_byte(str + count);
    
    sendCommand(cmdBuffer.data, 2);
  }
}

void printChar(uint8_t c)
{
  cmdBuffer.cmd = DRW_PRNT_C;
  cmdBuffer.par1 = c;

  sendCommand(cmdBuffer.data, 2);
}

void printCharPos(int16_t x, int16_t y, uint8_t c)
{
  cmd_T_Buf.cmd = DRW_PRNT_POS_C;
  cmd_T_Buf.par0 = c;
  cmd_T_Buf.par1 = x;
  cmd_T_Buf.par2 = y;

  sendCommand(cmd_T_Buf.data, 6);
}


// ---------------- Low Level --------------- //
void tftSetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  cmdBuffer.cmd = SET_ADR_WIN;
  cmdBuffer.par1 = x0;
  cmdBuffer.par2 = x0;
  cmdBuffer.par3 = x1;
  cmdBuffer.par4 = y1;
  
  sendCommand(cmdBuffer.data, 9);
}

void tftSetRotation(uint8_t m)
{
  cmdBuffer.cmd = SET_ROTATION;
  cmdBuffer.par1 = m;
  
  sendCommand(cmdBuffer.data, 2);
}

void tftSetScrollArea(uint16_t TFA, uint16_t BFA)
{
  cmdBuffer.cmd = SET_SCRL_AREA;
  cmdBuffer.par1 = TFA;
  cmdBuffer.par2 = BFA;
  
  sendCommand(cmdBuffer.data, 5);
}

void tftScrollAddress(uint16_t VSP)
{
  cmdBuffer.cmd = SET_V_SCRL_ADR;
  cmdBuffer.par1 = VSP;
  
  sendCommand(cmdBuffer.data, 3);
}

uint16_t tftScroll(uint16_t lines, uint16_t yStart)
{
  cmdBuffer.cmd = MAK_SCRL;
  cmdBuffer.par1 = lines;
  cmdBuffer.par2 = yStart;
  
  sendCommand(cmdBuffer.data, 5);
}

uint16_t tftScrollSmooth(uint16_t lines, uint16_t yStart, uint8_t wait)
{
  cmd_T_Buf.cmd = MAK_SCRL_SMTH;
  cmd_T_Buf.par0 = wait;
  cmd_T_Buf.par1 = lines;
  cmd_T_Buf.par2 = yStart;
  
  sendCommand(cmdBuffer.data, 6);
}

void tftSetSleep(bool enable)
{
  cmdBuffer.cmd = SET_SLEEP;
  cmdBuffer.par1 = enable;
  
  sendCommand(cmdBuffer.data, 2);
}

void tftSetIdleMode(bool mode)
{
  cmdBuffer.cmd = SET_IDLE;
  cmdBuffer.par1 = mode;
  
  sendCommand(cmdBuffer.data, 2);
}

void tftSetDispBrightness(uint8_t brightness)
{
  cmdBuffer.cmd = SET_BRIGHTNES;
  cmdBuffer.par1 = brightness;
  
  sendCommand(cmdBuffer.data, 2);
}

void tftSetInvertion(bool i)
{
  cmdBuffer.cmd = SET_INVERTION;
  cmdBuffer.par1 = i;
  
  sendCommand(cmdBuffer.data, 2);
}

//void setGamma(uint8_t gamma);
// SET_GAMMA

void tftPushColor(uint16_t color)
{
  cmdBuffer.cmd = PSH_CR;
  cmdBuffer.par1 = color;
  
  sendCommand(cmdBuffer.data, 3);
}

void writeCommand(uint8_t c)
{
  cmdBuffer.cmd = WRT_CMD;
  cmdBuffer.par1 = c;
  
  sendCommand(cmdBuffer.data, 2);
}

void writeData(uint8_t d)
{
  cmdBuffer.cmd = WRT_DATA;
  cmdBuffer.par1 = d;
  
  sendCommand(cmdBuffer.data, 2);
}

void writeWordData(uint16_t c)
{
  cmdBuffer.cmd = WRT_DATA_U16;
  cmdBuffer.par1 = c;
  
  sendCommand(cmdBuffer.data, 3);
}

// --------------- Tile/Sprite -------------- //
void SDLoadTileFromSet8x8(const char *tileSetArrName, uint8_t tileSetW, uint8_t ramTileNum, uint8_t tileNum)
{
  cmdBuffer.cmd = LDD_TLE_8;
  cmdBuffer.data[1] = strlen(tileSetArrName);
  cmdBuffer.data[2] = tileSetW;
  cmdBuffer.data[3] = ramTileNum;
  cmdBuffer.data[4] = tileNum;
  
  sendCommand(cmdBuffer.data, 5);
  sendCommand(tileSetArrName, cmdBuffer.data[1]); // send name of file
}


void SDLoadTileSet8x8(const char *tileSetArrName, uint8_t tileSetW, uint8_t ramTileBase, uint8_t tileMax)
{
  cmdBuffer.cmd = LDD_TLES_8;
  cmdBuffer.data[1] = strlen(tileSetArrName);
  cmdBuffer.data[2] = tileSetW;
  cmdBuffer.data[3] = ramTileBase;
  cmdBuffer.data[4] = tileMax;
  
  sendCommand(cmdBuffer.data, 5);
  sendCommand(tileSetArrName, cmdBuffer.data[1]); // send name of file
}


void SDLoadRegionOfTileSet8x8(const char *tileSetArrName, uint8_t tileSetW, uint8_t ramTileBase, uint8_t tileMin, uint8_t tileMax)
{
  cmdBuffer.cmd = LDD_TLES_RG_8;
  cmdBuffer.data[1] = strlen(tileSetArrName);
  cmdBuffer.data[2] = tileSetW;
  cmdBuffer.data[3] = ramTileBase;
  cmdBuffer.data[4] = tileMin;
  cmdBuffer.data[5] = tileMax;
  
  sendCommand(cmdBuffer.data, 6);
  sendCommand(tileSetArrName, cmdBuffer.data[1]); // send name of file
}

void drawTile8x8(int16_t posX, int16_t posY, uint8_t tileNum)
{
  cmd_T_Buf.cmd = DRW_TLE_8_POS;
  cmd_T_Buf.par0 = tileNum;
  cmd_T_Buf.par1 = posX;
  cmd_T_Buf.par2 = posY;
  
  sendCommand(cmd_T_Buf.data, 6);
}

// -------------------- ___ ---------------------- //

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

//void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
//void drawBitmapBG(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg);
void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
  int16_t i, j, byteWidth = (w + 7) / 8;
  
  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i % 8))) {
        tftDrawPixel(x+i, y+j, color);
      }
    }
  }
}
